#include <algorithm>

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

  // disable verbosity from FPGA UI
  set_verbose(0);
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
  if (target_yaw >= 180) {
    target_yaw -= 360;
  } else if (target_yaw < -180) {
    target_yaw += 360;
  }

  this->target_yaw = target_yaw;
  dyn_set_target_yaw(target_yaw);
}

void SubmarineSingleton::set_target_depth(int target_depth)
{
#if 0
  const int upper_depth_staircase_limit = 200;
  const int staircase_depth = 50;
  const int stable_threshold = 15;

  while (this->target_depth != target_depth) {
    // The PID controller is slow to converge at low depths, so skip the staircase movement
    if (this->target_depth <= upper_depth_staircase_limit && target_depth <= upper_depth_staircase_limit) {
      this->target_depth = target_depth;
      dyn_set_target_depth(target_depth);
    } else {
      int delta_target_depth = std::min(abs(this->target_depth - target_depth), staircase_depth);
      int intermediate_depth = this->target_depth + (target_depth > this->target_depth) ? delta_target_depth : -delta_target_depth;
      this->target_depth = intermediate_depth;
      dyn_set_target_depth(intermediate_depth);
      if (this->target_depth != target_depth) {
        // let the submarine stabilize
        int count = 0;
        while (abs(this->target_depth - get_depth()) > stable_threshold && count < 15) {
          count++;
        }
      }
    }
  }
#else
  this->target_depth = target_depth;
  dyn_set_target_depth(target_depth);
#endif
}
