#include "AttitudeInput.h"
#include "../scripts/scripts.h"

// The caller must construct and destruct the FPGA resource!
// This is because there may be multiple interfaces to the FPGA, but altogether,
// they can only be constructed and destructed once.

// TODO: implement yaw, pitch and roll

// On the order of cm
int AttitudeInputSubmarine::depth()
{
  return get_depth();
}

// TODO: implement target yaw, pitch and roll
