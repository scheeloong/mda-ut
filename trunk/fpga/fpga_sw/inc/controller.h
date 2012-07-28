// Function prototypes implemented in controller.c

#ifndef _MDA_CONTROLLER_H
#define _MDA_CONTROLLER_H

#define ABS(x) (((x) > 0) ? (x) : (-(x)))

/* acceleration data in x,y,z */
struct t_accel_data {
  alt_16 x, y, z;
};

struct orientation {
  // angles
  int pitch;
  int roll;
  int heading;

  // scalars
  int speed;
  int depth;
};

// Motor 1 = front, vertical, left; 
// Motor 2 = front, vertical, right;
// Motor 3 = center, horizontal, right;
// Motor 4 = center, horizontal, left;
// Motor 5 = rear, vertical;
#define M_FRONT_LEFT motor_duty_cycle[0]
#define M_FRONT_RIGHT motor_duty_cycle[1]
#define M_LEFT motor_duty_cycle[2]
#define M_RIGHT motor_duty_cycle[3]
#define M_REAR motor_duty_cycle[4]

void set_target_speed(int speed);
void set_target_heading(int heading);
void set_target_depth(int depth);

void get_orientation(struct t_accel_data *accel_data, struct orientation *orientation);

void calculate_pid();
void controller_output(int pitch_setting, int roll_setting, int heading, int speed, int depth_setting);

void calculate_pid_2();
void pid_init();

#endif
