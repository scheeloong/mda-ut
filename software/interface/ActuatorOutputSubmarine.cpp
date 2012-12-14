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
  // TODO: implement
}

void ActuatorOutputSubmarine::stop()
{
  // TODO: implement
}

void ActuatorOutputSubmarine::special_cmd(SPECIAL_COMMAND cmd)
{
  // TODO: implement
  switch(cmd) {
    case (SUB_POWER_ON):
      power_on();
      break;
    case (SUB_POWER_OFF):
      power_off();
      break;
    default:
     break;
  }
}
