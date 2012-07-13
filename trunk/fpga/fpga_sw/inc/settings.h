// Set some #define values to use as settings

// allow the user to interact when using commands
#define INTERACTIVE

// enable the controller
#define ENABLE_CONTROLLER

#define CLOCK_SPEED 50000000
#define TIMER_RATE_IN_HZ 20

#define NUM_MOTORS 6
#define STR_LEN 30
#define RAD_TO_DEG 57.3  // 180/PI
#define MOTOR_CONTROLLER_0_DUTY_CYCLE (MOTOR_CONTROLLER_0_BASE + 32)

// duty cycle range = (200,824), centered at 512, dont go full range. 
#define ZERO_PWM 200
#define FULL_PWM 800 // true max is 824
#define HALF_PWM 512

#ifndef INTERACTIVE
#undef puts
#undef printf
#undef putchar
#else
#include <stdio.h>
#endif

/** Ritchie's PID controller settings. 
 * These are multiplicative constants in the PID equation
 * Alpha is the decay rate for Integral term
 */
// #define USE_PID_2
#ifdef USE_PID_2
#define ROLL_CONST_P 1
#define ROLL_CONST_I 1
#define ROLL_CONST_D 1
#define ROLL_ALPHA 0
#define PITCH_CONST_P 1
#define PITCH_CONST_I 1
#define PITCH_CONST_D 1
#define PITCH_ALPHA 0
#define DEPTH_CONST_P 1
#define DEPTH_CONST_I 1
#define DEPTH_CONST_D 1
#define DEPTH_ALPHA 0

/** The following constants define the scaling between PID controller outputs
 *  and the motor force. Say the PID reads a string of angles/depth values and spits
 *  out "30". This constant converts this relative control factor to relative motor force.
 *  We should try to keep the force around -100 to 100 or something
 */
#define FACTOR_PID_ROLL_TO_FORCE 1
#define FACTOR_PID_PITCH_TO_FORCE 1
#define FACTOR_PID_DEPTH_TO_FORCE 1

/** This converts the relative motor force to PWM */
#define FACTOR_FORCE_TO_PWM 1
#endif