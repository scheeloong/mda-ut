#include <GL/glut.h>
#include <cv.h>
#include "types.h"
#include "physical_model.h"
#include "motors.h"
#include "sim.h"

#include "../vision/lib/mda_vision.h"
#include "../control/lib/vci.h"
#include "../control/lib/mda_control.h"

/**
 * Because the opencv code we have assumes that images passed to it are of the size 
 * COMMON_IMG_WIDTH and COMMON_IMG_HEIGHT in the settings file, we have to ensure we
 * abide by this. We will use a temp image and rescale that image to common size
 */

IplImage* cv_img_fwd ;
IplImage* cv_img_down; // stores the front and down camera imgs respectively
IplImage* cv_img_result;

bool FLAG_USE_IMG_DOWN; // make this true if you want to use img_down instead of img_fwd

extern unsigned DEBUG_MODEL;
extern CV_TASK_ENUM cv_task_enum;
extern physical_model model;

// variables for vision and control
MDA_VISION_MODULE_BASE* vision_module;
MDA_CONTROL_MODULE_BASE* control_module;
VCI interface;

void cv_init () {
    unsigned width=600, height = 400; // temporary
    
    vision_module = NULL;
    FLAG_USE_IMG_DOWN = false;
    
    if (cv_task_enum == CV_VISION_TEST)
        vision_module = new MDA_VISION_MODULE_TEST;
    else if (cv_task_enum == CV_VISION_GATE)
        vision_module = new MDA_VISION_MODULE_GATE;
    else if (cv_task_enum == CV_VISION_PATH) {
        vision_module = new MDA_VISION_MODULE_PATH;
        FLAG_USE_IMG_DOWN = true;
    }
    else if (cv_task_enum == CV_VISION_BUOY)
        vision_module = new MDA_VISION_MODULE_BUOY;
    else if (cv_task_enum == CV_VISION_FRAME)
        vision_module = new MDA_VISION_MODULE_FRAME;

    cv_img_fwd = cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 3);
    cv_img_fwd->origin = 1;
    cv_img_down = cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 3);
    cv_img_down->origin = 1;
    
    cvNamedWindow("Downwards Cam",1);
    cvMoveWindow ("Downwards Cam", 380,0);
}

void cvQueryFrameGL (IplImage* img) {
// this function reads the OpenGL buffer and puts the data into an iplimage.
// Plz allocate the image before calling
// MAKE SURE the ORIGIN parameter in img is set to 1 !!!
    // read the GL buffer. This is a lot easier than that guy on the internet said
    glReadPixels(0,0, img->width-1,img->height-1, GL_BGR,GL_UNSIGNED_BYTE, img->imageData);
}

void cv_display () {
// this function is called by glutMainLoop every time the window needs to be redrawn 
// the redraw flag is raised by the function glutPostRedisplay();
// with CV_VISION_FLAG the cv code will run every time the window is updated.
    IplImage* img_to_use = NULL;
   
    if (cv_task_enum != NO_TASK) {
        // first grab both front and bottom cam images       
        model.angle.pitch += 90;      // grab downwards first
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        set_camera();
        draw();
        cvQueryFrameGL (cv_img_down);
        
        model.angle.pitch -= 90;
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        set_camera();
        draw();
        glutSwapBuffers();
        cvQueryFrameGL (cv_img_fwd);       

        cvShowImage ("Downwards Cam", cv_img_down);
        
        img_to_use = FLAG_USE_IMG_DOWN ? cv_img_down : cv_img_fwd;
        
        /** OPENCV CODE GOES HERE. */    
        if (vision_module != NULL) {
            vision_module->filter (img_to_use, &interface);
        }
        /** END OPENCV CODE */

        char c = cvWaitKey(5); // without 5ms delay the window will not show properly
        if (c == 'q' || c == 27) {
            destroy ();
            exit(0);
        }
    }
    else {
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        set_camera();
        draw();              // front camera
        glutSwapBuffers();   
        // we are using double buffering. Front buffer is displayed. you draw on the back
        // buffer, which isnt displayed. This is so theres time to draw everything without 
        // flicker or tear.
        // when done drawing you use glutSwapBuffers to switch front and back buffers.
    }   
    
    if (DEBUG_MODEL)
        model.print();

    glFlush();
    //copy_bytes();
}

inline
void range_angle(float& angle)
{
    if (angle >= 180)
        angle -= 360;
    else if (angle <= -180)
        angle += 360;
}

void cv_keyboard(unsigned char key, int x, int y)
{
    static Motors m = Motors(&model);

    if (key == 'q' || key == 27) { // q or escape
        destroy ();
        exit(0);
    }
    else if (key == 'p') {
        //screenshot();
        cvSaveImage ("cvSimImgFwd.jpg", cv_img_fwd);
        cvSaveImage ("cvSimImgDwn.jpg", cv_img_down);
    }   
    else { // pass the key to the motor object to handle
      m.key_command(key);
    }

    glutPostRedisplay();
}

void cv_reshape(int w, int h)
// have to change opencv struct size when changing windowz
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(CAMERA_FIELD_OF_VIEW, (GLfloat)w/(GLfloat)h, .05, 200.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   // resize the opencv structure too eh
   if (cv_task_enum) {
       cvReleaseImage (&cv_img_fwd);
       cv_img_fwd = cvCreateImage (cvSize(w,h), IPL_DEPTH_8U, 3);
       cv_img_fwd->origin=1;
       cvReleaseImage (&cv_img_down);
       cv_img_down = cvCreateImage (cvSize(w,h), IPL_DEPTH_8U, 3);
       cv_img_down->origin=1;
    }
}
