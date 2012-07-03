// Function prototypes implemented in utils.c

#ifndef _MDA_UTILS_H
#define _MDA_UTILS_H

#include "accelerometer_adxl345_spi.h"

enum MOTOR_DIR {
  MOTOR_DIR_STOPPED,
  MOTOR_DIR_BRAKE,
  MOTOR_DIR_FORWARD,
  MOTOR_DIR_REVERSE
};

void init();
void alt_getline(char *, int);
int read_hex(char *);
void print_int(int);
void set_motor_dir(int, enum MOTOR_DIR);
char get_motor_dir(int);
void set_motor_duty_cycle(int, int);
void set_pwm_freq(int);
void set_pow(int);
int get_motor_duty_cycle(int);
int get_pwm_freq();
void get_accel(struct t_accel_data *, struct orientation *);
int get_depth();

// API to give commands to NIOS

enum COMMAND_ID {
  COMMAND_INVALID,
  COMMAND_FORWARD,
  COMMAND_REVERSE,
  COMMAND_STOP_ALL,
  COMMAND_STOP,
  COMMAND_BRAKE,
  COMMAND_DUTY_CYCLE,
  COMMAND_FREQ,
  COMMAND_POW,
  COMMAND_ACCEL,
  COMMAND_ACCEL_X,
  COMMAND_ACCEL_Y,
  COMMAND_ACCEL_Z,
  COMMAND_DEPTH,
  COMMAND_MOTORS,
  COMMAND_HELP,
  COMMAND_SPEED,
  COMMAND_HEADING_CHANGE,
  COMMAND_VERTICAL_OFFSET
};

struct command_struct {
  char *name;
  enum COMMAND_ID id;
  char *help_string;
};

struct PD_controller_error_values{
    int pitch_P;
	int roll_P;
	int depth_P;
	int heading_P;
	
	int pitch_D;
	int roll_D;
	int depth_D;
	int heading_D;
};

struct PD_controller_inputs{
    int pitch_P;
	int roll_P;
	int depth_P;
	int heading_P;
	
	int pitch_D;
	int roll_D;
	int depth_D;
	int heading_D;
};

#endif
