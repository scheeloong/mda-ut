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
#include "pid.h"
#include "pwm_force.h"
#include "rs232.h"
#include "settings.h"
#include "utils.h"

#define MAX_YAW 360

// Structures used by the PD controller for stabilization
struct orientation target_orientation = {};
struct orientation current_orientation = {};
// Structure containing various PD parameters
struct PD_controller_inputs PD_controller_inputs = {};
// Structure containing PD controller feedback values
struct PD_controller_error_values PD_controller_error_values = {};

// Data to average depth readings
int depth_values[NUM_DEPTH_VALUES] = {};
int depth_sum = 0;

// Motor duty cycles
static int motor_duty_cycle[NUM_MOTORS];

// Set target values for the controller

void set_target_speed(int speed)
{
   target_orientation.speed = speed;
}

void set_target_heading(int heading)
{
   target_orientation.yaw = heading;
}

void set_target_depth(int depth)
{
   target_orientation.depth = depth;
}

// Update depth reading by tracking the sum
// of the last NUM_DEPTH_VALUES readings
void update_depth_reading()
{
   static unsigned idx = 0;

   depth_sum -= depth_values[idx];
   int depth_reading = get_depth();
   depth_values[idx] = depth_reading;
   depth_sum += depth_reading;

   // Rotate idx
   idx++;
   idx %= NUM_DEPTH_VALUES;
}

// Get an average depth of the last NUM_DEPTH_VALUES readings
int get_average_depth()
{
   return depth_sum / NUM_DEPTH_VALUES;
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

static Controller_PID PID_Roll; // 4 controllers for controlling each DOF we care about
static Controller_PID PID_Pitch;
static Controller_PID PID_Yaw;
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
    
    PID_Reset (&PID_Yaw);
    PID_Yaw.Const_P = YAW_CONST_P;
    PID_Yaw.Const_I = YAW_CONST_I;
    PID_Yaw.Const_D = YAW_CONST_D;
    PID_Yaw.Alpha = YAW_ALPHA;
    
    PID_Reset (&PID_Depth);
    PID_Depth.Const_P = DEPTH_CONST_P;
    PID_Depth.Const_I = DEPTH_CONST_I;
    PID_Depth.Const_D = DEPTH_CONST_D;
    PID_Depth.Alpha = DEPTH_ALPHA;

    // Initialize target orientation
    set_target_depth(0);
    set_target_speed(0);
    set_target_heading(0);

    // Initialize motor linearization lookup table
    init_lookup();
}

double motor_force_to_pwm (double force) {
    int pwm = ZERO_PWM + pwm_of_force(force*FACTOR_CONTROLLER_FORCE_TO_LBS);
    return pwm;
}

// the following function calculates the pwm needed for 2 or 3 stabilizing motors. 
// It ensures motors are balanced and no motor exceeds 0.8*FULL_PWM by reducing 
// the force if any motor pwm does exceed the limit.
#define MAX_FORCE_BALANCING_LOOPS 10
void stabilizing_motors_force_to_pwm (
        double f_1, double f_2, double f_3,
        double *m_1, double *m_2, double *m_3
)
{
    unsigned loops;

    for (loops = 0; loops < MAX_FORCE_BALANCING_LOOPS; loops++) {
        // calculate the pwms
        *m_1 = motor_force_to_pwm(f_1);
        *m_2 = motor_force_to_pwm(f_2);
        if (m_3) {
            *m_3 = motor_force_to_pwm(f_3);
        }

        double pwm_limit = MAX_PWM;

        // if any pwm is out of bound
        if (ABS(*m_1) > pwm_limit ||
            ABS(*m_2) > pwm_limit ||
            (m_3 && ABS(*m_3) > pwm_limit))
        {
            // reduce force
            f_1 *= FORCE_REDUCTION_FACTOR;
            f_2 *= FORCE_REDUCTION_FACTOR;
            f_3 *= FORCE_REDUCTION_FACTOR;
        }
        else {
            break;
        }
    }
}

void calculate_pid()
{
   // Get orientation data from IMU and depth from depth sensor
   get_imu_orientation(&current_orientation);
   current_orientation.depth = get_average_depth();
   
   /* current_orientation.pitch and .roll are controlled towards zero
    * and .depth is controlled towards target_orientation.depth
    */
   
   // update the controller with the new pitch and roll values
   PID_Update (&PID_Roll, current_orientation.roll);
   PID_Update (&PID_Pitch, current_orientation.pitch);

   // normalize yaw difference between -180 and 180 degrees
   double delta_yaw = (target_orientation.yaw - current_orientation.yaw);
   delta_yaw = fmod(delta_yaw, MAX_YAW);
   if (delta_yaw > MAX_YAW/2) {
     delta_yaw -= MAX_YAW;
   } else if (delta_yaw < -MAX_YAW/2) {
     delta_yaw += MAX_YAW;
   }
   PID_Update (&PID_Yaw, delta_yaw);

   // calculate depth difference
   double delta_depth = (target_orientation.depth - current_orientation.depth);
   PID_Update (&PID_Depth, delta_depth);
   
   // calculate the force required
   double Roll_Force_Needed = FACTOR_PID_ROLL_TO_FORCE * PID_Output(&PID_Roll);
   double Pitch_Force_Needed = FACTOR_PID_PITCH_TO_FORCE * PID_Output(&PID_Pitch);
   double Depth_Force_Needed = FACTOR_PID_DEPTH_TO_FORCE * PID_Output(&PID_Depth);
   double Yaw_Force_Needed = FACTOR_PID_YAW_TO_FORCE * PID_Output(&PID_Yaw);
   double Forward_Force_Needed = FACTOR_SPEED_TO_FORCE * target_orientation.speed;

   /** orientation stability
    *  If the COM is off center we would have some sort of factors here instead of 0.5
    */

   double m_front_left, m_front_right, m_rear, m_left, m_right;
   
   stabilizing_motors_force_to_pwm ( // this calculates the pwms for yaw motors
      0.5*Yaw_Force_Needed + Forward_Force_Needed, // m_left
      -0.5*Yaw_Force_Needed + Forward_Force_Needed, // m_right
      0, // unused
      &m_left,
      &m_right,
      NULL
   );

   stabilizing_motors_force_to_pwm ( // this calculates the pwms for pitch and roll motors
      -0.5*Roll_Force_Needed + 0.25*Pitch_Force_Needed + 0.25*Depth_Force_Needed, // m_front_left
      0.5*Roll_Force_Needed + 0.25*Pitch_Force_Needed + 0.25*Depth_Force_Needed, // m_front_right
      -0.5*Pitch_Force_Needed + 0.5*Depth_Force_Needed, // m_rear
      &m_front_left,
      &m_front_right,
      &m_rear
   );

   M_FRONT_LEFT = (int)m_front_left;
   M_FRONT_RIGHT = (int)m_front_right;
   M_LEFT = (int)m_left;
   M_RIGHT = (int)m_right;
   M_REAR = (int)m_rear;

   /** Note that motor_force_to_pwm returns a value between -400 and 400, and the factors are such that the sum of
    *  each factor for every motor adds up (absolutely) to 1.0. Physics son! 
    */
   
   // write the motor settings
   int i;
   for ( i = 0; i < 5; i++ )
   {
      set_motor_duty_cycle(i, motor_duty_cycle[i]);  
   }  
}

void print_debug_controller()
{
   double Depth_Force_Needed = FACTOR_PID_DEPTH_TO_FORCE * PID_Output(&PID_Depth);

   // Print controller debug messages
   printf ("PID_Depth.P = %f\n", PID_Depth.P*PID_Depth.Const_P);
   printf ("PID_Depth.I = %f\n", PID_Depth.I*PID_Depth.Const_I);
   printf ("PID_Depth.D = %f\n", PID_Depth.D*PID_Depth.Const_D);
   printf ("Depth_PID: %f\n", Depth_Force_Needed);
}
