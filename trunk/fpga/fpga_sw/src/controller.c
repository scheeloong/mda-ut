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
// 200 is ZERO_PWM
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

/// Ritchie's Stuff
// I'm kinda confused about how the PD controller works so I wrote my own version of calculate_pid
#ifdef USE_PID_2
#include "pid.h"

static Controller_PID PID_Roll; // 3 controllers for controlling each DOF we care about
static Controller_PID PID_Pitch;
static Controller_PID PID_Depth;

void pid_init () // call this anytime before calling calculate_pid2
{
    // set up PID values
    PID_Reset (&PID_Roll);
    PID_Roll.Const_P = ROLL_CONST_P;
    PID_Roll.Const_I = ROLL_CONST_I;
    PID_Roll.Const_D = ROLL_CONST_D;
    PID_Roll.Alpha = ROLL_ALPHA;
    
    PID_Reset (&PID_Pitch);
    PID_Pitch.Const_P = PITCH_CONST_P;
    PID_Pitch.Const_I = PITCH_CONST_I;
    PID_Pitch.Const_D = PITCH_CONST_D;
    PID_Pitch.Alpha = PITCH_ALPHA;
    
    PID_Reset (&PID_Depth);
    PID_Depth.Const_P = PITCH_CONST_P;
    PID_Depth.Const_I = PITCH_CONST_I;
    PID_Depth.Const_D = PITCH_CONST_D;
    PID_Depth.Alpha = PITCH_ALPHA;
}

int motor_force_to_pwm (HW_NUM force) { // assume this conversion is linear for now
    int pwm = force * FACTOR_FORCE_TO_PWM;
    pwm = (pwm > 200) ? 200 : pwm;
    pwm = (pwm < -200) ? -200 : pwm;
    
    return pwm;
}

void calculate_pid_2()
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
   
   /** Ritchie - At this point I assume that current_orientation.pitch and .roll should be controlled towards zero
    *            and .depth should be controlled towards target_orientation.depth
    *            and nothing else matters just yet
    */
   
   // update the controller with the new values
   PID_Update (&PID_Roll, current_orientation.roll);
   PID_Update (&PID_Pitch, current_orientation.pitch);
   PID_Update (&PID_Depth, current_orientation.depth - target_orientation.depth);
   
   // calculate the force required
   HW_NUM Roll_Force_Needed = FACTOR_PID_ROLL_TO_FORCE * PID_Output(PID_Roll);
   HW_NUM Pitch_Force_Needed = FACTOR_PID_PITCH_TO_FORCE * PID_Output(PID_Pitch);
   HW_NUM Depth_Force_Needed = FACTOR_PID_DEPTH_TO_FORCE * PID_Output(PID_Depth);
    
   /** orientation stability - the signs are almost surely wrong 
    *  If the COM is off center we would have some sort of factors here instead of 0.5
    */
   M_FRONT_LEFT = HALF_PWM + 0.5*motor_force_to_pwm(Roll_Force_Needed) - 0.25*motor_force_to_pwm(Pitch_Force_Needed);
   M_FRONT_RIGHT = HALF_PWM - 0.5*motor_force_to_pwm(Roll_Force_Needed) - 0.25*motor_force_to_pwm(Pitch_Force_Needed);
   M_REAR = HALF_PWM + 0.5*motor_force_to_pwm(Pitch_Force_Needed);
   M_LEFT = HALF_PWM + (speed) + (heading); // M_LEFT and M_RIGHT are same as before
   M_RIGHT = HALF_PWM + (speed) - (heading);
   
   /** depth control. Again if COM off center use different factors
    */
   M_FRONT_LEFT -= 0.25*motor_force_to_pwm(Depth_Force_Needed);
   M_FRONT_RIGHT -= 0.25*motor_force_to_pwm(Depth_Force_Needed);
   M_REAR -= 0.5*motor_force_to_pwm(Depth_Force_Needed);
   
   /** Note that motor_force_to_pwm returns a value between -200 and 200, and the factors are such that the sum of
    *  each factor for every motor adds up (absolutely) to 1.0. Physics son! 
    */
   
   // write the motor settings. I dont think we need to skip horiz motors as we can set them to 0 manually
   for ( i = 0; i < 5; i++ )
   {
      if ( motor_duty_cycle[i] < ZERO_PWM )
         motor_duty_cycle[i] = ZERO_PWM;
      if ( motor_duty_cycle[i] > FULL_PWM )
         motor_duty_cycle[i] = FULL_PWM;
      set_motor_duty_cycle(i, motor_duty_cycle[i]);  
   }  
}

#endif
