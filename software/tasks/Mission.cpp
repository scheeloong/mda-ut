#include "Mission.h"
#include "mda_tasks.h"

Mission::~Mission()
{
}

void Mission::work()
{
  // Tasks
  MDA_TASK_GATE gate(attitude_input, image_input, actuator_output);
  MDA_TASK_PATH path(attitude_input, image_input, actuator_output);

  // List of tasks to be performed in order (NULL-terminated)
  MDA_TASK_BASE *tasks[] = {&gate, &path, NULL};

  // Pointer to current task
  MDA_TASK_BASE **task_ptr = tasks;

  // Run each task until the list of tasks is complete
  while (*task_ptr) {
    (*task_ptr)->run_task();
    task_ptr++;
  }
}
