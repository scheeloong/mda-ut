/*
 * main.c
 *
 * The guts of the FPGA processor
 *
 * Author: victor
 */

#include <string.h>
#include "system.h"
#include "sys/alt_stdio.h"

#include "utils.h"

volatile int *motor_addr = (int *)MOTOR_CONTROLLER_0_BASE;
int motor_word = 0;

void do_command(char *st) {
  if (strncmp(st, "f", 1) == 0) {
    alt_putstr("going forward\n");
    motor_word |= 0xfff;
  } else if (strncmp(st, "r", 1) == 0) {
    alt_putstr("going in reverse\n");
    motor_word &= 0xff0000;
    motor_word |= 0x555;
  } else if (strncmp(st, "stop", 4) == 0) {
    alt_putstr("stopping\n");
    motor_word &= 0xff0000;
  } else if (strncmp(st, "sd ", 3) == 0) {
    alt_putstr("setting duty cycle\n");
    motor_word &= 0xffff;
	motor_word |= (read_hex(&st[3]) << 16);
  } else {
    alt_putstr("command not recognized\n");
  }
  *(motor_addr+1) = motor_word;
}

int main() {
  char buffer_str[STR_LEN+1];

  while(1) {
    alt_getline(buffer_str, STR_LEN);
    do_command(buffer_str);
  }

  return 0;
}
