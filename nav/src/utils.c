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

// remember what mode each motor is in ('s' for stopped, 'f' for forward, 'r' for reverse)
char motor_modes[NUM_MOTORS];

// remember the duty cycle value for each motor
int motor_duty_cycles[NUM_MOTORS];

// initialize array values
void init()
{
  // instantiate motor state as stopped and 0 duty cycle
  memset(motor_modes, (int)'s', NUM_MOTORS);
  memset(motor_duty_cycles, 0, NUM_MOTORS * sizeof(int));
}

// returns a string until the maximum length (int len) or a newline character is reached from stdin
void alt_getline(char *st, int len)
{
  while (len--) {
    char c = (char)alt_getchar();
    putchar(c);
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
  while (*st && *st != '\n' && *st != ' ') {
    i <<= 4;
    if (*st >= '0' && *st <= '9')
      i |= (int)(*st - '0');
    else if (*st >= 'a' && *st <= 'f')
      i |= (int)(*st - 'a' + 10);
    st++;
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

// set motor direction
void set_motor_dir(int motor_num, enum MOTOR_DIR dir)
{
  switch(dir) {
    case MOTOR_DIR_STOPPED:
      IOWR(MOTOR_CONTROLLER_0_BASE, motor_num, 0x0);
      motor_modes[motor_num] = 's';
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
  IOWR(MOTOR_CONTROLLER_0_DUTY_CYCLE, motor_num, duty_cycle);
  motor_duty_cycles[motor_num] = duty_cycle;
}

// get motor duty cycle
int get_motor_duty_cycle(int motor_num)
{
  if (motor_num < 0 || motor_num >= NUM_MOTORS)
    return 0;
  return motor_duty_cycles[motor_num];
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