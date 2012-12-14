#include "AttitudeInput.h"
#include "SubmarineSingleton.h"
#include "../scripts/scripts.h"

AttitudeInputSubmarine::AttitudeInputSubmarine()
{
  SubmarineSingleton::get_instance().register_instance();
}

AttitudeInputSubmarine::~AttitudeInputSubmarine()
{
}

// TODO: implement yaw, pitch and roll

// On the order of cm
int AttitudeInputSubmarine::depth()
{
  return get_depth();
}

// TODO: implement target yaw, pitch and roll
