#include "Operation.h"

Mission::~Mission()
{
}

void Mission::initialize(AttitudeInput *a, ImageInput *i, ActuatorOutput *o)
{
  attitude_input = a;
  image_input = i;
  actuator_output = o;
}

void Mission::work()
{
  // TODO: implement
}
