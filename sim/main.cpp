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
#include "keyboard.h"

#include <cv.h>
#include <highgui.h>
#include "../vision/common.h"
#include "../vision/task_gate.h"
#include "../vision/task_path.h"
#include "../vision/task_U.h"

#include "../mission/mission.h"

unsigned int randNum;
struct timeval last;

/* store of texture references*/
GLuint texName[8];
/* physical model*/
physical_model model;
orientation &angle = model.angle;
world_vector &position = model.position;
Mission m(&model);
KeyboardInput keyboard(m);

/* window size */
int window_width, window_height;        // only updates when window resized by user

/**
* @brief makes texture from given file
*
* @param filename
* File name
* @param tex_name
* Texture name, which is a number
*/

void makeTextureImage(char filename[], GLuint tex_name) {
   unsigned long width=0;
   long height=0;	/* width/height of textures */
   GLubyte *image=NULL;	/* storage for texture */

   if (bmp_read (filename, &width, &height, image))
      assert(false);

   /* create texture object */
   glBindTexture(GL_TEXTURE_2D, texName[tex_name]);

   /* set up wrap & filter params */
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   /*  printf("Texture %s, height %d, width %d\n",
       filename, (int)height, (int)width); */

   /* load the image */
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                0, GL_RGB, GL_UNSIGNED_BYTE, image);
}

#define CAMERA_FIELD_OF_VIEW 37

/** sky's color*/
const float sky[4] = { .527343, .804687, 5/*1*/, 1.0f};

/**
* @brief Read textures and define lighting
*
* @see init_site
*/

void init() {
   glEnable (GL_LINE_SMOOTH);
   glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
   glLineWidth (1.5);

   gettimeofday(&last, NULL);
   model.reset_pos();
   model.reset_angle();
   angle.pitch = 0;

   /* set background colour */
   glClearColor(sky[0], sky[1], sky[2], sky[3]);

   /* generate texture names */
   texName[0] = 0;
   texName[1] = 1;
   texName[2] = 2;
   texName[3] = 3;
   texName[4] = 4;
   texName[5] = 5;
   texName[6] = 6;
   texName[7] = 7;
   glGenTextures(8, texName);

   /* load textures */
    //#define TNAME "img/green_resized.bmp"
    #define TNAME "img/floor2.bmp"
    #define SNAME "img/sky.bmp"
    #define SSNAME "img/surfsky.bmp"
    #define JVNAME "img/jamesvelcro.bmp"
    #define BINNAME1 "img/ship.bmp"
    #define BINNAME2 "img/tank.bmp"
    #define BINNAME3 "img/plane.bmp"
    #define BINNAME4 "img/factory.bmp"

   makeTextureImage((char *)TNAME, 0);
   makeTextureImage((char *)SNAME, 1);
   makeTextureImage((char *)SSNAME, 2);
   makeTextureImage((char *)JVNAME, 3);
   makeTextureImage((char *)BINNAME1, 4);
   makeTextureImage((char *)BINNAME2, 5);
   makeTextureImage((char *)BINNAME3, 6);
   makeTextureImage((char *)BINNAME4, 7);

   /* enable texturing & set texturing function */
   glEnable(GL_TEXTURE_2D);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   //http://www.falloutsoftware.com/tutorials/gl/gl8.htm
   GLfloat mat_specular[]={1.0, 1.0, 1.0, 1.0};
   GLfloat mat_diffuse[]={1.0, 1.0, 1.0, 1.0};
   GLfloat mat_ambient[]={1.0, 1.0, 1.0, 1.0};
   GLfloat mat_shininess={100.0};
   GLfloat light_ambient[]={0.0, 0.0, 0.0, 1.0};
   GLfloat light_diffuse[]={1.0, 1.0, 1.0, 1.0};
   GLfloat light_specular[]={1.0, 1.0, 1.0, 1.0};
   GLfloat light_position[]={0, 0.1, 0.0, 0.0};
   GLfloat cutoff[]={180};
   GLfloat light_direction[]={0, 1, 0.0};

   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   // don't shine on the floor
   glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
   glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,light_direction);

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

   glShadeModel(GL_SMOOTH);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   light_direction[2] = -1;
   glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, cutoff);
   glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,light_direction);
   light_position[0]=5;
   light_position[1]=2;
   light_position[2]=-5;

   GLfloat light1_ambient[]={1.0, 1.0, 1.0, 1.0};
   GLfloat light1_diffuse[]={1.0, .65, .0, 1.0};
   GLfloat light1_specular[]={1.0, .65, .0, 1.0};

   glLightfv(GL_LIGHT1, GL_POSITION, light_position);
   glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glEnable(GL_LIGHT1);

   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   glEnable (GL_COLOR_MATERIAL);
   glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

   init_fog();
   init_site();
}

//http://www.bme.jhu.edu/~reza/book/kinematics/kinematics.htm
// x is pointing right
// y is pointing up
// z is pointing towards the programmer
int verbose_camera = true;
int camera_display_override = 0;

void set_camera() {
   /* rotate scene */
   quat_camera(angle.roll, angle.pitch, angle.yaw);

   /* allow for translate scene */
   glTranslatef(-position.x, -position.y, -position.z);
   if (verbose_camera && camera_display_override)
   {
      printf ("%d\n", verbose_camera);
      printf("position %f %f %f\n",
             position.x, position.y, position.z);
      printf("angle p(y) %f y(z) %f r(x) %f\n",
             angle.pitch, angle.yaw, angle.roll);
   }
}

void range_angle(float& angle)
{
   if (angle >= 180)
      angle -= 360;
   else if (angle <= -180)
      angle += 360;
}

// max refresh rate
#define DELAY 500
void anim_scene() 
{
   struct timeval cur;
   gettimeofday(&cur, NULL);
   
   long delta = (long)(cur.tv_sec*1e6+cur.tv_usec - (last.tv_sec*1e6+last.tv_usec));
   if (delta > DELAY)
   {
      last = cur;

      delta = (int)DELAY;
      model.update(delta);
      glutPostRedisplay();
   }
}

/**
* @brief Take a screenshot from current angle and save as bitmap
*/
void screenshot()
{
   int n = 3*window_width*window_height;
   GLubyte *pixels = new GLubyte[n];

   glReadPixels(0,0,window_width,window_height,
                GL_RGB,GL_UNSIGNED_BYTE,pixels);

   if (bmp_24_write ((char *)"test.bmp", window_width, window_height, pixels))
      assert(false);
   delete []pixels;
}

/** OpenCV integration section starts here.
 *  Info:
 *  The glut model of code goes revolves around the function glutMainLoop(). A
 *  glut program generally has 2 steps: 
 *      1. Init the main loop with parameters. 
 *      2. Register functions that will be called by mainloop when keyboard is hit,
 *      screen needs to be redraw, etc
 *      3. Call glutMainLoop. Once this is done the loop cannot be exited.
 * 
 *  For our purposes theres only 2 functions you need to care about.
 *      cv_display - function that is called whenever sim needs to update the
 *          window. redraws the window.
 *      cv_keyboard - function that is called whenever key is hit. It processes
 *          the keyhit, updates the model, and raises a flag to redraw window.
 *  Thus everytime u hit key the 2 above functions get called. Idle redraw has been
 *      disabled for now.
 * 
 * The openCV integrated model will expand this: instead of 
 *      key hit -> glut makes decision using key -> update parameters -> redraw window
 * We have: 
 *      Any key hit -> send current img to opencv -> cv decides on a key using img 
 *      -> sends keystroke command to glut -> glut makes decision using key 
 *      -> update parameters -> redraw window
 * 
 * The Any key hit refers to the user hitting any key. Its just to "go to next step" of sim.
 * User has no input when cv is running. cv will overwrite the user's key.
 * 
 * This is implemented by adding a section in cv_display that sends img to cv and processes
 * it. Thus cv will see a new image immediately as it is painted on window.
 * 
 * cv processes the image and updates a global var CV_COMMAND
 * 
 * Then cv_keyboard reads CV_COMMAND and does its normal thing. 
 * 
 * To start the cycle each time user must press key, this will call cv_keyboard to execute 
 * the current CV_COMMAND, then flag window as needing to be redraw. This causes cv to update 
 * CV_COMMAND. Thus you will see on terminal what CV wants to do, and press a key to see the
 * action occur.
 * 
 */

#define WIN0 "window0"
#define WIN1 "window1"
#define WIN2 "window2"

/** all the global variables used by opencv is here */

char* cv_windows[3]; // string names for 3 windows cv can use
int CV_VISION_FLAG=0;  // 1 to show opencv windows
//int CV_CONTROL_ON=0; // 1 to have cv issue commands
IplImage *cv_img, *cv_img2; // stores the front and down camera imgs respectively

vision_in Vin, Vin2;
vision_out Vout;

/** end opencv global vars */

void cv_init () {
       cv_img = cvCreateImage (cvSize(WINDOW_SIZE_X,WINDOW_SIZE_Y), IPL_DEPTH_8U, 3);
       cv_img->origin = 1;
       cv_img2 = cvCreateImage (cvSize(WINDOW_SIZE_X,WINDOW_SIZE_Y), IPL_DEPTH_8U, 3);
       cv_img2->origin = 1;
       
       switch (CV_VISION_FLAG) {
           case '1':
           case '2':
               Vin.HSV.setSim1();
               Vin2.HSV.setSim2();
               break;
           case '3':
               Vin.HSV.setSim1();
               Vin2.HSV.setSim2();
               break;
           default:
               printf ("Unrecognized CV_VISION_FLAG. Shutting Down\n");
               exit(1);
               break;
       }
       cvNamedWindow("Downwards Cam",1);
       cvMoveWindow ("Downwards Cam", 380,0);
       create_windows ();
}

void cv_queryFrameGL (IplImage* img) {
// this function reads the OpenGL buffer and puts the data into an iplimage.
// Plz allocate the image before calling
// MAKE SURE the ORIGIN parameter in img is set to 1 !!!
    // read the GL buffer. This is a lot easier than that guy on the interwebs said
    glReadPixels(0,0,img->width-1,img->height-1,GL_BGR,GL_UNSIGNED_BYTE,img->imageData);
    // srsly the guy on the interwebs had like 20 lines for this
}

void cv_display (void) {
// this function is called by glutMainLoop every time the window needs to be redrawn 
// the redraw flag is raised by the function glutPostRedisplay();
// with CV_VISION_FLAG the cv code will run every time the window is updated.
   
   if (CV_VISION_FLAG) {
       // first grab both front and bottom cam images       
       angle.pitch = 90;      // grab downwards first
       glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
       glLoadIdentity();
       set_camera();
       draw();
       cv_queryFrameGL (cv_img2);
       
       glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
       angle.pitch = 0;
       glLoadIdentity();
       set_camera();
       draw();
       glutSwapBuffers();
       cv_queryFrameGL (cv_img);       

       Vin.img = cv_img;
       Vin2.img = cv_img2;
              
       cvShowImage ("Downwards Cam", Vin2.img);//cv_img2);
       
       /** OPENCV CODE GOES HERE. */      
       //cvShowImage (WIN0,cv_img);
       switch (CV_VISION_FLAG) {
            case '1':
                controller_GATE (Vin, m);
                break;
            case '2':
                controller_PATH (Vin2, m);
                break;
            case '3':
                controller_U (Vin, m);
                break;
       }
  
       cvWaitKey(5); // without 5ms delay the window will not show properly
       /** END OPENCV CODE */ 
   }
   else {
       glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
       glLoadIdentity();
       set_camera();
       draw();              // front camera
       glutSwapBuffers();   // we are using double buffering. Front buffer is displayed. you draw 
                        // on back buffer, which isnt displayed. This is so theres time to
                        // draw everything without flicker.
                        // when done drawing you swap buffers. Front and back swap places.
   }
   
   glFlush();
   //copy_bytes();
}

void cv_keyboard(unsigned char key, int x, int y)
{
   if (key == 'q' || key == 27) { // q or escape
      glDeleteTextures( 5, texName );
      exit(0);
   }
      
   //if (CV_CONTROL_ON) key = CV_COMMAND;  // use opencv command 

   switch (key)
   {
   case 'j': // strafe in yz plane is ijkl
      position.z -=  POS_INC*sin((angle.yaw*PI)/180);
      position.x -=  POS_INC*cos((angle.yaw*PI)/180);
      break;
   case 'l':
      position.z +=  POS_INC*sin((angle.yaw*PI)/180);
      position.x +=  POS_INC*cos((angle.yaw*PI)/180);
      break;
   case 'i':      
      position.y +=  POS_INC;
      break;
   case 'k':
      position.y -=  POS_INC;
      break;
   case 'f': // roll is rf
      angle.roll -= ANGLE_INC/4;
      break;
   case 'r':
      angle.roll += ANGLE_INC/4;
      break;
   case 't': // pitch is gt
      angle.pitch -= ANGLE_INC;
      break;
   case 'g':
      angle.pitch += ANGLE_INC;
      break;
   case 'x': // reset angle
      model.reset_angle();
      break;
   case 'z': // reset position
      model.reset_pos();
      break;
   /*case ' ': // spacebar switches to downwards cam
       if (CAM_ANGLE == 0) CAM_ANGLE = 90;
       else CAM_ANGLE = 0;
       angle.pitch = CAM_ANGLE;
       break;*/
   /*case 'v':
       if (CV_CONTROL_ON) { printf ("Vision Control OFF\n");
                            CV_CONTROL_ON=0; } 
       else { printf ("Vision Control ON\n");
              CV_CONTROL_ON=1; }*/
   case 'p':
   {
      //screenshot();
      cvSaveImage ("cvSimImg.jpg", cv_img);
      cvSaveImage ("cvSimImg2.jpg", cv_img2);
      break;
   }
   default:
      keyboard.read_input(key);
      break;
   }

   range_angle(angle.yaw);
   range_angle(angle.roll);
   range_angle(angle.pitch);

   glutPostRedisplay();
}

void cv_reshape(int w, int h)
// have to change opencv struct size when changing windowz
{
   window_width = w;
   window_height = h;

   printf("reshape called\n");
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(CAMERA_FIELD_OF_VIEW, (GLfloat)w/(GLfloat)h, .05, 200.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   // resize the opencv structure too eh
   if (CV_VISION_FLAG) {
       cvReleaseImage (&cv_img);
       cv_img = cvCreateImage (cvSize(window_width,window_height), IPL_DEPTH_8U, 3);
       cv_img->origin=1;
   }
}

/**
* @brief Main loop for sim
*
* Open window with initial window size, title bar, color index display mode, and handle input events
*/
int main(int argc, char** argv)
{
   if ((argc > 1) && (argv[1][0] >= '1') && (argv[1][0] <= '9'))
      CV_VISION_FLAG = argv[1][0];
   if (argc > 2)
   {
      sscanf(argv[2], "%u", &randNum);
      printf("Random number: %d\n", randNum);
   }
   else
      randNum = 0;

   printf(
         "*** USE ./sim v TO USE VISION CODE\n"
         "*** Commands: \n"
         "*** ijkl to strafe in yz plane\n"
         "*** wasd to move fowards/back and turn on vertical axis\n"
         "*** r and f to roll\n"
         "*** t and g to pitch\n"
         "*** x to reset angle, z to reset position\n"
         "*** o to save current window as jpg (VISION MODE ONLY)\n"
         "*** 0-9 to set speed\n"
         "*** . to stop (set speed to 0)\n"
         "*** <> to set reverse/forward speed\n"
         "*** -+ to set down/up depth speed\n"
         "*** [] to set negative/positive angular speed\n"
         "*** p to take a screenshot saved as test.bmp\n"
         "*** (and other undocumented features)\n"
         "*** TO CHANGE inital position, see init.h\n"
         "-----------------------------------------\n");
      
   /*glut inits*/
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
   glutInitWindowSize (WINDOW_SIZE_X, WINDOW_SIZE_Y);
   glutInitWindowPosition(10, 0);
   glutCreateWindow ("Forwards Cam");
   init();
      
   if (CV_VISION_FLAG) {
       cv_init();
   }
   
   /** register callback functions for glut */
   glutReshapeFunc (cv_reshape);                    // called when window resized
   glutKeyboardFunc (cv_keyboard);                  // called with key pressed
   glutDisplayFunc (cv_display);                    // called when glutPostRedisplay() raises redraw flag
   glutIdleFunc(anim_scene);                        // called when idle (simulate speed)
   
   /*start the main glut loop*/
   glutMainLoop();
   
   destroy_windows ();
   cvDestroyWindow ("Downwards Cam");
   if (CV_VISION_FLAG) {
       cvReleaseImage (&cv_img);
       cvReleaseImage (&cv_img2);
   }
   destroy();
   return 0;
}
