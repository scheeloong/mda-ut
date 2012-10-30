#include "ActuatorOutput.h"
#include "SimulatorSingleton.h"

ActuatorOutputSimulator::ActuatorOutputSimulator()
{
  SimulatorSingleton::get_instance().increment_instances();
  // no need to register anything with the singleton
}

ActuatorOutputSimulator::~ActuatorOutputSimulator()
{
  SimulatorSingleton::get_instance().decrement_instances();
}

void ActuatorOutputSimulator::set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir, int delta)
{
  // TODO: implement
}

void ActuatorOutputSimulator::set_attitude_absolute(ATTITUDE_DIRECTION dir, int val)
{
  // TODO: implement
}

void ActuatorOutputSimulator::special_cmd(SPECIAL_COMMAND cmd)
{
  // TODO: implement
  switch(cmd) {
    default:
     break;
  }
}
