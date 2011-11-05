#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <cmath>

#include "calc_draw.h"
#include "preproc_filters.h"
#include "cv_tasks.h"

#define ANG_RNG(X) (((X) > 90) ? ((X)-180) : (X))

/*
#define GATE_HMIN 10 
#define GATE_HMAX 30 
#define GATE_SMIN 60
#define GATE_SMAX 255
*/
#define GATE_HMIN 45 
#define GATE_HMAX 90 
#define GATE_SMIN 130
#define GATE_SMAX 200

#define OP_DIM 7  // kernal rows for gradient
#define CLOSE_DIM 11

#define NLINES 30

#define WIN0 "window0"
#define WIN1 "window1"
#define WIN2 "window2"

#define PATH_SKINNYNESS 0.12

HSV_settings HSV (40, 90, 100, 250, 80, 250);

int main( int argc, char** argv ) {
    cvNamedWindow(WIN0,1);   // create 3 windows for cv to use
    cvMoveWindow(WIN0, 650, 0);
    cvNamedWindow(WIN1,1);
    cvMoveWindow(WIN1, 1000, 0);
    cvNamedWindow(WIN2,1);
    cvMoveWindow(WIN2, 650, 300);
    
    HSV.setRange1();
    
    char* cv_windows[3];
    cv_windows[0]=(char*)malloc(10); cv_windows[1]=(char*)malloc(10); cv_windows[2]=(char*)malloc(10);
    strcpy(cv_windows[0], WIN0); strcpy(cv_windows[1], WIN1); strcpy(cv_windows[2], WIN2);
    
    /** your code here */
    int gateX, gateY; float range;    
    // webcam video
    
    CvCapture* capture = cvCreateCameraCapture(1) ;    // create a webcam video capture
    IplImage* frame = cvQueryFrame( capture );         // read a single frame from the cam
/*
    CvVideoWriter * vid1 = cvCreateVideoWriter (       // create a video file to store video
        "webcam1.avi",              // name of the video file
        CV_FOURCC('P','I','M','1'), // video codec (don't worry about this one)
        25,                         // framerate that gets stored along with the video
        cvGetSize(frame),           // the resolution. 
        1);                         // 1 here means color video, 0 means not color
*/
    while(1) {                      // play the video like before     
        frame = cvQueryFrame( capture );
        if( !frame ) break;
        
        //cvWriteFrame( vid1, frame );      // write the frame to the video writer
        cvShowImage( WIN2, frame );
        vision_GATE (frame, gateX, gateY, range, HSV, cv_windows, _DISPLAY);
    
        char c = cvWaitKey(20);
        if( c == 'q' ) break;
    }
        
    /*
    IplImage* img = cvLoadImage( argv[1], CV_LOAD_IMAGE_ANYCOLOR); // load image with name argv[1], color
    
    int gateX, gateY;
    float range;
    vision_GATE (img, gateX,gateY, range, cv_windows);
    
    cvWaitKey(0);*/
    return 0;
}
