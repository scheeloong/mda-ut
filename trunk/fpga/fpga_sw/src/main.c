/*
 * main.c
 *
 * The guts of the FPGA processor
 *
 * Author: victor
 */

#include <string.h>
#include "altera_avalon_timer_regs.h"
#include "io.h"
#include "system.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"

#include "settings.h"
#include "utils.h"

// Victor, please determine where these lines should go, I need these global.
// Structures used by the PD controller for stabilization
struct orientation target_orientation;
struct orientation current_orientation;
struct orientation previous_orientation;
// Structure containing various PD parameters
struct PD_controller_inputs PD_controller_inputs;
// Structure containing PD controller feedback values
struct PD_controller_error_values PD_controller_error_values;
// Structure for debugging interrupt data, printf's taking too long
extern int DEBUG_interrupt[10];

// For power management
int power_failures[7] = {0};
int old_power_failures[7] = {0};

// This is the list of all the commands
// PLEASE KEEP THIS IN ALPHABETICAL ORDER
// Note: end the first string with a \n to ensure an exact match if no arguments are used
struct command_struct my_cmds[] = {
  {"ga\n", COMMAND_ACCEL, "ga - get acceleration\n  Usage: ga\n\n  Print (x,y,z) acceleration\n"},
  {"gax\n", COMMAND_ACCEL_X, "gax - get x-acceleration\n  Usage: gax\n\n  Print x-acceleration\n"},
  {"gay\n", COMMAND_ACCEL_Y, "gay - get y-acceleration\n  Usage: gay\n\n  Print y-acceleration\n"},
  {"gaz\n", COMMAND_ACCEL_Z, "gaz - get z-acceleration\n  Usage: gaz\n\n  Print z-acceleration\n"},
  {"gd\n", COMMAND_DEPTH, "gd - get depth\n  Usage: gd\n\n  Print depth (not converted)\n"},
  {"gg\n", COMMAND_GYRO, "gg - get gyroscope info\n  Usage: gg\n\n  Print (x,y,z) gyroscope info\n"},
  {"ggx\n", COMMAND_GYRO_X, "ggx - get x-gyroscope heading\n  Usage: ggx\n\n  Print x-gyroscope heading\n"},
  {"ggy\n", COMMAND_GYRO_Y, "ggy - get y-gyroscope heading\n  Usage: ggy\n\n  Print y-gyroscope heading\n"},
  {"ggz\n", COMMAND_GYRO_Z, "ggz - get z-gyroscope heading\n  Usage: ggz\n\n  Print z-gyroscope heading\n"},
  {"gm\n", COMMAND_MOTORS, "gm - get motor data\n  Usage: gm\n\n  Print all motor settings (direction on one line and duty cycle on the next)\n"},
  {"gmm", COMMAND_MOTORS_MIXED, ""},
  {"h", COMMAND_HELP, "h - help\n  Usage: h <cmd>\n\n  Print the help message for all commands that start with cmd, leave empty to print all help messages\n"},
  {"p", COMMAND_POW, "p - power off/on\n  Usage: p (0|1)\n\n Turn off/on power to all the voltage fails\n"},
  {"smd", COMMAND_DUTY_CYCLE, "smd - set motor duty cycle\n  Usage: smd <n> <0xdc>\n\n  Set the duty cycle of the nth motor to dc\nNote: the duty cycle is inputted in hex out of 0x3ff (1024 in decimal)\n"},
  {"smf", COMMAND_FORWARD, "smf - set motor forward\n  Usage: smf <n>\n\n  Turn on the nth motor in the forward direction\n"},
  {"smr", COMMAND_REVERSE, "smr - set motor reverse\n  Usage: smr <n>\n\n  Turn on the nth motor in the reverse direction\n"},
  {"sms", COMMAND_STOP, "sms - set motor stop\n  Usage: sms <n>\n\n  Turn the nth motor off\n"},
  {"smb", COMMAND_BRAKE, "smb - set motor brake\n  Usage: smb <n>\n\n  Turn the nth motor off\n"},
  {"spf", COMMAND_FREQ, "spf - set PWM frequency\n  Usage: spf <0xn>\n\n  Set the PWM frequency to n in kilohertz\nNote: the frequency is inputted in hex\n"},
  {"stop\n", COMMAND_STOP_ALL, "stop\n  Usage: stop\n\n  Stop all motors\n"},
  {"di\n", COMMAND_INTERRUPT_DEBUG, "di - print interrupt debug values\n  Usage: di\n"},
  {"ss", COMMAND_SPEED, "ss - set forward or backward speed of motor\n positive or negative, range from -157 to 157\n"},
  {"shc", COMMAND_HEADING_CHANGE, "shc - set heading change of motor\n positive or negative, range from -157 to 157\n"},
  {"svo", COMMAND_VERTICAL_OFFSET, "svo - set vertical offset of motor\n positive or negative, range from -157 to 157\n"}
};

// the size of the above array
int cmd_len = sizeof(my_cmds) / sizeof(struct command_struct);

// print a description of all available commands
void print_help(char *st)
{
  // ignore leading spaces
  while (*st == ' ') st++;

  printf("Command descriptions:\n\n");

  // -1 to ignore the \n at the end of st
  int len = strlen(st) - 1, i;
  if (len < 0) len = 0;

  for (i = 0; i < cmd_len; i++) {
    if (strncmp(st, my_cmds[i].name, len) == 0) {
      printf("%s\n", my_cmds[i].help_string);
    }
  }
}

// process a command
void process_command(char *st)
{
  // linearly search for the command from the table of commands
  enum COMMAND_ID cid = COMMAND_INVALID;
  int i;
  for (i = 0; i < cmd_len; i++) {
    if (strncmp(st, my_cmds[i].name, strlen(my_cmds[i].name)) == 0) {
      cid = my_cmds[i].id;
      st += strlen(my_cmds[i].name);
      break;
    }
  }

  // variables used in case statement
  struct t_accel_data accel_data;
  struct orientation orientation;
  int c, dc, x, y, z;
  int inputs[5];
  const int ALL = 10; // 10 is a in hex

  switch (cid) {
    case COMMAND_INVALID:
      printf("Sorry, command %s is not recognized. For a list of valid commands, type 'h' for help\n", st);
      break;
    case COMMAND_HELP:
      print_help(st+1);
      break;
    case COMMAND_POW:
      i = read_hex(st);
      set_pow(i);
      printf("setting power %s\n", (i % 2  == 0) ? "off" : "on");
      break;
    case COMMAND_STOP_ALL:
      target_orientation.speed = 0;
      // When the stop all command is received, maintain the current depth
      target_orientation.depth = get_depth();
      printf("stopping\n");
      break;
    case COMMAND_STOP:
      i = read_hex(st);
      if (i == ALL) {
        for (c = 0; c < NUM_MOTORS; c++) {
          set_motor_dir(c, MOTOR_DIR_STOPPED);
          printf("stopping motor %d\n", c);
        }
      } else if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        set_motor_dir(i, MOTOR_DIR_STOPPED);
        printf("stopping motor %d\n", i);
      }
      break;
    case COMMAND_BRAKE:
      i = read_hex(st);
      if (i == ALL) {
        for (c = 0; c < NUM_MOTORS; c++) {
          set_motor_dir(c, MOTOR_DIR_BRAKE);
          printf("braking motor %d\n", c);
        }
      } else if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        set_motor_dir(i, MOTOR_DIR_BRAKE);
        printf("braking motor %d\n", i);
      }
      break;
    case COMMAND_FORWARD:
      i = read_hex(st);
      if (i == ALL) {
        for (c = 0; c < NUM_MOTORS; c++) {
          set_motor_dir(c, MOTOR_DIR_FORWARD);
          printf("setting motor %d to forward\n", c);
        }
      } else if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        set_motor_dir(i, MOTOR_DIR_FORWARD);
        printf("setting motor %d to forward\n", i);
      }
      break;
    case COMMAND_REVERSE:
      i = read_hex(st);
      if (i == ALL) {
        for (c = 0; c < NUM_MOTORS; c++) {
          set_motor_dir(c, MOTOR_DIR_REVERSE);
          printf("setting motor %d to reverse\n", c);
        }
      } else if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        set_motor_dir(i, MOTOR_DIR_REVERSE);
        printf("setting motor %d to reverse\n", i);
      }
      break;
    case COMMAND_DUTY_CYCLE:
      i = read_hex(st);
      if (i == ALL) {
        dc = read_hex(st + 2);
        if (dc < 0) {
          printf("duty cycle # invalid\n");
        } else {
          for (c = 0; c < NUM_MOTORS; c++) {
            set_motor_duty_cycle(c, dc);
            printf("setting motor %d to duty cycle %d\n", c, dc);
          }
        }
      } else if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        dc = read_hex(st + 2);
        if (dc < 0) {
          printf("duty cycle # invalid\n");
        } else {
          set_motor_duty_cycle(i, dc);
          printf("setting motor %d to duty cycle %d\n", i, dc);
        }
      }
      break;
    case COMMAND_FREQ:
      i = read_hex(st);
      set_pwm_freq(i);
      printf("PWM frequency set to: %d\n", i);
      for (i = 0; i < NUM_MOTORS; i++) {
        set_motor_duty_cycle(i, get_motor_duty_cycle(i));
      }
      break;
    case COMMAND_ACCEL:
      get_accel(&accel_data, &orientation);
      printf("raw: ");
      print_int(accel_data.x);
      alt_putchar(',');
      print_int(accel_data.y);
      alt_putchar(',');
      print_int(accel_data.z);
      alt_putchar('\n');
      printf("in degrees: ");
      print_int(orientation.pitch >> 3);
      alt_putchar(',');
      print_int(orientation.roll >> 3);
      alt_putchar('\n');
      break;
    case COMMAND_ACCEL_X:
      get_accel(&accel_data, &orientation);
      print_int(accel_data.x);
      alt_putchar('\n');
      break;
    case COMMAND_ACCEL_Y:
      get_accel(&accel_data, &orientation);
      print_int(accel_data.y);
      alt_putchar('\n');
      break;
    case COMMAND_ACCEL_Z:
      get_accel(&accel_data, &orientation);
      print_int(accel_data.z);
      alt_putchar('\n');
      break;
    case COMMAND_DEPTH:
      i = get_depth();
      print_int(i);
      alt_putchar('\n');
      break;
    case COMMAND_GYRO:
      get_gyro(&x, &y, &z);
      print_int(x);
      alt_putchar(',');
      print_int(y);
      alt_putchar(',');
      print_int(z);
      alt_putchar('\n');
      break;
    case COMMAND_GYRO_X:
      get_gyro(&x, &y, &z);
      print_int(x);
      alt_putchar('\n');
      break;
    case COMMAND_GYRO_Y:
      get_gyro(&x, &y, &z);
      print_int(y);
      alt_putchar('\n');
      break;
    case COMMAND_GYRO_Z:
      get_gyro(&x, &y, &z);
      print_int(z);
      alt_putchar('\n');
      break;
    case COMMAND_MOTORS:
      print_int(get_pwm_freq());
      alt_putchar('\n');
      alt_putchar(get_motor_dir(0));
      for (i = 1; i < NUM_MOTORS; i++) {
        alt_putchar(',');
        alt_putchar(get_motor_dir(i));
      }
      alt_putchar('\n');
      print_int(get_motor_duty_cycle(0));
      for (i = 1; i < NUM_MOTORS; i++) {
        alt_putchar(',');
        print_int(get_motor_duty_cycle(i));
      }
      alt_putchar('\n');
      break;
    case COMMAND_MOTORS_MIXED:
      sscanf(st,"%d %d %d %d %d",&inputs[0],&inputs[1],&inputs[2],&inputs[3],&inputs[4]);
      controller_output(inputs[0],inputs[1],inputs[2],inputs[3],inputs[4]);
      break;  
    case COMMAND_INTERRUPT_DEBUG:
      for ( i = 0; i < 10; i++){
        printf("Interrupt debug %d: %d\n", i, DEBUG_interrupt[i]); 
      }
      break;
    case COMMAND_SPEED:
      i = read_hex(st);
      if (i < -157) { target_orientation.speed = -157; }
      else if (i > 157) { target_orientation.speed = 157; }
      else { target_orientation.speed = i; }
      break;
    case COMMAND_HEADING_CHANGE:
      i = read_hex(st);
      if (i < -157) { target_orientation.heading = -157; }
      else if (i > 157) { target_orientation.heading = 157; }
      else { target_orientation.heading = i; }
      break;
    case COMMAND_VERTICAL_OFFSET:
      i = read_hex(st);
      if (i < -157) { target_orientation.depth_offset = -157; }
      else if (i > 157) { target_orientation.depth_offset = 157; }
      else { target_orientation.depth_offset = i; }
      break;
  }
}


// Controller Interrupt Routine, should be 100Hz.
static void timer_interrupts(void* context, alt_u32 id)
{
   // Power stuff
   int i;
   for (i = 0; i < 7; i++) {
      // Reset to 0 if unchanged
      if (power_failures[i] == old_power_failures[i]) {
         power_failures[i] = 0;
      }
      old_power_failures[i] = power_failures[i];
   }

   // Get current orientation data
   struct t_accel_data accel_data;   
   int pitch_setting = 200;
   int roll_setting = 200;
   int depth_setting = 200;
   int heading = 200;
   int velocity = 200;
   
   // Calculate orientation data
   previous_orientation = current_orientation;
   get_accel(&accel_data, &current_orientation);
	
   // Calculate PD error values	
   // I assume that I only want stabilization (target_orientation is set from main notebook master)
   PD_controller_error_values.pitch_D = current_orientation.pitch - target_orientation.pitch - PD_controller_error_values.pitch_P;
   PD_controller_error_values.roll_D = current_orientation.roll - target_orientation.roll - PD_controller_error_values.roll_P;
   PD_controller_error_values.depth_D = current_orientation.depth - target_orientation.depth - PD_controller_error_values.depth_P;
   PD_controller_error_values.heading_D = current_orientation.heading - target_orientation.heading - PD_controller_error_values.heading_P;
   PD_controller_error_values.pitch_P = current_orientation.pitch - target_orientation.pitch;
   PD_controller_error_values.roll_P = current_orientation.roll - target_orientation.roll;
   PD_controller_error_values.depth_P = current_orientation.depth - target_orientation.depth;
   PD_controller_error_values.heading_P = current_orientation.heading - target_orientation.heading;
   
   // PD values yet to be determined
   pitch_setting = pitch_setting + (PD_controller_error_values.pitch_D>>4)*4 + (PD_controller_error_values.pitch_P>>4)*4;
   roll_setting = roll_setting + (PD_controller_error_values.roll_D>>4)*4 + (PD_controller_error_values.roll_P>>4)*4;
   if (target_orientation.depth_offset == 0){
   // The mission computer has set depth change to zero, therefore maintain the target_orientation
     depth_setting = depth_setting + (current_orientation.depth_offset>>4)*4;
   }
   else {
   // The mission computer has set the depth change value, ignore pd input
     depth_setting = depth_setting + target_orientation.depth_offset;
   }
   
   DEBUG_interrupt[5] = pitch_setting;
   DEBUG_interrupt[6] = roll_setting;
   DEBUG_interrupt[7] = depth_setting;
   DEBUG_interrupt[8] = heading;
   DEBUG_interrupt[9] = velocity;
   
   // Set motor outputs
   controller_output(pitch_setting, roll_setting, depth_setting, target_orientation.heading, target_orientation.speed);
   
   // Restart Interrupt for timer_0
   IOWR_ALTERA_AVALON_TIMER_SNAPL(CONTROLLER_INTERRUPT_COUNTER_BASE, 1);
   IOWR_ALTERA_AVALON_TIMER_CONTROL(CONTROLLER_INTERRUPT_COUNTER_BASE,0); // Clear interrupt (ITO)
   IOWR_ALTERA_AVALON_TIMER_STATUS(CONTROLLER_INTERRUPT_COUNTER_BASE, 0); // Clear TO
   IOWR_ALTERA_AVALON_TIMER_CONTROL(CONTROLLER_INTERRUPT_COUNTER_BASE,7); // Enable IRQ and Start timer
}

// Power management Interrupt routine
//
// If this triggers, something went wrong with one of the voltage lines
// Turn off power to all the voltage rails and print out what caused
// the problem
static void pm_interrupt(void *context, alt_u32 id)
{
   // Get failing voltage line
   int which_failed = IORD(POWER_MANAGEMENT_SLAVE_0_BASE, 0);
   power_failures[which_failed]++;
   // Hack to avoid 5V under-volt failures
   if (which_failed == 4) return;

   static const int failure_threshold = 100;
   if (power_failures[which_failed] < failure_threshold) {
      return;
   }

   // Turn off power
   IOWR(POWER_MANAGEMENT_SLAVE_0_BASE, 0, 0);

   // Might be in the middle of printing, send a newline
   alt_putchar('\n');
   int i;
   for (i = 0; i < 7; i++) {
      if (power_failures[i] == 0) {
         continue;
      }
      switch(i) {
         case 0:
            alt_putstr("24 V under-voltage failed");
            break;
         case 1:
            alt_putstr("12 V over-voltage failed");
            break;
         case 2:
            alt_putstr("12 V under-voltage failed");
            break;
         case 3:
            alt_putstr("5 over-voltage failed");
            break;
         case 4:
            alt_putstr("5 V under-voltage failed");
            break;
         case 5:
            alt_putstr("3.3 V over-voltage failed");
            break;
         case 6:
            alt_putstr("3.3 V under-voltage failed");
            break;
       }
       printf(" %d times\n", power_failures[i]);
   }
}

// the main function
int main()
{
  char buffer_str[STR_LEN+1];

  // Set inital target orientation
  target_orientation.pitch = 0;
  target_orientation.roll = 0;
  target_orientation.speed = 0;
  target_orientation.depth_offset = 0;
  target_orientation.depth = 0;
  target_orientation.heading = 0;
  // Set PD values
  PD_controller_inputs.pitch_D = 0;
  PD_controller_inputs.roll_D = 0;
  PD_controller_inputs.depth_D = 0;
  PD_controller_inputs.heading_D = 0;
  PD_controller_inputs.pitch_P = 0;
  PD_controller_inputs.roll_P = 0;
  PD_controller_inputs.depth_P = 0;
  PD_controller_inputs.heading_P = 0;

  init();

  // Setup controller interrupt
  alt_ic_isr_register(CONTROLLER_INTERRUPT_COUNTER_IRQ_INTERRUPT_CONTROLLER_ID,CONTROLLER_INTERRUPT_COUNTER_IRQ,timer_interrupts,0,0);	// Register Interrupt (check system.h for defs)
  alt_ic_isr_register(POWER_MANAGEMENT_SLAVE_0_IRQ_INTERRUPT_CONTROLLER_ID, POWER_MANAGEMENT_SLAVE_0_IRQ,pm_interrupt,0,0);	// Register Interrupt (check system.h for defs)
  IOWR_ALTERA_AVALON_TIMER_CONTROL(CONTROLLER_INTERRUPT_COUNTER_BASE, 0);		// Clear control register
  IOWR_ALTERA_AVALON_TIMER_CONTROL(CONTROLLER_INTERRUPT_COUNTER_BASE, 2);		// Continuous Mode ON
  IOWR_ALTERA_AVALON_TIMER_PERIODL(CONTROLLER_INTERRUPT_COUNTER_BASE, 0x4B40);   // Timer interrupt period is 100ms, 10 Hz refresh rate
  IOWR_ALTERA_AVALON_TIMER_PERIODH(CONTROLLER_INTERRUPT_COUNTER_BASE, 0x004C);
  IOWR_ALTERA_AVALON_TIMER_CONTROL(CONTROLLER_INTERRUPT_COUNTER_BASE, 3);	    // Enable timer_0 interrupt
  IOWR_ALTERA_AVALON_TIMER_CONTROL(CONTROLLER_INTERRUPT_COUNTER_BASE, 7);       // Start timer interrupt

  // read and process commands continuously
  while(1) {
    alt_getline(buffer_str, STR_LEN);
    process_command(buffer_str);
  //process_command("di\n");
  //printf("\n");
  }

  return 0;
}
