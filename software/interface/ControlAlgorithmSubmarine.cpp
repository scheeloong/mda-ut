#include "ControlAlgorithm.h"

ControlAlgorithmSubmarine::~ControlAlgorithmSubmarine()
{
}

void ControlAlgorithmSubmarine::initialize(AttitudeInput *a, ImageInput *i)
{
  attitude_input = a;
  image_input = i;
}

void ControlAlgorithmSubmarine::work()
{
  // TODO: implement
}
