#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define ZERO_DC 512
#define PWM_FREQ 20

unsigned cmd_ok = 0;
unsigned power = 0;

pid_t child_pid = 0;
int p_stdin[2], p_stdout[2];
FILE *infp = NULL, *outfp = NULL;

int atoi_safe (char *str) {
    if (str == NULL) {
        printf ("Invalid input to atoi.\n");
        return 0;
    }
    return atoi (str);
}

void cmd_error () { printf ("**** Invalid command. Type \"help\" for command list\n"); }

void exit_safe ()
{
    power_off();

    fflush(infp);
    fflush(outfp);
    fclose(infp);
    fclose(outfp);

    close(p_stdin[0]);
    close(p_stdout[1]);
    if (child_pid > 0) {
       kill(child_pid, SIGINT);
    }

    exit(0);
}

pid_t popen2 (char *proc, int *infp, int *outfp)
{
    const int READ = 0, WRITE = 1;
    pid_t pid;

    if (pipe(p_stdin) || pipe(p_stdout)) {
        printf("pipe failed\n");
        return -1;
    }

    pid = fork();

    if (pid < 0) {
        return pid;
    } else if (pid == 0) {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);

        execl(proc, "", NULL);
        perror("Running execl command failed!");
        exit(1);
    }

    *infp = p_stdin[WRITE];
    *outfp = p_stdout[READ];

    return pid;
}

void spawn_term (char *proc)
{
    if (proc) {
        int inh, outh;
        child_pid = popen2(proc, &inh, &outh);
        infp = fdopen(inh, "w");
        outfp = fdopen(outh, "r");
    }
    if (!infp) {
        infp = stderr;
    }
    if (!outfp) {
        outfp = stdin;
    }
}

void read_from_term (char *cmd)
{
    fflush(outfp);
    fprintf(infp, "%s\n", cmd);
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
    int pwm;
    fscanf(outfp, "%d", &pwm);

    printf("pwm: %d\n", pwm);
}

void motor_set (int pwm, char motor_flags) {
    cmd_ok = 1;
    if (pwm < -PWM_MAX || pwm > PWM_MAX) {
        printf ("**** Invalid pwm. motor pwm unchanged.\n");
        return;
    }
    
    if (motor_flags & H_FRONT_LEFT) {
        printf ("set left front vertical motor to %d\n", pwm);
        fprintf (infp, "smd %d %x\n", M_FRONT_LEFT, pwm);
    }
    if (motor_flags & H_FRONT_RIGHT) {
        printf ("set right front vertical motor to %d\n", pwm);
        fprintf (infp, "smd %d %x\n", M_FRONT_RIGHT, pwm);
    }
    if (motor_flags & H_FWD_LEFT) {
        printf ("set left forward motor to %d\n", pwm);
        fprintf (infp, "smd %d %x\n", M_FWD_LEFT, pwm);
    }
    if (motor_flags & H_FWD_RIGHT) {
        printf ("set right forward motor to %d\n", pwm);
        fprintf (infp, "smd %d %x\n", M_FWD_RIGHT, pwm);
    }
    if (motor_flags & H_REAR) {
        printf ("set rear vertical motor to %d\n", pwm);
        fprintf (infp, "smd %d %x\n", M_REAR, pwm);
    }
}

void power_status () {
    cmd_ok = 1;
    printf("power is turned %s\n", (power) ? "on" : "off");
}

void power_on () {
    cmd_ok = 1;
    printf ("turned power on.\n");
    fprintf (infp, "p 1\n");
    fprintf (infp, "spf %x\n", PWM_FREQ);
    fprintf (infp, "smd a %x\n", ZERO_DC);
    fprintf (infp, "smf a\n");
    power = 1;
}

void power_off () {
    cmd_ok = 1;
    printf ("turned power off.\n");
    fprintf (infp, "p 0\n");
    fprintf (infp, "sms a\n");
    fprintf (infp, "smd a %x\n", ZERO_DC);
    power = 0;
}

void dyn_status () {
    cmd_ok = 1;
    int x, y, z, d;

    // Acceleration
    read_from_term("ga");
    fscanf(outfp, "raw: %d, %d, %d", &x, &y, &z);
    printf("(x,y,z) acceleration: %d %d %d\n", x, y, z);
    // Depth
    read_from_term("gd");
    fscanf(outfp, "%d", &d);
    printf("depth: %d\n", d);
}
