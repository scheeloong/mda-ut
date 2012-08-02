#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <string.h>
#include <time.h>

#include "calc_draw.h"
#include "preproc_filters.h"
#include "common.h"
#include "task_buoy.h"
#include "task_gate.h"

int main( int argc, char** argv ) {
    unsigned CAM_NUMBER = 0, DISPLAY = 1, WRITE = 1, BUOY = 0, GATE = 0;
    unsigned nframes = 0, t_start, t_end;
    float RES_SCALING = 1;
    vision_in Vin;
    vision_out Vout;
    
    if (argc == 1) 
        printf ("For options use --help\n\n");

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "1"))
            CAM_NUMBER = 1;
        else if (!strcmp (argv[i], "--small"))
            RES_SCALING = 0.5;
        else if (!strcmp (argv[i], "--med") || !strcmp (argv[i], "--medium"))
            RES_SCALING = 0.75;
        else if (!strcmp (argv[i], "--no_display") || !strcmp (argv[i], "--no_disp"))
            DISPLAY = 0;
        else if (!strcmp (argv[i], "--no_write"))
            WRITE = 0;
        else if (!strcmp (argv[i], "--gate"))
            GATE = 1;
        else if (!strcmp (argv[i], "--buoy"))
            BUOY = 1;
        else if (!strcmp (argv[i], "--help")) {
            printf ("OpenCV based webcam program. Hit 'q' to exit. Defaults to cam0, writes to \"webcam.avi\"\n");
            printf ("Put any integer as an argument (without --) to use that as camera number\n\n");
            printf ("  --small --med\n    Scale down the image. Speed up capture.\n\n");
            printf ("  --no_write\n    Does not write captured video to file.\n\n");
            printf ("  --no_display\n    Does not display captured video.\n\n");
            printf ("  Example: `./webcam 1 --med --no_write` will use cam1, medium size scaling, does not write to disk\n\n");
            return 0;
        }
    }
    
    if (DISPLAY) {
        cvNamedWindow("webcam", CV_WINDOW_AUTOSIZE); 
        cvMoveWindow("webcam", 100, 100); // offset from the UL corner of the screen
    }
    
    CvCapture* capture = cvCreateCameraCapture(CAM_NUMBER);    // create a webcam video capture
    IplImage* frame = cvQueryFrame( capture );         // read a single frame from the cam
    //IplImage* img = cvCreateImage (cvSize(frame->width*RES_SCALING,frame->height*RES_SCALING), 
      //                       frame->depth, frame->nChannels);
    
    IplImage* img = cvCreateImage (cvSize(480,360), frame->depth, frame->nChannels);

    if (BUOY || GATE) {
        if (DISPLAY) create_windows ();
        Vin.img = cvCreateImage (cvSize(img->width,img->height), 
                                 img->depth, img->nChannels);
        Vin.HSV.setAll (-30,30, 80,255,60,255);
    }
    
    CvVideoWriter * vid1;
    if (WRITE) {
            vid1 = cvCreateVideoWriter (       // create a video file to store video
            "webcam.avi",              // name of the video file
            CV_FOURCC('P','I','M','1'), // video codec (don't worry about this one)
            30,                         // framerate that gets stored along with the video resolution
            cvGetSize(frame),           // store the full size frame
            1);                         // 1 here means color video, 0 means not color
    }
    
    cvReleaseCapture(&capture);
    capture = cvCreateCameraCapture(CAM_NUMBER);
    char c;
    t_start = clock();
    
   /* for (int i = 0; i < 10; i++) {
        frame = cvQueryFrame (capture);
    }*/
    
    while(1) {                      // play the video like before     
        frame = cvQueryFrame (capture);
  	if (nframes < 50) {
		nframes++;
		continue;
	}
	      
        if (RES_SCALING != 1)
            cvResize(frame, img);
        else
            img = frame;
        
        if (DISPLAY)
            cvShowImage("webcam", img);
        
        if (BUOY) {
            Vin.img = img;
            if (DISPLAY) 
                vision_BUOY (Vin, Vout, _DISPLAY);
            else 
                vision_BUOY (Vin, Vout, 0);
        }
     
        if (GATE) {
            Vin.img = img;
            if (DISPLAY) 
                vision_GATE (Vin, Vout, _DISPLAY);
            else 
                vision_GATE (Vin, Vout, 0);
        }
    
        if (WRITE)
            cvWriteFrame(vid1, frame);      // write the frame to the video writer
      
        nframes++;  
        c = cvWaitKey(2);
        if(c == 'q') break;
    }
    
    t_end = clock();
    printf ("Average Framerate = %f\n", (float)nframes/(t_end-t_start)*CLOCKS_PER_SEC);

    cvDestroyWindow ("webcam");
    if ((BUOY || GATE) && DISPLAY) {
        destroy_windows();
    }
    return 0;
}
