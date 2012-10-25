#include <GL/glut.h>

#include <signal.h>
#include <unistd.h>

#include "motors.h"
#include "sim.h"

#include "SimulatorSingleton.h"

// Global variables needed by sim
physical_model model;
Motors m = Motors(&model);
unsigned DEBUG_MODEL = 0;
CV_TASK_ENUM cv_task_enum;
unsigned int randNum;

/* Constructor and destructor for sim resource */

void SimulatorSingleton::create()
{
  if (instances == 0) {
    return;
  }
  created = true;

  // Start simulation (may need to use passed in data)
  child_pid = fork();
  if (fork() == 0) {
    init_sim();
    cv_init(); 

    glutMainLoop();
  }
}

void SimulatorSingleton::destroy()
{
  if (!created) {
    return;
  }

  // Clean up resources
  kill(child_pid, SIGTERM);
}

physical_model& SimulatorSingleton::attitude()
{
  return model;
}
