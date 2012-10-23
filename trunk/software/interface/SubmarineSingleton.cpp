#include "SubmarineSingleton.h"
#include "../scripts/scripts.h"

/* Constructor and destructor for submarine's FPGA resource */

void SubmarineSingleton::create()
{
  if (instances == 0) {
    return;
  }
  created = true;

  init();
  power_on();
}

void SubmarineSingleton::destroy()
{
  if (!created) {
    return;
  }

  exit_safe();
}
