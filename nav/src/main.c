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

// This is the list of all the commands
// PLEASE KEEP THIS IN ALPHABETICAL ORDER
struct command_struct my_cmds[] = {
  {"ga", COMMAND_ACCEL, "get acceleration\n  Usage: ga\n\n  Print (x,y,z) acceleration\n\n"},
  {"gax", COMMAND_ACCEL_X, "get x-acceleration\n  Usage: gax\n\n  Print x-acceleration\n\n"},
  {"gay", COMMAND_ACCEL_Y, "get y-acceleration\n  Usage: gay\n\n  Print y-acceleration\n\n"},
  {"gaz", COMMAND_ACCEL_Z, "get z-acceleration\n  Usage: gaz\n\n  Print z-acceleration\n\n"},
  {"h", COMMAND_HELP, "help\n  Usage: h <cmd>\n\n  Print the help message for all commands that start with cmd, leave empty to print all help messages\n\n"},
  {"sdc", COMMAND_DUTY_CYCLE, "set duty cycle\n  Usage: sf <n> <dc>\n\n  Set the duty cycle of the nth motor to dc\n\n"},
  {"sf", COMMAND_FORWARD, "set forward\n  Usage: sf <n>\n\n  Turn on the nth motor in the forward direction\n\n"},
  {"sr", COMMAND_REVERSE, "set reverse\n  Usage: sr <n>\n\n  Turn on the nth motor in the reverse direction\n\n"},
  {"ss", COMMAND_STOP, "set stop\n  Usage: ss <n>\n\n  Turn the mth motor off\n\n"},
  {"stop", COMMAND_STOP_ALL, "stop\n  Usage: stop\n\n  Stop all motors\n\n"}
};

int cmd_len = sizeof(my_cmds) / sizeof(struct command_struct);

void print_help(char *st)
{
  // ignore leading spaces
  while (*st == ' ') st++;

  printf("Help messages\n\n");

  int i, len = strlen(st);
  for (i = 0; i < cmd_len; i++) {
    if (strncmp(st, my_cmds[i].name, len) == 0) {
      printf("%s - %s", my_cmds[i].name, my_cmds[i].help_string);
    }
  }
}

void process_command(char *st)
{
  enum COMMAND_ID cid = COMMAND_INVALID;
  int mid;
  for (mid = 0; mid < cmd_len; mid++) {
    if (strncmp(st, my_cmds[mid].name, strlen(my_cmds[mid].name)) == 0) {
      cid = my_cmds[mid].id;
      st += strlen(my_cmds[mid].name);
      break;
    }
  }

  // variables used in case statement
  struct t_accel_data accel_data;
  int i, dc;
  switch (cid) {
    case COMMAND_INVALID:
      printf("Sorry, command %s is not recognized. For a list of valid commands, type 'h' for help\n", st);
      break;
    case COMMAND_HELP:
      print_help(st+1);
      break;
    case COMMAND_STOP_ALL:
      for (i = 0; i < NUM_MOTORS; i++) {
        IOWR(MOTOR_CONTROLLER_0_BASE, i, 0x0);
      }
      printf("stopping\n");
      break;
    case COMMAND_STOP:
      i = read_hex(st);
      if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        IOWR(MOTOR_CONTROLLER_0_BASE, i, 0x0);
        printf("stopping motor %d\n", i);
      }
      break;
    case COMMAND_FORWARD:
      i = read_hex(st);
      if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        IOWR(MOTOR_CONTROLLER_0_BASE, i, 0x1 | 0x2);
        printf("setting motor %d to forward\n", i);
      }
      break;
    case COMMAND_REVERSE:
      i = read_hex(st);
      if (i < 0 || i >= NUM_MOTORS) {
        printf("motor # invalid\n");
      } else {
        IOWR(MOTOR_CONTROLLER_0_BASE, i, 0x1);
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
          IOWR(MOTOR_CONTROLLER_0_DUTY_CYCLE, i, dc);
          printf("setting motor %d to duty cycle %d\n", i, dc);
        }
      }
      break;
    case COMMAND_ACCEL:
      get_accel(&accel_data);
#ifdef INTERACTIVE
      printf("%d,%d,%d\n", accel_data.x, accel_data.y, accel_data.z);
#else
      alt_printf("%x,%x,%x\n", accel_data.x, accel_data.y, accel_data.z);
#endif
      break;
    case COMMAND_ACCEL_X:
      get_accel(&accel_data);
#ifdef INTERACTIVE
      printf("%d\n", accel_data.x);
#else
      alt_printf("%x\n", accel_data.x);
#endif
      break;
    case COMMAND_ACCEL_Y:
      get_accel(&accel_data);
#ifdef INTERACTIVE
      printf("%d\n", accel_data.y);
#else
      alt_printf("%x\n", accel_data.y);
#endif
      break;
    case COMMAND_ACCEL_Z:
      get_accel(&accel_data);
#ifdef INTERACTIVE
      printf("%d\n", accel_data.z);
#else
      alt_printf("%x\n", accel_data.z);
#endif
      break;
  }
}

int main()
{
  char buffer_str[STR_LEN+1];

  while(1) {
    alt_getline(buffer_str, STR_LEN);
    process_command(buffer_str);
  }

  return 0;
}
