/*
 * controller.c
 *
 * The submarine's motor controller.
 *
 * Author: Ritchie
 */

#include <math.h>

#include "alt_types.h"
#include "io.h"
#include "system.h"
#include "sys/alt_stdio.h"

#include "controller.h"
#include "utils.h"
#include "settings.h"

static int motor_duty_cycle[NUM_MOTORS];

// Get orientation from acceleration
void get_orientation(struct t_accel_data *accel_data, struct orientation *orientation)
{
  // Calculation orientation
  int z_squared = accel_data->z*accel_data->z, y_squared = accel_data->y*accel_data->y;

  // pitch and roll are zero when sub is "flat"
  orientation->pitch = (z_squared + y_squared == 0) ? 90 : atan(accel_data->x/sqrt(z_squared + y_squared)) * RAD_TO_DEG;
  orientation->roll = (z_squared == 0) ? 90 : atan(accel_data->y/sqrt(2*z_squared)) * RAD_TO_DEG;
}

// This function is responsible for calling the motor setting signals.
// An input of 200 is the neutral position for the specific axis input.
// An input of (200,200,200,200,200) would mean for the object to remain stationary. 
// Range of input = (0,400)
// duty cycle range = (200,824), centered at 512, dont go full range. 
void controller_output(int pitch_setting, int roll_setting, int heading, int speed, int depth_setting)
{
   int i;
   int vertical_thrust_steady_state = 0; //Tune for some number better for better depth controller
   
   pitch_setting -= ZERO_PWM; // adjust for zero offset
   roll_setting -= ZERO_PWM;
   depth_setting -= ZERO_PWM;
        
   M_FRONT_LEFT = HALF_PWM + vertical_thrust_steady_state + depth_setting + pitch_setting + roll_setting; 
   M_FRONT_RIGHT = HALF_PWM + vertical_thrust_steady_state + depth_setting + pitch_setting - roll_setting;
   M_LEFT = HALF_PWM + (speed) + (heading);
   M_RIGHT = HALF_PWM + (speed) - (heading);
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
