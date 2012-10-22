#include "CHeader.h"

/* Constructor and destructor for submarine's FPGA resource */

SimulatorSingleton::create()
{
  if (instances == 0) {
    return;
  }
  created = true;

  init();
  power_on();
}

SimulatorSingleton::destroy()
{
  if (!created) {
    return;
  }

  exit_safe();
}
