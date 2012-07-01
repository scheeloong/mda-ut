#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

unsigned cmd_ok = 0;

int atoi_safe (char *str) {
    if (str == NULL) {
        printf ("Invalid input to atoi.\n");
        return 0;
    }
    return atoi (str);
}

void cmd_error () { printf ("**** Invalid command. Type \"help\" for command list\n"); }
void exit_safe () { power_off(); exit(0); }

void help () {
    cmd_ok = 1;
    printf ("\n\tCommands go by the format <system> <options>, like \"power on\"\n");
    printf ("\n\tDo \"help <system>\" to print the options of that system.\n");
    printf ("\t\"q\" to quit.\n");
    printf ("\n\tList of commands\n\t\tmotor / m\n\t\tdyn / d\n\t\tpower / p\n\n");
}

void help_motor () {
    cmd_ok = 1;
    printf ("\n\tmotor / m\n");
    printf ("\t\t%-10s - %s","status","print each motor's PWM value. PWM values range from -100 to 100\n");
    printf ("\t\t%-10s - %s","<N> <pwm>","set motor N to PWM value pwm. For example \"motor 3 -55\".\n");
    printf ("\t\t%-10s - %s","all stop","stop all motors.\n");
    printf ("\t\t%-10s - %s","fwd <pwm>","set the 2 horizontal motors to pwm.\n");
    printf ("\t\t%-10s - %s","rise <pwm>","set the 3 veritcal motors to pwm.\n\n");
}

void help_dyn () {
    cmd_ok = 1;
    printf ("\n\tdyn / d\n");
    printf ("\t\t%-10s - %s","status","prints accelerometer and depth readings.\n\n");
}

void help_power () {
    cmd_ok = 1;
    printf ("\n\tpower / p\n");
    printf ("\t\t%-10s - %s","status","prints power status.\n");
    printf ("\t\t%-10s - %s","on / 1","turns power on.\n");
    printf ("\t\t%-10s - %s","off / 0","turns power off.\n\n");
}

void motor_status() {
    cmd_ok = 1;
    printf ("motor_status not implemented.\n");
}

void motor_set (int pwm, char motor_flags) {
    cmd_ok = 1;
    if (pwm < -PWM_MAX || pwm > PWM_MAX) {
        printf ("**** Invalid pwm. motor pwm unchanged.\n");
        return;
    }
    
    if (motor_flags & H_FRONT_LEFT) printf ("set M_FRONT_LEFT to %d\n", pwm);
    if (motor_flags & H_FRONT_RIGHT) printf ("set M_FRONT_RIGHT to %d\n", pwm);
    if (motor_flags & H_FWD_LEFT) printf ("set M_FWD_LEFT to %d\n", pwm);
    if (motor_flags & H_FWD_RIGHT) printf ("set M_FWD_RIGHT to %d\n", pwm);
    if (motor_flags & H_REAR) printf ("set M_REAR to %d\n", pwm);
}

void power_status () {
    cmd_ok = 1;
    printf ("power_status not implemented.\n");
}
void power_on () {
    cmd_ok = 1;
    printf ("power_on not implemented.\n");
}
void power_off () {
    cmd_ok = 1;
    printf ("power_off not implemented.\n");
}

void dyn_status () {
    cmd_ok = 1;
    printf ("dyn_status not implemented.\n");
}
