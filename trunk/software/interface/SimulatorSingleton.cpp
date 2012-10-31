#include <GL/glut.h>

#include <unistd.h>

#include "sim.h"

#include "SimulatorSingleton.h"

// Global variables needed by sim
volatile physical_model model;
unsigned DEBUG_MODEL = 0;
CV_TASK_ENUM cv_task_enum;
unsigned int randNum;

// Function declarations
void *run_sim(void *);
void sim_init();
void sim_keyboard(unsigned char, int, int);
void sim_display();
void sim_reshape(int, int);

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

const IplImage* SimulatorSingleton::get_image(ImageDirection dir)
{
  img_dir = dir;
  img_copy_start = true;
  img_copy_done = false;
  glutPostRedisplay();

  // Wait until transfer complete
  while(!img_copy_done) {
    ;
  }

  return (dir == FWD_IMG) ? img_fwd : img_dwn;
}

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

void SimulatorSingleton::add_acceleration(float accel, float angular_accel, float depth_accel)
{
  model.accel += accel;
  model.angular_accel += angular_accel;
  model.depth_accel += depth_accel;
}

void *run_sim(void *args)
{
  int argc = 0;
  char *argv[1];
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize (WINDOW_SIZE_X, WINDOW_SIZE_Y);
  glutInitWindowPosition(10, 0);
  glutCreateWindow ("Forwards Cam");

  glutReshapeFunc  (sim_reshape);
  glutDisplayFunc  (sim_display);
  glutIdleFunc     (anim_scene);
  glutKeyboardFunc (sim_keyboard);

  init_sim();
  sim_init(); 

  glutMainLoop();
  return NULL;
}

void sim_init()
{
  SimulatorSingleton::get_instance().sim_init();
}

void SimulatorSingleton::sim_init()
{
  unsigned width = 600, height = 400; // temporary

  img_fwd = cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 3);
  img_fwd->origin = 1;
  img_dwn = cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 3);
  img_dwn->origin = 1;
    
  cvNamedWindow("Downwards Cam", 1);
  cvMoveWindow ("Downwards Cam", 380, 0);
}

void sim_keyboard(unsigned char key, int x, int y)
{
  SimulatorSingleton::get_instance().sim_keyboard(key);
}

void SimulatorSingleton::sim_keyboard(unsigned char key)
{
  putchar(key);
  fputc(key, write);
  fflush(write);
}

void sim_display()
{
  SimulatorSingleton::get_instance().sim_display();
}

void SimulatorSingleton::sim_display()
{
  if (img_copy_start) {
    img_copy_start = false;
    if (img_dir == DWN_IMG) {
      model.angle.pitch += 90; // grab downwards
    }
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    set_camera();
    draw();
    if (img_dir == FWD_IMG) {
      cvQueryFrameGL (img_fwd);
    } else {
      cvQueryFrameGL (img_dwn);
    }
    if (img_dir == DWN_IMG) {
      model.angle.pitch -= 90; // reset pitch
    }
    img_copy_done = true;
  } else {
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    set_camera();
    draw(); // front camera
    glutSwapBuffers();   
  }

  glFlush();
}

void sim_reshape(int w, int h)
{
  SimulatorSingleton::get_instance().sim_reshape(w, h);
}

void SimulatorSingleton::sim_reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(CAMERA_FIELD_OF_VIEW, (GLfloat)w/(GLfloat)h, .05, 200.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
   
  // Also resize the Ipl images
  cvReleaseImage (&img_fwd);
  img_fwd = cvCreateImage (cvSize(w,h), IPL_DEPTH_8U, 3);
  img_fwd->origin = 1;

  cvReleaseImage (&img_dwn);
  img_dwn = cvCreateImage (cvSize(w,h), IPL_DEPTH_8U, 3);
  img_dwn->origin = 1;
}
