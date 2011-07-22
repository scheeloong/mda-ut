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

#define NUM_MOTORS 6
#define MOTOR_CONTROLLER_0_DUTY_CYCLE (MOTOR_CONTROLLER_0_BASE + 32)

void do_command(char *st)
{
  int i;
  if (strncmp(st, "f", 1) == 0) {
    for (i = 0; i < NUM_MOTORS; i++) {
      IOWR(MOTOR_CONTROLLER_0_BASE, i, 0x3);
    }
    if (INTERACTIVE) {
      alt_putstr("going forward\n");
    }
  } else if (strncmp(st, "r", 1) == 0) {
    for (i = 0; i < NUM_MOTORS; i++) {
      IOWR(MOTOR_CONTROLLER_0_BASE, i, 0x1);
    }
    if (INTERACTIVE) {
      alt_putstr("going in reverse\n");
    }
  } else if (strncmp(st, "stop", 4) == 0) {
    for (i = 0; i < NUM_MOTORS; i++) {
      IOWR(MOTOR_CONTROLLER_0_BASE, i, 0x0);
    }
    alt_putstr("stopping\n");
  } else if (strncmp(st, "sd ", 3) == 0) {
    int dc = read_hex(&st[3]);
    for (i = 0; i < NUM_MOTORS; i++) {
      IOWR(MOTOR_CONTROLLER_0_DUTY_CYCLE, i, dc);
    }
    if (INTERACTIVE) {
      alt_putstr("setting duty cycle\n");
    }
  } else if (strncmp(st, "ga", 2) == 0) {
    struct t_accel_data accel_data;
    get_accel(&accel_data);
    switch (st[2]) {
      case 'x':
        alt_printf("%x\n", accel_data.x);
        break;
      case 'y':
        alt_printf("%x\n", accel_data.y);
        break;
      case 'z':
        alt_printf("%x\n", accel_data.z);
        break;
      case '\n':
        alt_printf("%x,%x,%x\n", accel_data.x, accel_data.y, accel_data.z);
        break;
    }
  } else {
    if (INTERACTIVE) {
      alt_putstr("command not recognized\n");
    }
  }
}

int main()
{
  char buffer_str[STR_LEN+1];

  while(1) {
    alt_getline(buffer_str, STR_LEN);
    do_command(buffer_str);
  }

  return 0;
}
