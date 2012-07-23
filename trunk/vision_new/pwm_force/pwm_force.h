#ifndef _PWM_FORCE_H_
#define _PWM_FORCE_H_

#include <stdio.h>

#define NUM_ELEMENTS 80


// this aims to implement motor linearization for the motors of the sub.
// what this does is given a force to be applied, it would calculate the 
// pwm necessary to balance the sub (2 back motors each equal 1/2 a front motor)
// given by the equations interpolated from the data.

// the ranges given by PWM < -20 and PWM > 20 (not after PWM > |70| ) are linear, so 
// the two linear equations will be used.
// the rest of the values outside that range will be used by the quartic equation 
// that most accurately reflects the overall (all) data.

double pwm_force(double force);

// some helper functions:
double square (double a) {
  return (a*a);
}

double cubic (double a) {
  return (a*a*a);
}

double quartic (double a) {
  return (a*a*a*a);
}

// next, are the functions that represent the equations to be used:
// the linear equations
double pwm_greater_than(double force) {
  return ((force + 0.698636363)/0.045);
}

double pwm_lesser_than(float force) {
  return (-(force + 0.82990909)/0.0502);
}

// the quartic equation is much more complicated, we need a lookup for this. Also
// do not use factoring polynomials since this makes the programming/math ugly

double force_quartic(double pwm) {
  return (-5e-8)*quartic(pwm) - (5e-7)*cubic(pwm) + 0.0008*square(pwm) + 0.0002*pwm - 0.0303;
}
// we need two lookups: one for storing an array of pwms and a corresponding lookup
// that stores an array of forces

// we need these arrays to be global since it has to be accessed somewhere by FGPA

double array_of_pwm[NUM_ELEMENTS+1];
double array_of_force[NUM_ELEMENTS+1];

// this function initializes the lookups declared from the header file for array of pwms
// and array of forces

void init_lookup() {
  int i;
  for(i=0; i<NUM_ELEMENTS+1; i++) {
    array_of_pwm[i] = i;
    array_of_force[i] = force_quartic(array_of_pwm[i]);
  }
}

double pwm_force_pos (double force);
double pwm_force_neg (double force);

#endif