#include "pwm_force.h"
#include <stdio.h>
#include <stdlib.h>
// given a force, this function returns the pwm to generate that force using the equations above
// or the lookup, if necessary.
double pwm_force_pos (double force) {
  int positive_pwm = pwm_greater_than (force);
  
  if( positive_pwm > 20 && positive_pwm < 70 ) { // acceptance linear range
    return positive_pwm;
  }
  else {
    if(positive_pwm > 80) {
      printf("Error: PWM is not in acceptable range./n");
      return 0;
    }
  }
}

double pwm_force_neg (double force) {
  int negative_pwm = pwm_greater_than (force);
  
  if( negative_pwm < -20 && negative_pwm > -70 ) { // acceptance linear range
    return negative_pwm;
  }
  else {
    if(negative_pwm < -80) {
      printf("Error: PWM is not in acceptable range./n");
      return 0;
    }
  }
}    

int main() {
  init_lookup();
  char str[256];
  double force;
  
  double front_pwm, back_pwm1, back_pwm2;
  printf("Enter a force for a pwm to generate\n");
  
  scanf("%s", str);
  force = atof(str);

  printf("Force entered: %f\n", force);
  
  double pwm_pos = pwm_force_pos(force);
  double pwm_neg = pwm_force_neg(force);
  
  if(pwm_pos == 0 && pwm_neg == 0) { 
    // given a force, we find the approximate corresponding pwm from the lookups
    int i;
    for(i=0; i<NUM_ELEMENTS+1; i++) {
      if (force == array_of_force[i] ) {
	front_pwm = array_of_pwm[i];
	back_pwm1 = front_pwm/2;
	back_pwm2 = front_pwm/2;
	break;
      }
    }
    printf("FRONT PWM: %f\n", front_pwm);
    printf("BACK PWM 1: %f\n",back_pwm1);
    printf("BACK PWM 2: %f\n",back_pwm2);
  }
  else { // motor linearization
    printf("Positive PWM: %f\n", pwm_pos);
    printf("Negative PWM: %f\n", pwm_neg);
    
    front_pwm = pwm_pos;
    back_pwm1 = front_pwm/2;
    back_pwm2 = front_pwm/2;
    
    printf("FRONT PWM: %f\n", front_pwm);
    printf("BACK PWM 1: %f\n",back_pwm1);
    printf("BACK PWM 2: %f\n",back_pwm2);
  }
  return 0;
}