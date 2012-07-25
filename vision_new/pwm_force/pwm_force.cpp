#include "pwm_force.h"
#include <stdio.h>
#include <stdlib.h>
// given a force, this function returns the pwm to generate that force using the equations above
// or the lookup, if necessary.

// declare this array so it can be used on initializing the look-up
int pwm_of_force[SIZE]; // array of pwms
double lookup_force[SIZE]; // array of forces
int pwm_greater_than(double force) {
  return (int)((force + 0.698636363)/0.045);
}

int pwm_lesser_than(double force) {
  return (int)((force - 0.82990909)/0.0502);
}

double force_cubic(int pwm) {
  double har = (1.888e-5)*pow(pwm,3) - (6.593e-6)*pow(pwm,2) + (4.320e-3)*pwm;
  //printf("%f\n",har); // we don'tneed this
  return har;
}

/*the inverse_cubic() will not be used, as it causes complications */

int inverse_cubic(double x){
  //the inverse of the cubic is very ugly in appearance, so it will be broken down into bits.
  //source: http://www.wolframalpha.com/input/?i=find+inverse+of+y+%3D+%281.888e-5%29*x^3+-+%286.593e-6%29*x^2+%2B+%284.320e-3%29*x
  double a = 1.38739*pow(10,9)*sqrt(1.20303*pow(10,19)*pow(x,2)-1.20976*pow(10,16)*x+7.60966*pow(10,15))-4.81213*pow(10,18)*x+2.41952*pow(10,15);
  double b = pow(a,0.33333);
  double term_one = 0.116402 - 0.0000176554*b;
  double term_two = (4.31923*pow(10,6))/b;
  
  double inverse_result = term_one + term_two;
  return (int)(inverse_result);
}


void init_lookup() {
  int i;
  for (i = 0; i < SIZE; i++) {
    double force = (double)((i - 100.0) / 10000.0);
    pwm_of_force[i] = inverse_cubic(force);
    printf("%d\n", pwm_of_force[i]);
  }
}

int pwm_force_pos (double force) {
  int positive_pwm = pwm_greater_than (force);
  
  if( positive_pwm >= 20 && positive_pwm <= 70 ) { // acceptance linear range
    return positive_pwm;
  }
  else {
    return inverse_cubic(force);
    }
  }


int pwm_force_neg (double force) {
  int negative_pwm = pwm_lesser_than (force);
  
  if( negative_pwm <= -20 && negative_pwm >= -70 )  // acceptance linear range
    return negative_pwm;
  
  else 
    return inverse_cubic(force);
    
  }
    

void pwm_force() {
  
  init_lookup();
  
  while(1){
    
    char str[256];
    double force;

    int front_pwm, back_pwm1, back_pwm2;
    printf("Enter a force for a pwm to generate\n");
  
    scanf("%s", str);
    force = atof(str);

    printf("Force entered: %f\n", force);
  
    if( force < 0)
      front_pwm = pwm_force_neg(force);
    else
      front_pwm = pwm_force_pos(force);
    
    
    // with the pwm for the front motor, divide that same number by two to distribute 
    // pwm for the two back motors
    back_pwm1 = front_pwm/2;
    back_pwm2 = front_pwm/2;
  
    printf("FRONT PWM: %d\n", front_pwm);
    printf("BACK PWM 1: %d\n",back_pwm1);
    printf("BACK PWM 2: %d\n",back_pwm2);
    
  }
}