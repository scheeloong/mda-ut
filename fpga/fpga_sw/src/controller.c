/*
 * utils.c
 *
 * Useful utility functions
 *
 * Author: Ritchie
 */

#include "alt_types.h"
#include "io.h"
#include "system.h"
#include "sys/alt_stdio.h"

#include "controller.h"
#include "utils.h"
#include "settings.h"

static int motor_duty_cycle[NUM_MOTORS];

// This function is responsible for calling the motor setting signals.
// An input of 200 is the neutral position for the specific axis input.
// An input of (200,200,200,200,200) would mean for the object to remain stationary. 
// Range of input = (0,400)
// duty cycle range = (200,824), centered at 512, dont go full range. 
void controller_output(int pitch_setting, int roll_setting, int depth_setting,int heading, int velocity)
{
   int i;
   int vertical_thrust_steady_state = 0; //Tune for some number better for better depth controller
   
   pitch_setting -= ZERO_PWM; // adjust for zero offset
   roll_setting -= ZERO_PWM;
   depth_setting -= ZERO_PWM;
        
   M_FRONT_LEFT = HALF_PWM + vertical_thrust_steady_state + depth_setting + pitch_setting + roll_setting; 
   M_FRONT_RIGHT = HALF_PWM + vertical_thrust_steady_state + depth_setting + pitch_setting - roll_setting;
   M_LEFT = HALF_PWM + (velocity) + (heading);
   M_RIGHT = HALF_PWM + (velocity) - (heading);
   M_REAR = HALF_PWM + vertical_thrust_steady_state + depth_setting - pitch_setting;

   // write the motor duty cycles to FPGA
   for ( i = 0; i < 5; i++ )
   {
      if ( motor_duty_cycle[i] < ZERO_PWM )
         motor_duty_cycle[i] = ZERO_PWM;
      if ( motor_duty_cycle[i] > FULL_PWM )
         motor_duty_cycle[i] = FULL_PWM;
      set_motor_duty_cycle(i, motor_duty_cycle[i]);  
   }  
}
