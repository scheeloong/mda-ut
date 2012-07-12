// Function prototypes implemented in controller.c

#ifndef _MDA_CONTROLLER_H
#define _MDA_CONTROLLER_H

// Motor 1 = front, vertical, left; 
// Motor 2 = front, vertical, right;
// Motor 3 = center, horizontal, right;
// Motor 4 = center, horizontal, left;
// Motor 5 = rear, vertical;
#define M_FRONT_LEFT motor_duty_cycle[0]
#define M_FRONT_RIGHT motor_duty_cycle[1]
#define M_LEFT motor_duty_cycle[2]
#define M_RIGHT motor_duty_cycle[3]
#define M_REAR motor_duty_cycle[4]

void controller_output(int pitch_setting, int roll_setting, int depth_setting,int heading, int velocity);

#endif
