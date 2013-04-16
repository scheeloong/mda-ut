#include "ActuatorOutput.h"
#include "SubmarineSingleton.h"
#include "../scripts/scripts.h"

#define SPEED_FACTOR 5 /* Tune the submarine speed offset to sort of resemble the simulator */

ActuatorOutputSubmarine::ActuatorOutputSubmarine()
{
  SubmarineSingleton::get_instance().register_instance();
}

ActuatorOutputSubmarine::~ActuatorOutputSubmarine()
{
}

void ActuatorOutputSubmarine::set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir, int delta)
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
  switch(dir) {
    case FORWARD:
    case REVERSE:
      set_attitude_absolute(SPEED, delta); // doesn't make sense to change the speed, set absolute
      break;
    case RIGHT:
    case LEFT:
      set_attitude_absolute(SPEED, 0); // stop forward speed
      set_attitude_absolute(YAW, get_yaw() + delta);
      break;
    case RISE:
    case SINK:
      set_attitude_absolute(SPEED, 0); // stop forward speed
      set_attitude_absolute(DEPTH, get_depth() + delta);
      break;
  }
}

void ActuatorOutputSubmarine::set_attitude_absolute(ATTITUDE_DIRECTION dir, int val)
{
  switch (dir) {
    case (SPEED):
      dyn_set_target_speed(val * SPEED_FACTOR);
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
