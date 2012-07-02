#ifndef __MDA_UI_UTILS__
#define __MDA_UI_UTILS__

#define PWM_MAX 100

#define H_FRONT_LEFT 0x1 // in hex
#define H_FRONT_RIGHT 0x2
#define H_FWD_LEFT 0x4
#define H_FWD_RIGHT 0x8
#define H_REAR 0x10
#define M_FRONT_LEFT 0 // in decimal
#define M_FRONT_RIGHT 1
#define M_FWD_LEFT 2
#define M_FWD_RIGHT 3
#define M_REAR 4

#define H_FWD 0xc
#define H_RISE 0x13
#define H_ALL 0x1f

extern unsigned cmd_ok;

int atoi_safe (char *str);
void exit_safe ();
void cmd_error ();

void spawn_term (char *);

void help ();
void help_motor ();
void help_dyn ();
void help_power ();

void motor_status();
void motor_set (int pwm, char motor_flags);

void power_status();
void power_on();
void power_off();

void dyn_status();

#endif
