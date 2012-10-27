#include <GL/glut.h>

#include <unistd.h>

#include "sim.h"

#include "SimulatorSingleton.h"

// Global variables needed by sim
volatile physical_model model;
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

  int p[2];
  if (pipe(p)) {
    puts("pipe failed");
    exit(1);
  }

  rh = p[0];
  wh = p[1];
  read = fdopen(rh, "r");
  write = fdopen(wh, "w");

  // Start simulation (may need to use passed in data)
  pthread_create(&sim_thread, NULL, run_sim, NULL);
}

void SimulatorSingleton::destroy()
{
  if (!created) {
    return;
  }

  ::destroy();
}

/* Accessors */

physical_model SimulatorSingleton::attitude()
// Returns a copy of the model.
//
// Since model is volatile, it must be copied (memcpy is safe to cast away the volatile pointer)
{
  physical_model m;
  memcpy(&m, (char *)&model, sizeof(physical_model));
  return m;
}


/* Mutators */

void SimulatorSingleton::add_position(world_vector p)
{
  model.position.x += p.x;
  model.position.y += p.y;
  model.position.z += p.z;
}

void add_orientation(orientation a)
{
  model.angle.yaw += a.yaw;
  model.angle.pitch += a.pitch;
  model.angle.roll += a.roll;
}

/* pthread function */

void sim_keyboard(unsigned char, int, int);

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
  glutKeyboardFunc (sim_keyboard);

  init_sim();
  cv_init(); 

  glutMainLoop();
  return NULL;
}

void sim_keyboard(unsigned char key, int x, int y)
{
  FILE *write_fp = SimulatorSingleton::get_instance().write_fp();
  putchar(key);
  fputc(key, write_fp);
  fflush(write_fp);
}
