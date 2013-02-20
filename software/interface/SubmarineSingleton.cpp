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
}

void SubmarineSingleton::destroy()
{
  if (!created) {
    return;
  }

  exit_safe();
}

void SubmarineSingleton::set_target_yaw(int target_yaw)
{
  this->target_yaw = target_yaw;
  dyn_set_target_yaw(target_yaw);
}

void SubmarineSingleton::set_target_depth(int target_depth)
{
  this->target_depth = target_depth;
  dyn_set_target_depth(target_depth);
}
