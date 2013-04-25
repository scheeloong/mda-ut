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

bool ActuatorOutputSubmarine::set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir, int delta)
{
  switch(dir) {
    case REVERSE:
    case LEFT:
    case SINK:
      delta *= -1;
      break;
    default:
      break;
  }
  int current_yaw, target_yaw, current_depth, target_depth;
  switch(dir) {
    case FORWARD:
    case REVERSE:
      set_attitude_absolute(SPEED, delta); // doesn't make sense to change the speed, set absolute
      return true;
    case RIGHT:
    case LEFT:
      set_attitude_absolute(SPEED, 0); // stop forward speed
      // Only set yaw change if yaw is stable
      current_yaw = get_yaw();
      target_yaw = SubmarineSingleton::get_instance().get_target_yaw();
      if (abs(current_yaw - target_yaw) <= stable_yaw_threshold || abs(current_yaw - target_yaw) >= 360 - stable_yaw_threshold) {
        set_attitude_absolute(YAW, current_yaw + delta);
        return true;
      } else {
        return false;
      }
    case RISE:
    case SINK:
      set_attitude_absolute(SPEED, 0); // stop forward speed
      // Only set depth change if depth is stable
      current_depth = get_depth();
      target_depth = SubmarineSingleton::get_instance().get_target_depth();
      if (abs(current_depth - target_depth) <= stable_depth_threshold) {
        set_attitude_absolute(YAW, current_depth + delta);
        return true;
      } else {
        return false;
      }
    default:
      return false;
  }
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
  set_attitude_absolute(YAW, get_yaw());
  set_attitude_absolute(DEPTH, get_depth());
}

void ActuatorOutputSubmarine::special_cmd(SPECIAL_COMMAND cmd)
{
  switch(cmd) {
    case (SUB_POWER_ON):
      power_on();
      break;
    case (SUB_STARTUP_SEQUENCE):
      startup_sequence();
      SubmarineSingleton::get_instance().set_target_yaw(SubmarineSingleton::get_instance().get_target_yaw());
      SubmarineSingleton::get_instance().set_target_depth(SubmarineSingleton::get_instance().get_target_depth());
      break;
    case (SUB_POWER_OFF):
      power_off();
      break;
    default:
      break;
  }
}
