#include "ActuatorOutput.h"
#include "SubmarineSingleton.h"

ActuatorOutputSubmarine::ActuatorOutputSubmarine()
{
  SubmarineSingleton::get_instance().increment_instances();
  // no need to register anything with the singleton
}

ActuatorOutputSubmarine::~ActuatorOutputSubmarine()
{
  SubmarineSingleton::get_instance().decrement_instances();
}
