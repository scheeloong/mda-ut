#include "ControlAlgorithm.h"

ControlAlgorithmSubmarine::~ControlAlgorithmSubmarine()
{
}

void ControlAlgorithmSubmarine::initialize(AttitudeInput *a, ImageInput *i, ActuatorOutput *o)
{
  attitude_input = a;
  image_input = i;
  actuator_output = o;
}

void ControlAlgorithmSubmarine::work()
{
  // TODO: implement
}
