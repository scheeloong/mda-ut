/**
* @file sim/main.cpp
*
* @brief This is the main file for the both simulators
*
* This is the main file for simulator, calls init() from site.cpp
*/

/* code started from the following file
 * aaindex.c (deprecated)
 * This program draws shows how to draw anti-aliased lines in color
 * index mode. It draws two diagonal lines to form an X; when 'r'
 * is typed in the window, the lines are rotated in opposite
 * directions.
 */
// path interpolation with b-splines
// http://www.nar-associates.com/nurbs/c_code.html

#include <GL/glut.h>
#include <stdlib.h>
#include "bmp_io.h"
#include "types.h"
//#include "protocol.h"

//for movement of the sub
//#include <sys/time.h>
//for movement of the sub
//#include <signal.h>

#include <math.h>

#include <cv.h>     // for some reason this has to come after the other libs
#include <highgui.h>
#include "../vision/cv_tasks.h"

#define ANGLE_INC 5.
#define POS_INC .2
#define PI 3.14159265

//char is_interactive;
unsigned int randNum;

//struct timeval last, cur;
//void anim_scene();
//void init_vision(int start_fifo);
//void run_vision();
//void service_fifo_client();

int cur_pos = 0;

/** store of texture references*/
GLuint texName[8];

/* camera location*/
world_vector position;

/* camera orientation*/
orientation angle;

char right_mouse=0, left_mouse=0;

int window_width, window_height;        // only updates when window resized by user

/**
* @brief makes texture from given file
*
* @param filename
* File name
* @param tex_name
* Texture name, which is a number
*/
void makeTextureImage(char filename[], GLuint tex_name)
{
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
void init()
{
   glEnable (GL_LINE_SMOOTH);
   glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
   glLineWidth (1.5);

   reset_pos();
   reset_angle();
   update_model(0);

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

   makeTextureImage(TNAME, 0);
   makeTextureImage(SNAME, 1);
   makeTextureImage(SSNAME, 2);
   makeTextureImage(JVNAME, 3);
   makeTextureImage(BINNAME1, 4);
   makeTextureImage(BINNAME2, 5);
   makeTextureImage(BINNAME3, 6);
   makeTextureImage(BINNAME4, 7);

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
bool verbose_camera = true;

void set_camera()
{
   /* rotate scene */
   quat_camera(angle.roll, angle.pitch, angle.yaw);

   /* allow for translate scene */
   glTranslatef(-position.x, -position.y, -position.z);
//#ifndef LONGV
   if (verbose_camera)
   {
      printf("position %f %f %f\n",
             position.x, position.y, position.z);
      printf("angle p(y) %f y(z) %f r(x) %f\n",
             angle.pitch, angle.yaw, angle.roll);

      // if (!is_interactive)
      //   verbose_camera = false;
   }
}

extern GLubyte *pixels_cpy;
extern int cur_pix_size;
/*
// this function is for server.cpp, doesn't seem to work if done by the server thread 
void copy_bytes()
{
   int n = 3*window_width*window_height;
   //cout << "******nnnnn" << window_width << " " <<window_height << endl;

   if ((n != cur_pix_size) && (pixels_cpy != NULL))
   {
      delete []pixels_cpy;
      delete []CUR_IMAGE_store;
   }

   if (n != cur_pix_size)
   {
      pixels_cpy = new GLubyte[n];
      CUR_IMAGE_store = new GLubyte[n];
      cur_pix_size = n;
   }



   glReadPixels(0,0,window_width,window_height,
                GL_RGB,GL_UNSIGNED_BYTE,pixels_cpy);


   run_vision();

   
    // glReadPixels(0,0,window_width,window_height,
    // GL_RED,GL_UNSIGNED_BYTE,pixels_cpy);
    // glReadPixels(0,0,window_width,window_height,
    // GL_GREEN,GL_UNSIGNED_BYTE,&pixels_cpy[window_width*window_height]);
    // glReadPixels(0,0,window_width,window_height,
    // GL_BLUE,GL_UNSIGNED_BYTE,&pixels_cpy[2*window_width*window_height]); 

}
*/
void range_angle(float& angle)
{
   if (angle >= 180)
      angle -= 360;
   else if (angle <= -180)
      angle += 360;
}

/**
* @brief Manage mouse input
*/
/*
void mouse(int button, int state, int x, int y)
{
   // check for mouse button state changes 
   //if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
  //right_mouse = 1;

   if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
   {
     // toggle mouse state
     right_mouse = 1&(~right_mouse);
   }

   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
      left_mouse = 1;

   if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
   {
      left_mouse = 0;
   }

   // store mouse position 
   printf("pos: %d %d\n", x, y);
}
*//*
void glutTimer(int value)
{
  if ( right_mouse == 1 )
  {
     glutWarpPointer(window_width/2,window_height/2);
  }

   glutTimerFunc(5, glutTimer, 0);    
   //printf("value %d\n", value);
}

void passive_mouse_motion(int x, int y)
{
  if ( right_mouse == 1 )
  {
    int yaw = x - window_width/2;
    int pitch = y - window_height/2;

    angle.yaw += yaw/2;
    angle.pitch += pitch/2;

    //printf("yaw: %d\n",angle.yaw);

  }
   glutPostRedisplay();
}
*/
// max refresh rate
/*commented out by RZ. This is so sim does not update by itself (you have to press key)
#define DELAY 2e5
void anim_scene() 
{
   gettimeofday(&cur, NULL);
   
   long delta = (long)(cur.tv_sec*1e6+cur.tv_usec - (last.tv_sec*1e6+last.tv_usec));
   if (delta > DELAY)
   {
      last = cur;

      delta = (int)DELAY;
      update_model(delta);
      glutPostRedisplay();
   }
   service_fifo_client();
}*/

/**
* @brief Take a screenshot from current angle and save as bitmap
*/
void screenshot()
{
   int n = 3*window_width*window_height;
   GLubyte *pixels = new GLubyte[n];
   char tmp[64];
   char zim[4];

   int tmp_pos = cur_pos;
   for (int t=0, x=100; x!=0; x=x/10)
   {
      zim[t] = tmp_pos/x ;
      tmp_pos -= zim[t] * x;
      zim[t] |= '0';
      // printf("%d %d %d\n", x, t, tmp_pos);
      t++;
   }
   zim[3] = 0;

   sprintf(tmp, "test%s.bmp", zim);
   glReadPixels(0,0,window_width,window_height,
                GL_RGB,GL_UNSIGNED_BYTE,pixels);

   if (bmp_24_write (tmp, window_width, window_height, pixels))
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
int CV_VISION_ON=0;  // 0 if manual control, 1 if auto control
float CAM_ANGLE = 00.;    // set to 90 for down pointing
char CV_COMMAND=0; // stores the latest command sent by the opencv loop
IplImage* cv_img;

/** end opencv global vars */

void cv_queryFrameGL (IplImage* img) {
// this function reads the OpenGL buffer and puts the data into an iplimage.
// Plz create the image before calling
// MAKE SURE the ORIGIN parameter in img is set to 1 !!!

    // read the GL buffer. This is a lot easier than that guy on the interwebs said
    glReadPixels(0,0,img->width-1,img->height-1,GL_BGR,GL_UNSIGNED_BYTE,img->imageData);
    // srsly the guy on the interwebs had like 20 lines for this
}

void cv_display (void) {
// this function is called by glutMainLoop every time the window needs to be redrawn 
// the redraw flag is raised by the function glutPostRedisplay();
// with CV_VISION_ON the cv code will run every time the window is updated.

   glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();
   set_camera();
   draw();          
   glutSwapBuffers();   // we are using double buffering. Front buffer is displayed. you draw 
                        // on back buffer, which isnt displayed. This is so theres time to
                        // draw everything without flicker.
                        // when done drawing you swap buffers. Front and back swap places. 
   
   if (CV_VISION_ON) {
       CV_COMMAND = 0;
       cv_queryFrameGL (cv_img);    // grab the current front buffer
   
       /** OPENCV CODE GOES HERE. RESULT IS CHANGE TO CV_COMMAND */
       int gateX, gateY;
       
       cvShowImage (WIN0,cv_img);
       vision_GATE (cv_img, gateX, gateY,
                    1, cv_windows);

       cvWaitKey(5); // without 5ms delay the window will not show properly
       CV_COMMAND='w';
       /** END OPENCV CODE */ 
   }
   
   glFlush();
   //copy_bytes();
}

void cv_keyboard(unsigned char key, int x, int y)
{
   //float posX, posZ;

   //cos((angle.yaw*PI)/180)
   if (key == 'q' || key == 27) { // q or escape
      glDeleteTextures( 5, texName );
      terminate_server();
      exit(0);
   }
   else if (key == '0' && CV_VISION_ON) // save an image
       cvSaveImage ("cvSimImg.jpg", cv_img);
       
   //if (CV_VISION_ON) key = CV_COMMAND;  // use opencv command 

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
   case 'w': // fowards/back and turn is is wasd
      position.z -=  POS_INC*cos((angle.yaw*PI)/180)*cos(((angle.pitch-CAM_ANGLE)*PI)/180);
      position.x +=  POS_INC*sin((angle.yaw*PI)/180);
      position.y -=  POS_INC*sin(((angle.pitch-CAM_ANGLE)*PI)/180);
      break;
   case 's':
      position.z +=  POS_INC*cos((angle.yaw*PI)/180)*cos(((angle.pitch-CAM_ANGLE)*PI)/180);
      position.x -=  POS_INC*sin((angle.yaw*PI)/180);
      position.y +=  POS_INC*sin(((angle.pitch-CAM_ANGLE)*PI)/180);
      break;
   case 'a':
      angle.yaw -= ANGLE_INC/2;
      break;
   case 'd':
      angle.yaw += ANGLE_INC/2;
      break;
   case 'f': // roll is rf
      angle.roll -= ANGLE_INC/4;
      break;
   case 'r':
      angle.roll += ANGLE_INC/4;
      break;
   case 't': // pitch is rf
      angle.pitch -= ANGLE_INC;
      break;
   case 'g':
      angle.pitch += ANGLE_INC;
      break;
   case 'x': // reset angle
      reset_angle();
      update_angle_from_model();
      break;
   case 'z': // reset position
      reset_pos();
      update_pos_from_model();
      break;
   case ' ': // spacebar switches to downwards cam
       if (CAM_ANGLE == 0) CAM_ANGLE = 90;
       else CAM_ANGLE = 0;
       angle.pitch = CAM_ANGLE;
       break;
   case 'p':
   {
      screenshot();
      break;
   }
   default:
      break;
   }
    /* RZ: I have no idea what this does, i think its velocity?
   position.x += posX*cos((angle.yaw*PI)/180);
   position.z += posZ*cos((angle.pitch*PI)/180);
   //position.y += sin((angle.pitch*PI)/180);
  */ 
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
   if (CV_VISION_ON) {
       cvReleaseImage (&cv_img);
       cv_img = cvCreateImage (cvSize(window_width,window_height), IPL_DEPTH_8U, 3);
       cv_img->origin=1;
   }
}


// why is this included here? RZ: I have no idea either!
/*
#if LIN_SERV
#include <signal.h>
#endif
*/
/**
* @brief Main loop for sim
*
* Open window with initial window size, title bar, color index display mode, and handle input events
*/
int main(int argc, char** argv)
{
   if ((argc > 1) && (argv[1][0] == 'v'))
      CV_VISION_ON = 1;
   if (argc > 2)
   {
      randNum = atoi(argv[2]);
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
         "*** t and g to roll\n"
         "*** x to reset angle, z to reset position\n"
         "*** 0 to save current window as jpg (VISION MODE ONLY)\n"
         "*** SPACEBAR to switch to downwards cam\n"
         "*** + other undocumented features\n"
         "*** TO CHANGE inital position, see init.h\n"
         "-----------------------------------------\n");
      
   /*glut inits*/
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH |GLUT_RGB | GLUT_DOUBLE);
   glutInitWindowSize (WINDOW_SIZE_X, WINDOW_SIZE_Y);
   glutInitWindowPosition(100, 100);
   glutCreateWindow (argv[0]);
   init();
   angle.pitch = CAM_ANGLE;
      
   if (CV_VISION_ON) {
       // OPENCV INIT
       cv_img = cvCreateImage (cvSize(WINDOW_SIZE_X,WINDOW_SIZE_Y), IPL_DEPTH_8U, 3);
       printf ("%d %d\n", WINDOW_SIZE_X, WINDOW_SIZE_Y);
       cv_img->origin = 1;
       cvNamedWindow(WIN0,1);   // create 3 windows for cv to use
       cvNamedWindow(WIN1,1);
      // cvNamedWindow(WIN2,1);
       
       cv_windows[0]=(char*)malloc(10); cv_windows[1]=(char*)malloc(10); cv_windows[2]=(char*)malloc(10);
       strcpy(cv_windows[0], WIN0); strcpy(cv_windows[1], WIN1); strcpy(cv_windows[2], WIN2);
   }
   //glutMouseFunc (mouse);
   glutReshapeFunc (cv_reshape);                    // called when window resized
   glutKeyboardFunc (cv_keyboard);                  // called with key pressed
   glutDisplayFunc (cv_display);                    // called when glutPostRedisplay() raises redraw flag
   //glutSpecialFunc(special_input);                // called when special keys pressed

   //glutPassiveMotionFunc (passive_mouse_motion);    // no idea (mouse related)
   //glutTimerFunc(5, glutTimer, 0);                  // also mouse
   //gettimeofday(&last, NULL);                       // mouse
   
/* RZ
   if (!is_interactive)
   {
      printf("WARNING, motion is now controlled by physical model\n");
      printf("keyboard control will be restored if physical model is disabled\n");

      glutIdleFunc(anim_scene);

#if WIN_SERV
      HANDLE hThread;
      DWORD IDThread;

      hThread = CreateThread( NULL, 0, create_server, NULL, 0, &IDThread );
#elif LIN_SERV
      printf("Creating server thread\n");
      pthread_t thread1;
      int ret = pthread_create( &thread1, NULL, create_server, NULL);
      if (ret != 0)
         printf("Error creating thread\n");
#endif
   }*/
   //init_vision(!is_interactive);
   /*start the main glut loop*/
   
   
   glutMainLoop();
   
   cvDestroyWindow (WIN0);
   cvDestroyWindow (WIN1);
   cvDestroyWindow (WIN2);
   destroy();
   return 0;
}
