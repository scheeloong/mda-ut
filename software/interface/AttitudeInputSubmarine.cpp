#include "AttitudeInput.h"
#include "SubmarineSingleton.h"
#include "../scripts/scripts.h"

AttitudeInputSubmarine::AttitudeInputSubmarine()
{
  SubmarineSingleton::get_instance().increment_instances();
  // no need to register anything with the singleton
}

AttitudeInputSubmarine::~AttitudeInputSubmarine()
{
  SubmarineSingleton::get_instance().increment_instances();
}

// TODO: implement yaw, pitch and roll

// On the order of cm
int AttitudeInputSubmarine::depth()
{
  return get_depth();
}

// TODO: implement target yaw, pitch and roll
