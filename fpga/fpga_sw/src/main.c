/*
 * main.c
 *
 * The guts of the FPGA processor
 *
 * Author: victor
 */

#include <string.h>
#include "io.h"
#include "system.h"
#include "sys/alt_stdio.h"

#include "settings.h"
#include "utils.h"

// This is the list of all the commands
// PLEASE KEEP THIS IN ALPHABETICAL ORDER
// Note: end the first string with a \n to ensure an exact match if no arguments are used
struct command_struct my_cmds[] = {
  {"ga\n", COMMAND_ACCEL, "ga - get acceleration\n  Usage: ga\n\n  Print (x,y,z) acceleration\n"},
  {"gax\n", COMMAND_ACCEL_X, "gax - get x-acceleration\n  Usage: gax\n\n  Print x-acceleration\n"},
  {"gay\n", COMMAND_ACCEL_Y, "gay - get y-acceleration\n  Usage: gay\n\n  Print y-acceleration\n"},
  {"gaz\n", COMMAND_ACCEL_Z, "gaz - get z-acceleration\n  Usage: gaz\n\n  Print z-acceleration\n"},
  {"gm\n", COMMAND_MOTORS, "gm - get motor data\n Usage: gm\n\n Print all motor settings (direction on one line and duty cycle on the next)\n"},
  {"h", COMMAND_HELP, "h - help\n  Usage: h <cmd>\n\n  Print the help message for all commands that start with cmd, leave empty to print all help messages\n"},
  {"smd", COMMAND_DUTY_CYCLE, "smd - set motor duty cycle\n  Usage: smd <n> <0xdc>\n\n  Set the duty cycle of the nth motor to dc\nNote: the duty cycle is inputted in hex out of 0x3ff (1024 in decimal)\n"},
  {"smf", COMMAND_FORWARD, "smf - set motor forward\n  Usage: smf <n>\n\n  Turn on the nth motor in the forward direction\n"},
  {"smr", COMMAND_REVERSE, "smr - set motor reverse\n  Usage: smr <n>\n\n  Turn on the nth motor in the reverse direction\n"},
  {"sms", COMMAND_STOP, "sms - set motor stop\n  Usage: sms <n>\n\n  Turn the nth motor off\n"},
  {"spf", COMMAND_FREQ, "spf - set PWM frequency\n  Usage: sms <0xn>\n\n  Set the PWM frequency to n in kilohertz\nNote: the frequency is inputted in hex\n"},
  {"stop\n", COMMAND_STOP_ALL, "stop\n  Usage: stop\n\n  Stop all motors\n"}
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
  int dc;

  switch (cid) {
    case COMMAND_INVALID:
      printf("Sorry, command %s is not recognized. For a list of valid commands, type 'h' for help\n", st);
      break;
    case COMMAND_HELP:
      print_help(st+1);
      break;
    case COMMAND_STOP_ALL:
      for (i = 0; i < NUM_MOTORS; i++) {
        set_motor_dir(i, MOTOR_DIR_STOPPED);
      }
      printf("stopping\n");
      break;
    case COMMAND_STOP:
      i = read_hex(st);
      if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        set_motor_dir(i, MOTOR_DIR_STOPPED);
        printf("stopping motor %d\n", i);
      }
      break;
    case COMMAND_FORWARD:
      i = read_hex(st);
      if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        set_motor_dir(i, MOTOR_DIR_FORWARD);
        printf("setting motor %d to forward\n", i);
      }
      break;
    case COMMAND_REVERSE:
      i = read_hex(st);
      if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        set_motor_dir(i, MOTOR_DIR_REVERSE);
        printf("setting motor %d to reverse\n", i);
      }
      break;
    case COMMAND_DUTY_CYCLE:
      i = read_hex(st);
      if (i < 0 || i >= NUM_MOTORS) {
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
      get_accel(&accel_data);
      print_int(accel_data.x);
      alt_putchar(',');
      print_int(accel_data.y);
      alt_putchar(',');
      print_int(accel_data.z);
      alt_putchar('\n');
      break;
    case COMMAND_ACCEL_X:
      get_accel(&accel_data);
      print_int(accel_data.x);
      alt_putchar('\n');
      break;
    case COMMAND_ACCEL_Y:
      get_accel(&accel_data);
      print_int(accel_data.y);
      alt_putchar('\n');
      break;
    case COMMAND_ACCEL_Z:
      get_accel(&accel_data);
      print_int(accel_data.z);
      alt_putchar('\n');
      break;
    case COMMAND_MOTORS:
      print_int(get_freq());
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
  }
}

// the main function
int main()
{
  char buffer_str[STR_LEN+1];

  init();

  // read and process commands continuously
  while(1) {
    alt_getline(buffer_str, STR_LEN);
    process_command(buffer_str);
  }

  return 0;
}
