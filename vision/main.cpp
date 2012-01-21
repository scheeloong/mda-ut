#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <cmath>

#include "calc_draw.h"
#include "preproc_filters.h"
#include "common.h"
#include "task_gate.h"
#include "task_path.h"

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

int main( int argc, char** argv ) {
    vision_in Vin;
    vision_out Vout;
    create_windows ();
    Vin.HSV.setAll (35,90, 90,255,90,255);
    
    // webcam video
    CvCapture* capture = cvCreateCameraCapture(1);    // create a webcam video capture
    IplImage* frame = cvQueryFrame( capture );         // read a single frame from the cam
    Vin.img = cvCreateImage (cvSize(frame->width/2,frame->height/2), 
                                      frame->depth, frame->nChannels);
/*
    CvVideoWriter * vid1 = cvCreateVideoWriter (       // create a video file to store video
        "webcam1.avi",              // name of the video file
        CV_FOURCC('P','I','M','1'), // video codec (don't worry about this one)
        25,                         // framerate that gets stored along with the video
        cvGetSize(frame),           // the resolution. 
        1);                         // 1 here means color video, 0 means not color
*/  
    cvReleaseCapture(&capture);
    capture = cvCreateCameraCapture(1);
    char c;
    while(1) {                      // play the video like before     
        frame = cvQueryFrame (capture);
        //cvWaitKey(20);
        //frame2 = cvQueryFrame (capture);        
        //cvAddWeighted (frame, 0.5, frame2, 0.5, 0, frame);
        //if( !frame ) break;
        
        //cvWriteFrame( vid1, frame );      // write the frame to the video writer
        //vision_SQUARE (frame, gateX, gateY, range, HSV, cv_windows, _DISPLAY | _QUIET);
        
        Vin.img = frame;
	    //cvResize(frame,Vin.img);
        cvShowImage(WIN2, Vin.img);
      printf ("str %s\n", Vin.window[0]);    
        vision_GATE (Vin, Vout, _DISPLAY);
        //vision_GATE (frame2, gateX,gateY, range, HSV, cv_windows, _DISPLAY);
      printf ("str2 %s\n", Vin.window[0]);  
        c = cvWaitKey(10);
        if( c == 'q' ) break;
    
    }
    
    destroy_windows();
    return 0;
}
