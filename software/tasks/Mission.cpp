#include "Mission.h"
#include "mda_tasks.h"

Mission::~Mission()
{
}

void Mission::work()
{
  // TODO: implement
  SingleTaskMission task_mission = SingleTaskMission(attitude_input, image_input, actuator_output, TASK_TEST);
  task_mission.work();
}


// ###################################################################################
// #### SingleTaskMission Methods
// ###################################################################################
SingleTaskMission:: SingleTaskMission(AttitudeInput *a, ImageInput *i, ActuatorOutput *o, MDA_TASK task_enum) :
  Operation(a, i, o)
{
  switch (task_enum) {
    case TASK_TEST:
      task = new MDA_TASK_TEST (attitude_input, image_input, actuator_output);
      break;
    case TASK_GATE:
    case TASK_PATH:
    case TASK_BUOY:
    case TASK_FRAME:
    default:
      printf ("You have selected a task that is not implemented yet.\n");
      exit (1);
  }
}

SingleTaskMission:: ~SingleTaskMission()
{
    delete task;
}

void SingleTaskMission::work()
{
  MDA_TASK_RETURN_CODE return_code = task->run_task();
  (void) return_code;
}
