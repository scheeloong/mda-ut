/**
* @file sim/main.cpp
*
* @brief This is the main file for the both simulators
*
* This is the main file for simulator, calls init() from site.cpp
*/

#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "bmp_io.h"
#include "types.h"
#include "physical_model.h"
#include "sim.h"
#include "sim_cv.h"

#include <highgui.h>

#include "../motors_new/motors.h"

unsigned int randNum;

GLuint texName[10];

/* physical model*/
physical_model model;
Motors m(&model);

/**
* @brief Main loop for sim
*
* Open window with initial window size, title bar, color index display mode, and handle input events
*/
int main(int argc, char** argv)
{   
    randNum = 0;
    
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-r")) {
            srand (time(NULL));
            randNum = rand() % 15000;
        }
    }
   
   printf(
    "*** Possible Options:\n" 
    "  -r: randomize site\n"
    "*** Commands: \n"
         "  ijkl to strafe in yz plane\n"
         "  wasd to move fowards/back and turn on vertical axis\n"
         "  r and f to roll\n"
         "  t and g to pitch\n"
         "  x to reset angle, z to reset position\n"
         "  <> to set reverse/forward speed\n"
         "  -+ to set down/up depth speed\n"
         "  p to take a screenshot saved as cvSimImg.jpg\n"
         "  (and other undocumented features)\n"
         "  TO CHANGE inital position, see init.h\n"
         "-----------------------------------------\n");
      
   /*glut inits*/
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
   glutInitWindowSize (WINDOW_SIZE_X, WINDOW_SIZE_Y);
   glutInitWindowPosition(10, 0);
   glutCreateWindow ("Forwards Cam");
   init();
      
   cv_init();

   /** register callback functions for glut */
   glutReshapeFunc  (cv_reshape);                    // called when window resized
   glutKeyboardFunc (cv_keyboard);                  // called with key pressed
   glutDisplayFunc  (cv_display);                    // called when glutPostRedisplay() raises redraw flag
   glutIdleFunc     (anim_scene);                        // called when idle (simulate speed)
   
   cv_reshape (600, 400); 
   /*start the main glut loop*/
   glutMainLoop();
   
   destroy();
   return 0;
}
