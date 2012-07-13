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
