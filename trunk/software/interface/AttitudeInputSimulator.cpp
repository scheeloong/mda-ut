#include "AttitudeInput.h"
#include "SimulatorSingleton.h"
#include "physical_model.h"

// global sim data
extern physical_model model;

AttitudeInputSimulator::AttitudeInputSimulator()
{
  SimulatorSingleton::get_instance().increment_instances();
  // no need to register anything with the singleton
}

AttitudeInputSimulator::~AttitudeInputSimulator()
{
  SimulatorSingleton::get_instance().decrement_instances();
}

// yaw, pitch and roll are in degrees
int AttitudeInputSimulator::yaw()
{
  return (int)model.angle.yaw;
}

int AttitudeInputSimulator::pitch()
{
  return (int)model.angle.pitch;
}

int AttitudeInputSimulator::roll()
{
  return (int)model.angle.roll;
}

// On the order of cm
int AttitudeInputSimulator::depth()
{
  // Should be taken from the simulator rather than hard-coded
  static const float POOL_HEIGHT = 8;
  return (int)(100*(POOL_HEIGHT - model.position.y));
}
