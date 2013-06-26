#include "ActuatorOutput.h"
#include "SubmarineSingleton.h"
#include "../scripts/scripts.h"

#define SPEED_FACTOR 15 /* Tune the submarine speed offset to sort of resemble the simulator */

ActuatorOutputSubmarine::ActuatorOutputSubmarine()
{
  SubmarineSingleton::get_instance().register_instance();
}

ActuatorOutputSubmarine::~ActuatorOutputSubmarine()
{
}

void ActuatorOutputSubmarine::set_attitude_absolute(ATTITUDE_DIRECTION dir, int val)
{
  static int speed_val = 0;

  switch (dir) {
    case (SPEED):
      if (val != speed_val) {
          dyn_set_target_speed(val * SPEED_FACTOR);
          speed_val = val;
      }
      break;
    case (DEPTH):
      SubmarineSingleton::get_instance().set_target_depth(val);
      break;
    case (YAW):
      SubmarineSingleton::get_instance().set_target_yaw(val);
      break;
    default:
      break;
  }
}

int ActuatorOutputSubmarine::get_target_attitude(ATTITUDE_DIRECTION dir)
{
  switch(dir) {
    case YAW:
      return SubmarineSingleton::get_instance().get_target_yaw();
    case DEPTH:
      return SubmarineSingleton::get_instance().get_target_depth();
      break;
    default:
      puts("Unsupported functionality, returning 0\n");
      return 0;
  }
}

void ActuatorOutputSubmarine::stop()
{
  set_attitude_absolute(SPEED, 0);
}

void ActuatorOutputSubmarine::special_cmd(SPECIAL_COMMAND cmd)
{
  switch(cmd) {
    case (SUB_POWER_ON):
      power_on();
      break;
    case (SUB_STARTUP_SEQUENCE):
      SubmarineSingleton::get_instance().set_target_yaw(get_yaw());
      SubmarineSingleton::get_instance().set_target_depth(get_depth());
      startup_sequence();
      break;
    case (SUB_POWER_OFF):
      power_off();
      break;
    default:
      break;
  }
}

void ActuatorOutputSubmarine::set_depth_constants(double P, double I, double D, double Alpha)
{
  set_pid_depth(P, I, D, Alpha);
}

void ActuatorOutputSubmarine::set_pitch_constants(double P, double I, double D, double Alpha)
{
  set_pid_pitch(P, I, D, Alpha);
}

void ActuatorOutputSubmarine::set_roll_constants(double P, double I, double D, double Alpha)
{
  set_pid_roll(P, I, D, Alpha);
}

void ActuatorOutputSubmarine::set_yaw_constants(double P, double I, double D, double Alpha)
{
  set_pid_yaw(P, I, D, Alpha);
}
