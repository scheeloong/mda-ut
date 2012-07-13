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

// Structures used by the PD controller for stabilization
struct orientation target_orientation = {};
struct orientation current_orientation = {};
struct orientation previous_orientation = {};
// Structure containing various PD parameters
struct PD_controller_inputs PD_controller_inputs = {};
// Structure containing PD controller feedback values
struct PD_controller_error_values PD_controller_error_values = {};

static int motor_duty_cycle[NUM_MOTORS];

// Set target values for the controller

void set_target_speed(int speed)
{
   speed = (speed > 157) ? 157 : speed;
   speed = (speed < -157) ? -157 : speed;
   target_orientation.speed = speed;
}

void set_target_heading(int heading)
{
   heading = (heading > 157) ? 157 : heading;
   heading = (heading < -157) ? -157 : heading;
   target_orientation.heading = heading;
}

void set_target_depth(int depth)
{
   target_orientation.depth = depth;
}

// Get orientation from acceleration
void get_orientation(struct t_accel_data *accel_data, struct orientation *orientation)
{
  // Calculation orientation
  int z_squared = accel_data->z*accel_data->z, y_squared = accel_data->y*accel_data->y;

  // pitch and roll are zero when sub is "flat"
  orientation->pitch = (z_squared + y_squared == 0) ? 90 : atan(accel_data->x/sqrt(z_squared + y_squared)) * RAD_TO_DEG;
  orientation->roll = (z_squared == 0) ? 90 : atan(accel_data->y/sqrt(2*z_squared)) * RAD_TO_DEG;
}

// Calculate PID values and then set the motor values
void calculate_pid()
{
   // Get current orientation data
   struct t_accel_data accel_data;   
   int pitch_setting = ZERO_PWM;
   int roll_setting = ZERO_PWM;
   int depth_setting = ZERO_PWM;
   //int heading = ZERO_PWM;
   
   // Calculate orientation data
   previous_orientation = current_orientation;
   get_accel(&accel_data);
   get_orientation(&accel_data, &current_orientation);
	
   // Calculate PD error values	
   // I assume that I only want stabilization (target_orientation is set from main notebook master)
   PD_controller_error_values.pitch_D = current_orientation.pitch - target_orientation.pitch - PD_controller_error_values.pitch_P;
   PD_controller_error_values.roll_D = current_orientation.roll - target_orientation.roll - PD_controller_error_values.roll_P;
   PD_controller_error_values.depth_D = current_orientation.depth - target_orientation.depth - PD_controller_error_values.depth_P;
   PD_controller_error_values.heading_D = current_orientation.heading - target_orientation.heading - PD_controller_error_values.heading_P;
   PD_controller_error_values.pitch_P = current_orientation.pitch - target_orientation.pitch;
   PD_controller_error_values.roll_P = current_orientation.roll - target_orientation.roll;
   PD_controller_error_values.depth_P = current_orientation.depth - target_orientation.depth;
   PD_controller_error_values.heading_P = current_orientation.heading - target_orientation.heading;
   
   // PD values yet to be determined
   pitch_setting = pitch_setting + (PD_controller_error_values.pitch_D>>4)*4 + (PD_controller_error_values.pitch_P>>4)*4;
   roll_setting = roll_setting + (PD_controller_error_values.roll_D>>4)*4 + (PD_controller_error_values.roll_P>>4)*4;
   
   // Set motor outputs
   controller_output(pitch_setting, roll_setting, target_orientation.heading, target_orientation.speed, depth_setting);
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
      // Skip left and right motors for now
      if (i == 2 | i == 3) continue;

      if ( motor_duty_cycle[i] < ZERO_PWM )
         motor_duty_cycle[i] = ZERO_PWM;
      if ( motor_duty_cycle[i] > FULL_PWM )
         motor_duty_cycle[i] = FULL_PWM;
      set_motor_duty_cycle(i, motor_duty_cycle[i]);  
   }  
}
