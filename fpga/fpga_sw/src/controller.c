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

//#define DEBUG_MSG

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

HW_NUM motor_force_to_pwm (HW_NUM force) {
    int pwm = ZERO_PWM + pwm_of_force(force*FACTOR_CONTROLLER_FORCE_TO_LBS);
    return pwm;
}

// the following function calculates the pwm needed for the 3 stabilizing motors. 
// It ensures motors are balanced and no motor exceeds 0.8*FULL_PWM by reducing 
// the force if any motor pwm does exceed the limit
#define MAX_FORCE_BALANCING_LOOPS 10
void stabilizing_motors_force_to_pwm (
        HW_NUM f_front_left, HW_NUM f_front_right, HW_NUM f_rear,
        HW_NUM *m_front_left, HW_NUM *m_front_right, HW_NUM *m_rear
)
{
    unsigned loops = 0;

    while (true && loops < MAX_FORCE_BALANCING_LOOPS) {
        // calculate the pwms
        *m_front_left = motor_force_to_pwm(f_front_left);
        *m_front_right = motor_force_to_pwm(f_front_right);
        *m_rear = motor_force_to_pwm(f_rear);

        HW_NUM pwm_limit = MAX_PWM;

        // if any pwm is out of bound
        if (ABS(*m_front_left) > pwm_limit ||
            ABS(*m_front_right) > pwm_limit ||
            ABS(*m_rear) > pwm_limit)
        {
            // reduce force
            f_front_left *= FORCE_REDUCTION_FACTOR;
            f_front_right *= FORCE_REDUCTION_FACTOR;
            f_rear *= FORCE_REDUCTION_FACTOR;
        }
        else
            break;

        loops++;
    }
}

void calculate_pid()
{
   update_depth_reading();

   static unsigned counter = 0;
   counter++;

   if (counter % NUM_DEPTH_VALUES != 0) {
      return;
   }

   // Get orientation data from IMU and depth from depth sensor
   get_imu_orientation(&current_orientation);
   current_orientation.depth = get_average_depth();
   
   /** Ritchie - At this point I assume that current_orientation.pitch and .roll should be controlled towards zero
    *            and .depth should be controlled towards target_orientation.depth
    *            and nothing else matters just yet
    */
   
   // update the controller with the new values
   PID_Update (&PID_Roll, current_orientation.roll);
   PID_Update (&PID_Pitch, current_orientation.pitch);
   PID_Update (&PID_Yaw, current_orientation.yaw);
   HW_NUM diff = (target_orientation.depth - current_orientation.depth);
   PID_Update (&PID_Depth, diff);
   
   // calculate the force required
   HW_NUM Roll_Force_Needed = FACTOR_PID_ROLL_TO_FORCE * PID_Output(&PID_Roll);
   HW_NUM Pitch_Force_Needed = FACTOR_PID_PITCH_TO_FORCE * PID_Output(&PID_Pitch);
   HW_NUM Depth_Force_Needed = FACTOR_PID_DEPTH_TO_FORCE * PID_Output(&PID_Depth);
   HW_NUM Yaw_Force_Needed = FACTOR_PID_YAW_TO_FORCE * PID_Output(&PID_Yaw);

   // Print some debug messages every so often...
#ifdef DEBUG_MSG 
   if (counter % (128*NUM_DEPTH_VALUES) == 0) {
	printf ("depth current = %d\n", current_orientation.depth);
	printf ("PID_Depth.P = %f\n", PID_Depth.P*PID_Depth.Const_P);
	printf ("PID_Depth.I = %f\n", PID_Depth.I*PID_Depth.Const_I);
	printf ("PID_Depth.D = %f\n", PID_Depth.D*PID_Depth.Const_D);
	printf ("Depth_PID: %f\n", Depth_Force_Needed);
    }
#endif
   /** orientation stability
    *  If the COM is off center we would have some sort of factors here instead of 0.5
    */

   HW_NUM m_front_left;
   HW_NUM m_front_right;
   HW_NUM m_rear;
   HW_NUM m_left = motor_force_to_pwm (0.5*Yaw_Force_Needed); // = get_motor_duty_cycle(2); // M_LEFT and M_RIGHT are same as before
   HW_NUM m_right = motor_force_to_pwm (-0.5*Yaw_Force_Needed); // = get_motor_duty_cycle(3);
   
   stabilizing_motors_force_to_pwm ( // this calculates the pwms for these 3 motors
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

