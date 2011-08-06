// Function prototypes implemented in utils.c

#ifndef _MDA_UTILS_H
#define _MDA_UTILS_H

#define STR_LEN 30
#include "accelerometer_adxl345_spi.h"

void alt_getline(char *, int);
int read_hex(char *);
void get_accel(struct t_accel_data *);

// API to give commands to NIOS

enum COMMAND_ID {
  COMMAND_INVALID,
  COMMAND_FORWARD,
  COMMAND_REVERSE,
  COMMAND_STOP_ALL,
  COMMAND_STOP,
  COMMAND_DUTY_CYCLE,
  COMMAND_ACCEL,
  COMMAND_ACCEL_X,
  COMMAND_ACCEL_Y,
  COMMAND_ACCEL_Z,
  COMMAND_HELP
};

struct command_struct {
  char *name;
  enum COMMAND_ID id;
  char *help_string;
};

#endif
