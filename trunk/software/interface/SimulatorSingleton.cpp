#include <GL/glut.h>

#include "sim.h"

#include "SimulatorSingleton.h"

// Global variables needed by sim
physical_model model;
unsigned DEBUG_MODEL = 0;
CV_TASK_ENUM cv_task_enum;
unsigned int randNum;

void *run_sim(void *);

/* Constructor and destructor for sim resource */

void SimulatorSingleton::create()
{
  if (instances == 0) {
    return;
  }
  created = true;

  // Start simulation (may need to use passed in data)
  pthread_create(&sim_thread, NULL, run_sim, NULL);
}

void SimulatorSingleton::destroy()
{
  if (!created) {
    return;
  }

  pthread_join(sim_thread, NULL);
}

/* Accessors */

physical_model& SimulatorSingleton::attitude()
{
  return model;
}

/* pthread function */

void *run_sim(void *args)
{
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
  return NULL;
}
