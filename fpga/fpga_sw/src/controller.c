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

#include "pid.h"
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
  orientation->pitch = (z_squared + y_squared == 0) ? 90 : atan2(accel_data->x,sqrt(z_squared + y_squared)) * RAD_TO_DEG;
  orientation->roll = (z_squared == 0) ? 90 : atan2(accel_data->y,sqrt(2*z_squared)) * RAD_TO_DEG;
}

// The PID controller!

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

    // Initialize target orientation
    set_target_depth(get_depth());
    set_target_speed(0);
    //set_target_heading(get_heading());
}

int motor_force_to_pwm (HW_NUM force) { // assume this conversion is linear for now
    int pwm = force * FACTOR_FORCE_TO_PWM;
    pwm = (pwm > 400) ? 400 : pwm;
    pwm = (pwm < -400) ? -400 : pwm;
    
    return pwm;
}

void calculate_pid()
{
   // Get current orientation data
   struct t_accel_data accel_data;   
   
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
   HW_NUM Roll_Force_Needed = FACTOR_PID_ROLL_TO_FORCE * PID_Output(&PID_Roll);
   HW_NUM Pitch_Force_Needed = FACTOR_PID_PITCH_TO_FORCE * PID_Output(&PID_Pitch);
   HW_NUM Depth_Force_Needed = FACTOR_PID_DEPTH_TO_FORCE * PID_Output(&PID_Depth);
    
   /** orientation stability - the signs are almost surely wrong 
    *  If the COM is off center we would have some sort of factors here instead of 0.5
    */
   M_FRONT_LEFT = HALF_PWM + 0.5*motor_force_to_pwm(Roll_Force_Needed) - 0.25*motor_force_to_pwm(Pitch_Force_Needed);
   M_FRONT_RIGHT = HALF_PWM - 0.5*motor_force_to_pwm(Roll_Force_Needed) - 0.25*motor_force_to_pwm(Pitch_Force_Needed);
   M_REAR = HALF_PWM + 0.5*motor_force_to_pwm(Pitch_Force_Needed);
   M_LEFT = get_motor_duty_cycle(2); // M_LEFT and M_RIGHT are same as before
   M_RIGHT = get_motor_duty_cycle(3);
   
   /** depth control. Again if COM off center use different factors
    */
   M_FRONT_LEFT -= 0.25*motor_force_to_pwm(Depth_Force_Needed);
   M_FRONT_RIGHT -= 0.25*motor_force_to_pwm(Depth_Force_Needed);
   M_REAR -= 0.5*motor_force_to_pwm(Depth_Force_Needed);
   
   /** Note that motor_force_to_pwm returns a value between -400 and 400, and the factors are such that the sum of
    *  each factor for every motor adds up (absolutely) to 1.0. Physics son! 
    */
   
   // write the motor settings
   int i;
   for ( i = 0; i < 5; i++ )
   {
      if ( motor_duty_cycle[i] < ZERO_PWM )
         motor_duty_cycle[i] = ZERO_PWM;
      if ( motor_duty_cycle[i] > FULL_PWM )
         motor_duty_cycle[i] = FULL_PWM;
      set_motor_duty_cycle(i, motor_duty_cycle[i]);  
   }  
}

