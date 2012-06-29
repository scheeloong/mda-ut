#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <string.h>

#include "calc_draw.h"
#include "preproc_filters.h"
#include "common.h"

int main( int argc, char** argv ) {
    unsigned CAM_NUMBER = 0, DISPLAY = 1, WRITE = 1;
    float RES_SCALING = 1;
    
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
    IplImage* img = cvCreateImage (cvSize(frame->width*RES_SCALING,frame->height*RES_SCALING), 
                             frame->depth, frame->nChannels);
    
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
    
   /* for (int i = 0; i < 100; i++) {
        frame = cvQueryFrame (capture);
    }*/
    
    while(1) {                      // play the video like before     
        frame = cvQueryFrame (capture);
        
        if (RES_SCALING != 1)
            cvResize(frame, img);
				else
					img = frame;
        
        if (DISPLAY)
            cvShowImage("webcam", img);
    
        if (WRITE)
            cvWriteFrame(vid1, frame);      // write the frame to the video writer
        
        c = cvWaitKey(16);
        if(c == 'q') break;
    }
    
    cvDestroyWindow ("webcam");
    return 0;
}
