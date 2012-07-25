#include "pwm_force.h"
#include <stdio.h>
#include <stdlib.h>
// given a force, this function returns the pwm to generate that force using the equations above
// or the lookup, if necessary.

// declare this array so it can be used on initializing the look-up
int pwm_of_force[SIZE];

int pwm_greater_than(double force) {
  return (int)((force + 0.698636363)/0.045);
}

int pwm_lesser_than(double force) {
  return (int)((force - 0.82990909)/0.0502);
}

double force_cubic(int pwm) {
  return (-5e-7)*pow(pwm, 3) + 0.0005*pow(pwm,2) + 0.0002*pwm + 0.1457;
}

int inverse_cubic(double x){
  // this function is the result of a cubic function's inverse, obtained from matlab
  
  return (int)(pow(((147573952589676412928*x)/5572392449786181 + pow((pow(((147573952589676412928*x)/5572392449786181 - 632102790618389393758244638159555047229893034175957/47470909521199675931189252779875122596859085800000),2) + 3122879011617526236642070885072818855886306351302285917744713791340273274419820904225939762730255706590632243/7042147658656018892397039849248736387998837701713675676482675488992600832524073443786755125000000000000),(1/2)) - 632102790618389393758244638159555047229893034175957/47470909521199675931189252779875122596859085800000),(1/3)) - 1461678031171954654252356464715037707/(19167628157058046751550999114050000*pow(((147573952589676412928*x)/5572392449786181 + pow((pow(((147573952589676412928*x)/5572392449786181 - 632102790618389393758244638159555047229893034175957/47470909521199675931189252779875122596859085800000),2) + 3122879011617526236642070885072818855886306351302285917744713791340273274419820904225939762730255706590632243/7042147658656018892397039849248736387998837701713675676482675488992600832524073443786755125000000000000),(1/2)) - 632102790618389393758244638159555047229893034175957/47470909521199675931189252779875122596859085800000),(1/3)) + 288282983532959/2476618866571636));
  
}


void init_lookup() {
  int i;
  for (i = 0; i < SIZE; i++) {
    double force = (i - 100) / 10000;
    pwm_of_force[i] = inverse_cubic(force);
    //print lookup
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