#include <cv.h>
#include <highgui.h>

#include "mda_tasks.h"

void MDA_TASK_BASE::move(ATTITUDE_CHANGE_DIRECTION direction, int delta_accel)
{
  actuator_output->set_attitude_change(direction, delta_accel);  
  ATTITUDE_DIRECTION dir;
  switch (direction) {
    case LEFT:
    case RIGHT:
      dir = YAW;
      break;
    case RISE:
    case SINK:
      dir = DEPTH;
      break;
    default:
      dir = SPEED;
      break;
  }

  stabilize(dir);
}

void MDA_TASK_BASE::set(ATTITUDE_DIRECTION dir, int val)
{
  actuator_output->set_attitude_absolute(dir, val);  
  stabilize(dir);
}

void MDA_TASK_BASE::stop()
{
  actuator_output->stop();
}

void MDA_TASK_BASE::stabilize(ATTITUDE_DIRECTION dir)
{
  fflush(stdout);

  if (dir == SPEED) {
    return; // No need to stabilize
  }

  const int yaw_threshold = 3, depth_threshold = 15;
  double max_elapsed_seconds = 10.;
  time_t start_time = time(NULL);
  while (1) {
    image_input->ready_image(FWD_IMG);
    image_input->ready_image(DWN_IMG);

    char c = cvWaitKey(TASK_WK);
    if (c != -1) {
      CharacterStreamSingleton::get_instance().write_char(c);
    }
    if (c == 'q') {
      return;
    }

    // Exit in max_elapsed_seconds to prevent hanging
    if (difftime(time(NULL), start_time) > max_elapsed_seconds) {
      break;
    }

    if (dir == YAW) {
      int current_yaw = attitude_input->yaw();
      int target_yaw = actuator_output->get_target_attitude(YAW);
      if (abs(current_yaw - target_yaw) <= yaw_threshold ||
          abs(current_yaw - target_yaw) >= 360 - yaw_threshold) {
        break;
      }
    } else if (dir == DEPTH) {
      int current_depth = attitude_input->depth();
      int target_depth = actuator_output->get_target_attitude(DEPTH);
      if (abs(current_depth - target_depth) <= depth_threshold) {
        break;
      }
    }
  }
}
