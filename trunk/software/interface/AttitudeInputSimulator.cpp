#include "AttitudeInput.h"
#include "physical_model.h"

// global sim data
extern physical_model model;

// The caller must construct and destruct the sim resource!
// This is because there may be multiple interfaces to the sim, but altogether,
// they can only be constructed and destructed once.

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
