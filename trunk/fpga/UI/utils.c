#include "utils.h"

#include <stdio.h>

void cmd_error () {
    printf ("Invalid command. Type \"help\" for command list\n");
}

void help () {
    cmd_ok = 1;
    printf ("\nType Commands into the UI.\nCommands go by the format <command> <options>.\n");
    printf ("Do \"help <command>\" to print options of that command.\n");
    printf ("\"q\" to quit.\n");
    printf ("\nList of commands\n\tmotor/m\n\tdyn/a\n\tpower/p\n");
}

void help_motor () {
    cmd_ok = 1;
    printf ("\n\tmotor/m\n");
    printf ("\t\t-10%s - %s","status","print each motor's PWM value. PWM values range from -100 to 100\n");
    printf ("\t\t-10%s - %s","<N> <pwm>","set motor N to PWM value pwm. For example \"motor 3 -55\".\n");
    printf ("\t\t-10%s - %s","all stop","stop all motors.\n");
    printf ("\t\t-10%s - %s","fwd <pwm>","set the 2 horizontal motors to pwm.\n");
    printf ("\t\t-10%s - %s","fwd stop","stops fwd motors. same as \"m fwd 0\".\n");
    printf ("\t\t-10%s - %s","rise <pwm>","set the 3 veritcal motors to pwm.\n");
    printf ("\t\t-10%s - %s","rise stop","stops vertical motors. same as \"m rise 0\".\n");
}

void help_dyn () {
    cmd_ok = 1;
    printf ("\n\tdyn/d\n");
    printf ("\t\t-10%s - %s","status","prints accelerometer and depth readings.\n");
}

void help_power () {
    cmd_ok = 1;
    printf ("\n\tpower/p\n");
    printf ("\t\t-10%s - %s","status","prints power status.");
    printf ("\t\t-10%s - %s","on","turns power on.");
    printf ("\t\t-10%s - %s","off","turns power off.");
}
