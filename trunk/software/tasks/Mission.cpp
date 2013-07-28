#include "Mission.h"
#include "mda_tasks.h"

Mission::~Mission()
{
}

// Turn controllers on once the submarine is in the water using a depth threshold
bool Mission::startup()
{
  actuator_output->special_cmd(SUB_MISSION_STARTUP_SEQUENCE);
  
  if (CharacterStreamSingleton::get_instance().wait_key(1) == 'q') {
    return false;
  }

  // startup successful
  return true;
}

void Mission::work_internal(bool show_image)
{
  mvWindow::setShowImage(show_image);
  
  MDA_TASK_BASE::starting_depth = 260;//attitude_input->depth();
  if (!startup()) {
    return;
  }

  // Tasks
  //MDA_TASK_GATE       gate(attitude_input, image_input, actuator_output);
  MDA_TASK_BUOY       buoy(attitude_input, image_input, actuator_output);
  MDA_TASK_FRAME      frame(attitude_input, image_input, actuator_output);
  MDA_TASK_PATH       path(attitude_input, image_input, actuator_output);
  //MDA_TASK_PATH_SKIP  path_skip(attitude_input, image_input, actuator_output);
  MDA_TASK_SURFACE    surface(attitude_input, image_input, actuator_output);
  MDA_TASK_RESET      reset(attitude_input, image_input, actuator_output);
  // List of tasks to be performed in order (NULL-terminated)
  /*MDA_TASK_BASE *tasks[] = {
    //&gate,
    &path,
    //&buoy,
    &path,
    //&path_skip,
    //&path,
    &frame,
    &surface,
    NULL};

  // Pointer to current task
  MDA_TASK_BASE **task_ptr = tasks;
  */
  // Result of a task
  MDA_TASK_RETURN_CODE ret_code;

  printf ("Running a %s mission!\n", show_image?"test":"Real");
  int RUN_BUOY;
  read_mv_setting ("hacks.csv", "RUN_BUOY", RUN_BUOY);  
  int RUN_FRAME;
  read_mv_setting ("hacks.csv", "RUN_FRAME", RUN_FRAME);  


  ret_code = path.run_task();
  if (RUN_BUOY) ret_code = buoy.run_task();
  ret_code = path.run_task();
  if (RUN_FRAME) ret_code = frame.run_task();
  ret_code = surface.run_task();

  /*int task_index = 0;

  // Run each task until the list of tasks is complete
  while (*task_ptr) {
    int starting_yaw = attitude_input->yaw();
    int starting_depth = attitude_input->depth();

    ret_code = (*task_ptr)->run_task();

    if (ret_code == TASK_REDO) {
      // reset position, go back to orig depth, orig yaw, then retreat for 2 seconds
      reset.run_task(starting_depth, starting_yaw, -2);
      // rerun task
      ret_code = (*task_ptr)->run_task();  
    }
    if (ret_code == TASK_QUIT) {
      surface.run_task();
      break;
    }
    
    task_ptr++;
    task_index++;
  }*/
}
