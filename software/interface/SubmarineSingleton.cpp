#include "CHeader.h"

/* Constructor and destructor for submarine's FPGA resource */

SimulatorSingleton::SimulatorSingleton()
{
  init();
}

SimulatorSingleton::create()
{
  power_on();
}

SimulatorSingleton::destroy()
{
  exit_safe();
}
