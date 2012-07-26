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

#include "interrupts.h"
#include "settings.h"
#include "pwm_force.h"
#include "utils.h"

// remember what mode each motor is in ('s' for stopped, 'f' for forward, 'r' for reverse)
char motor_modes[NUM_MOTORS];

// remember the duty cycle value for each motor
int motor_duty_cycles[NUM_MOTORS];

// remember the PWM period (calculated from inputted frequency)
int pwm_period = -1;

// initialize array values
void init()
{
  int i;

  // Initialize to power off
  set_pow(0);

  // instantiate motor state as stopped and 0 duty cycle
  memset(motor_modes, (int)'s', NUM_MOTORS);
  for (i = 0; i < NUM_MOTORS; i++) {
    set_motor_duty_cycle(i, HALF_PWM);
  }

  // Initialize interrupts
  init_interrupts();
  
  // Ritchie - Initialize PID controllers
  pid_init();

  // Initialize motor linearization lookup table
  init_lookup();
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
  return IORD(IMU_CONTROLLER_0_BASE, 5);
}

// returns a struct of x,y,z acceleration values
void get_accel(struct t_accel_data *accel_data)
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

  return;
}
