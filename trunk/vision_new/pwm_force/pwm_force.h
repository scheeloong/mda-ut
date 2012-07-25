#ifndef _PWM_FORCE_H_
#define _PWM_FORCE_H_

#include <stdio.h>
#include <math.h>

#define NUM_ELEMENTS 80


// this aims to implement motor linearization for the motors of the sub.
// what this does is given a force to be applied, it would calculate the 
// pwm necessary to balance the sub (2 back motors each equal 1/2 a front motor)
// given by the equations from the data.

// the ranges given by PWM < -20 and PWM > 20 (not after PWM > |70| ) are linear, so 
// the two linear equations will be used.
// the rest of the values outside that range will be used by the quartic equation 
// that most accurately reflects the overall (all) data.

// note: pwms are ints!!!!!!!!!
void pwm_force();

// next, are the functions that represent the equations to be used:
// the linear equations
int pwm_greater_than(double force);

int pwm_lesser_than(double force);

// the quartic equation is much more complicated, we need a lookup for this. Also
// do not use factoring polynomials since this makes the programming/math ugly

double force_cubic(int pwm);

int inverse_cubic(double force);

#define SIZE 200
//int pwm_of_force[SIZE];

void init_lookup();

int pwm_force_pos (double force);
int pwm_force_neg (double force);

#endif