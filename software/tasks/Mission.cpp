#include "Mission.h"
#include "mda_tasks.h"

Mission::~Mission()
{
}

// Turn controllers on once the submarine is in the water using a depth threshold
bool Mission::startup()
{
  static const int STARTUP_DEPTH_THRESHOLD = 10;

  actuator_output->special_cmd(SUB_POWER_ON);

  while (attitude_input->depth() < STARTUP_DEPTH_THRESHOLD) {
    if (CharacterStreamSingleton::get_instance().wait_key(1) == 'q') {
      return false;
    }
  }

  actuator_output->special_cmd(SUB_STARTUP_SEQUENCE);

  // clear webcam cache
  for (int i = 0; i < WEBCAM_CACHE; i++) {
    image_input->ready_image();
    image_input->ready_image(DWN_IMG);
  }

  // startup successful
  return true;
}

void Mission::work()
{
  // Tasks
  MDA_TASK_GATE    gate(attitude_input, image_input, actuator_output);
  MDA_TASK_BUOY    buoy(attitude_input, image_input, actuator_output);
//MDA_TASK_FRAME   frame(attitude_input, image_input, actuator_output);
  MDA_TASK_PATH    path(attitude_input, image_input, actuator_output);
  MDA_TASK_SURFACE surface(attitude_input, image_input, actuator_output);

  // List of tasks to be performed in order (NULL-terminated)
  MDA_TASK_BASE *tasks[] = {
    &gate,
    &path,
    &buoy,
    &path,
    &surface,
    NULL};

  // Pointer to current task
  MDA_TASK_BASE **task_ptr = tasks;

  // Result of a task
  MDA_TASK_RETURN_CODE ret_code;

  if (!startup()) {
    return;
  }

  // Run each task until the list of tasks is complete
  while (*task_ptr) {
    ret_code = (*task_ptr)->run_task();
    if (ret_code == TASK_QUIT) {
      surface.run_task();
      break;
    }
    task_ptr++;
  }
}
