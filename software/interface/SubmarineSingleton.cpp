#include "SubmarineSingleton.h"
#include "../scripts/scripts.h"

/* Constructor and destructor for submarine's FPGA resource */

void SubmarineSingleton::create()
{
  if (!registered) {
    return;
  }
  created = true;

  init_fpga();
  power_on();
}

void SubmarineSingleton::destroy()
{
  if (!created) {
    return;
  }

  exit_safe();
}
