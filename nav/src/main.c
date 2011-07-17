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

#include "utils.h"

#define NUM_MOTORS 6

volatile int *motor_addr = (int *)MOTOR_CONTROLLER_0_BASE;
int motor_word = 0;

void do_command(char *st)
{
  int i;
  if (strncmp(st, "f", 1) == 0) {
    for (i = 0; i < NUM_MOTORS; i++) {
      IOWR(MOTOR_CONTROLLER_0_BASE, i*4, 0x3);
    }
    alt_putstr("going forward\n");
  } else if (strncmp(st, "r", 1) == 0) {
    for (i = 0; i < NUM_MOTORS; i++) {
      IOWR(MOTOR_CONTROLLER_0_BASE, i*4, 0x1);
    }
    alt_putstr("going in reverse\n");
  } else if (strncmp(st, "stop", 4) == 0) {
    for (i = 0; i < NUM_MOTORS; i++) {
      IOWR(MOTOR_CONTROLLER_0_BASE, i*4, 0x0);
    }
    alt_putstr("stopping\n");
  } else if (strncmp(st, "sd ", 3) == 0) {
    int dc = read_hex(&st[3]);
    for (i = 0; i < NUM_MOTORS; i++) {
      IOWR(MOTOR_CONTROLLER_0_BASE, (8+i)*4, dc);
    }
    alt_putstr("setting duty cycle\n");
  } else {
    alt_putstr("command not recognized\n");
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
