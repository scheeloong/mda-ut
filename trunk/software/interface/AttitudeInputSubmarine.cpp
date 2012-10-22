#include "AttitudeInput.h"
#include "CHeader.h"

AttitudeInputSubmarine::AttitudeInputSubmarine()
{
  SubmarineSingleton s = SubmarineSingleton::get_instance();
  s.increment_instance();
  // no need to register anything with the singleton
}

AttitudeInputSubmarine::~AttitudeInputSubmarine()
{
  SubmarineSingleton s = SubmarineSingleton::get_instance();
  s.decrement_instance();
}

// TODO: implement yaw, pitch and roll

// On the order of cm
int AttitudeInputSubmarine::depth()
{
  return get_depth();
}

// TODO: implement target yaw, pitch and roll
