#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#define ZERO_DC 512
#define PWM_FREQ 20

unsigned cmd_ok = 0;
unsigned power = 0;

char *proc_str = NULL;
FILE *fterm = NULL;

int atoi_safe (char *str) {
    if (str == NULL) {
        printf ("Invalid input to atoi.\n");
        return 0;
    }
    return atoi (str);
}

void cmd_error () { printf ("**** Invalid command. Type \"help\" for command list\n"); }
void exit_safe () { power_off(); exit(0); pclose(fterm); }

void spawn_term (char *proc)
{
    if (proc) {
        fterm = (FILE *)popen(proc, "w");
    }
    if (!fterm) {
        fterm = stderr;
    }
    proc_str = proc;
}

void read_from_term (char *cmd)
{
    pclose(fterm);
    char full_cmd[100], out_str[100];

    sprintf(full_cmd, "echo \"%s\" | %s", cmd, proc_str);
    fterm = (FILE *)popen(full_cmd, "r");

    while (fgets(out_str, 100, fterm)) {
        printf("%s", out_str);
    }

    pclose(fterm);

    spawn_term(proc_str);
}

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
    read_from_term ("gm");
}

void motor_set (int pwm, char motor_flags) {
    cmd_ok = 1;
    if (pwm < -PWM_MAX || pwm > PWM_MAX) {
        printf ("**** Invalid pwm. motor pwm unchanged.\n");
        return;
    }
    
    if (motor_flags & H_FRONT_LEFT) {
        printf ("set left front vertical motor to %d\n", pwm);
        fprintf (fterm, "smd %d %x\n", M_FRONT_LEFT, pwm);
    }
    if (motor_flags & H_FRONT_RIGHT) {
        printf ("set right front vertical motor to %d\n", pwm);
        fprintf (fterm, "smd %d %x\n", M_FRONT_RIGHT, pwm);
    }
    if (motor_flags & H_FWD_LEFT) {
        printf ("set left forward motor to %d\n", pwm);
        fprintf (fterm, "smd %d %x\n", M_FWD_LEFT, pwm);
    }
    if (motor_flags & H_FWD_RIGHT) {
        printf ("set right forward motor to %d\n", pwm);
        fprintf (fterm, "smd %d %x\n", M_FWD_RIGHT, pwm);
    }
    if (motor_flags & H_REAR) {
        printf ("set rear vertical motor to %d\n", pwm);
        fprintf (fterm, "smd %d %x\n", M_REAR, pwm);
    }
}

void power_status () {
    cmd_ok = 1;
    printf("power is turned %s\n", (power) ? "on" : "off");
}
void power_on () {
    cmd_ok = 1;
    printf ("turned power on.\n");
    fprintf (fterm, "p 1\n");
    fprintf (fterm, "spf %x\n", PWM_FREQ);
    fprintf (fterm, "smd a %x\n", ZERO_DC);
    fprintf (fterm, "smf a\n");
    power = 1;
}
void power_off () {
    cmd_ok = 1;
    printf ("turned power off.\n");
    fprintf (fterm, "p 0\n");
    fprintf (fterm, "sms a\n");
    fprintf (fterm, "smd a %x\n", ZERO_DC);
    power = 0;
}

void dyn_status () {
    cmd_ok = 1;

    // Acceleration
    read_from_term("ga");
    // Depth
    read_from_term("gd");
}
