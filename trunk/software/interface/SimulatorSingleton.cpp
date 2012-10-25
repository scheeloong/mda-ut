#include <GL/glut.h>

#include <stdio.h>
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
  if (child_pid == 0) {
    int argc = 0;
    char *argv[1];
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize (WINDOW_SIZE_X, WINDOW_SIZE_Y);
    glutInitWindowPosition(10, 0);
    glutCreateWindow ("Forwards Cam");

    glutReshapeFunc  (cv_reshape);
    glutDisplayFunc  (cv_display);
    glutIdleFunc     (anim_scene);

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

  printf("Press enter to quit");
  getchar();
  putchar('\n');

  // Clean up resources
  kill(child_pid, SIGTERM);
}

physical_model& SimulatorSingleton::attitude()
{
  return model;
}
