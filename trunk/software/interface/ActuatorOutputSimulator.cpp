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
