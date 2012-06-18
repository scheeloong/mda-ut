/*
 * utils.c
 *
 * Useful utility functions
 *
 * Author: victor
 */

#include <stdlib.h>
#include <string.h>

#include "alt_types.h"
#include "io.h"
#include "system.h"
#include "sys/alt_stdio.h"

#include "settings.h"
#include "utils.h"

// Structure for debugging interrupt data, printf's taking too long
int DEBUG_interrupt[10];

// remember what mode each motor is in ('s' for stopped, 'f' for forward, 'r' for reverse)
char motor_modes[NUM_MOTORS];

// remember the duty cycle value for each motor
int motor_duty_cycles[NUM_MOTORS];

// remember the PWM period (calculated from inputted frequency)
int pwm_period = -1;

// initialize array values
void init()
{
  // Initialize to power off
  set_pow(0);

  // instantiate motor state as stopped and 0 duty cycle
  memset(motor_modes, (int)'s', NUM_MOTORS);
  memset(motor_duty_cycles, 0, NUM_MOTORS * sizeof(int));
}

// returns a string until the maximum length (int len) or a newline character is reached from stdin
void alt_getline(char *st, int len)
{
  while (len--) {
    char c = (char)alt_getchar();
    alt_putchar(c);
    *st++ = c;
    if (c == '\n')
      break;
  }
  *st = '\0';
}

// returns an int from reading a hex string
int read_hex(char *st)
{
  // ignore leading spaces
  while (*st == ' ') st++;

  if (*st == '\0' || *st == '\n') {
    return -1;
  }

  int i = 0;
  bool is_negative = (*st == '-');
  while (*st && *st != '\n' && *st != ' ') {
    i <<= 4;
    if (*st >= '0' && *st <= '9')
      i |= (int)(*st - '0');
    else if (*st >= 'a' && *st <= 'f')
      i |= (int)(*st - 'a' + 10);
    st++;
  }
  if (is_negative) {
    return -i;
  }
  return i;
}

// print an integer
void print_int(int i)
{
#ifdef INTERACTIVE
  printf("%d", i);
#else
  alt_print("%x", i);
#endif
}

// set power
void set_pow(int on_off)
{
  IOWR(POWER_MANAGEMENT_SLAVE_0_BASE, 0, on_off);
}

// set motor direction
void set_motor_dir(int motor_num, enum MOTOR_DIR dir)
{
  switch(dir) {
    case MOTOR_DIR_STOPPED:
      IOWR(MOTOR_CONTROLLER_0_BASE, motor_num, 0x0);
      motor_modes[motor_num] = 's';
      break;
    case MOTOR_DIR_BRAKE:
      IOWR(MOTOR_CONTROLLER_0_BASE, motor_num, 0x2);
      motor_modes[motor_num] = 'b';
      break;
    case MOTOR_DIR_REVERSE:
      IOWR(MOTOR_CONTROLLER_0_BASE, motor_num, 0x1);
      motor_modes[motor_num] = 'r';
      break;
    case MOTOR_DIR_FORWARD:
      IOWR(MOTOR_CONTROLLER_0_BASE, motor_num, 0x3);
      motor_modes[motor_num] = 'f';
      break;
  }
}

// get motor direction
char get_motor_dir(int motor_num)
{
  if (motor_num < 0 || motor_num >= NUM_MOTORS)
    return '?';
  return motor_modes[motor_num];
}

// set motor duty cycle
void set_motor_duty_cycle(int motor_num, int duty_cycle)
{
  // set duty period using duty_cycle as a fraction over 1024
  IOWR(MOTOR_CONTROLLER_0_DUTY_CYCLE, motor_num, duty_cycle * pwm_period / 1024);
  motor_duty_cycles[motor_num] = duty_cycle;
}

// set PWM frequency for motor
void set_pwm_freq(int freq)
{
  // 50000 is 50MHz in KHz
  pwm_period = 50000 / freq;
  IOWR(MOTOR_CONTROLLER_0_DUTY_CYCLE, NUM_MOTORS, pwm_period);
}

// get motor duty cycle
int get_motor_duty_cycle(int motor_num)
{
  if (motor_num < 0 || motor_num >= NUM_MOTORS)
    return 0;
  return motor_duty_cycles[motor_num];
}

// get PWM frequency
int get_pwm_freq()
{
  return  50000 / pwm_period;
}

// returns depth
int get_depth()
{
  return IORD(IMU_CONTROLLER_0_BASE, 3);
}

// returns a x,y,z gyroscope values
void get_gyro(int *x, int *y, int *z)
{
  *x = IORD(IMU_CONTROLLER_0_BASE, 0);
  *y = IORD(IMU_CONTROLLER_0_BASE, 1);
  *z = IORD(IMU_CONTROLLER_0_BASE, 2);
}

// This function is responsible for calling the motor setting signals.
// An input of 200 is the neutral position for the specific axis input.
// An input of (200,200,200,200,200) would mean for the object to remain stationary. 
// Range of input = (0,400)
// Motor 1 = front, vertical, left; 
// Motor 2 = front, vertical, right;
// Motor 3 = center, horizontal, right;
// Motor 4 = center, horizontal, left;
// Motor 5 = rear, vertical;
// duty cycle range = (200,824), centered at 512, dont go full range
void controller_output(int pitch_setting, int roll_setting, int depth_setting,int heading, int velocity)
{
   int motor_duty_cycle[5];
   int i;
   
   int vertical_thrust_steady_state = 0; //Tune for some number better for better depth controller
        
   motor_duty_cycle[0] = 512 + vertical_thrust_steady_state + (depth_setting-200) + (pitch_setting-200) + (roll_setting-200); 
   motor_duty_cycle[1] = 512 + vertical_thrust_steady_state + (depth_setting-200) + (pitch_setting-200) - (roll_setting-200);
   motor_duty_cycle[2] = 512 + (velocity) + (heading);
   motor_duty_cycle[3] = 512 + (velocity) - (heading);
   motor_duty_cycle[4] = 512 + vertical_thrust_steady_state + (depth_setting-200) - (pitch_setting-200);

   for ( i = 0; i < 5; i++ )
   {
      if ( motor_duty_cycle[i] < 200 )
         motor_duty_cycle[i] = 200;
      if ( motor_duty_cycle[i] > 824 )
         motor_duty_cycle[i] = 824;
      set_motor_duty_cycle(i, motor_duty_cycle[i]);  
      DEBUG_interrupt[i] = motor_duty_cycle[i];
   }  
}

// returns a struct of x,y,z acceleration values
void get_accel(struct t_accel_data *accel_data, struct orientation *orientation)
{
  IOWR(SELECT_I2C_CLK_BASE, 0, 0x00);

  // configure accelerometer as +-2g and start measure
  if (!ADXL345_SPI_Init(GSENSOR_SPI_BASE)) {
    // could not configure accelerometer
    return;
  }

  while (1) {
    if (ADXL345_SPI_IsDataReady(GSENSOR_SPI_BASE) && ADXL345_SPI_XYZ_Read(GSENSOR_SPI_BASE, accel_data)) {
      break;
    }
  }

  // With the accleration data, calculate the orientation as well.
  IOWR(IMU_CONTROLLER_0_BASE,0,accel_data->x);
  IOWR(IMU_CONTROLLER_0_BASE,1,accel_data->y);
  IOWR(IMU_CONTROLLER_0_BASE,2,(-1)*accel_data->z);
  // Delay 10 cycles for hardware processing
  int z;
  for ( z = 0; z < 10; z++ ){}
  // Write to output orientation structure
  orientation->pitch = -1 * IORD(IMU_CONTROLLER_0_BASE,8);
  orientation->roll = -1 * IORD(IMU_CONTROLLER_0_BASE,7);

  return;
}
