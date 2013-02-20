#include "ActuatorOutput.h"
#include "SubmarineSingleton.h"
#include "../scripts/scripts.h"

ActuatorOutputSubmarine::ActuatorOutputSubmarine()
{
  SubmarineSingleton::get_instance().register_instance();
}

ActuatorOutputSubmarine::~ActuatorOutputSubmarine()
{
}

void ActuatorOutputSubmarine::set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir, int delta)
{
  // TODO: implement
}

void ActuatorOutputSubmarine::set_attitude_absolute(ATTITUDE_DIRECTION dir, int val)
{
  switch (val) {
    case (SPEED):
      dyn_set_target_speed(val);
      break;
    case (DEPTH):
      dyn_set_target_depth(val);
      break;
    case (YAW):
      dyn_set_target_yaw(val);
      break;
    default:
      break;
  }
}

void ActuatorOutputSubmarine::stop()
{
  // TODO: implement
}

void ActuatorOutputSubmarine::special_cmd(SPECIAL_COMMAND cmd)
{
  switch(cmd) {
    case (SUB_POWER_ON):
      power_on();
      break;
    case (SUB_STARTUP_SEQUENCE):
      startup_sequence();
      break;
    case (SUB_POWER_OFF):
      power_off();
      break;
    default:
      break;
  }
}
