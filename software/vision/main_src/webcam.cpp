#include <stdio.h>
#include <cv.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "mgui.h"
#include "mv.h"
#include "mvLines.h"
#include "mvCircles.h"
#include "profile_bin.h"

void show_HSV_call_back (int event, int x, int y, int flags, void* param) {
// param must be the IplImage* pointer, with HSV color space    
    IplImage* img = (IplImage*) param;
    unsigned char * imgPtr;
    
    if (event == CV_EVENT_LBUTTONDOWN || event == CV_EVENT_RBUTTONDOWN) {
        // print the HSV values at x,y
        imgPtr = (unsigned char*) img->imageData + y*img->widthStep + x*img->nChannels;
        printf ("(%d,%d):  %u  %u  %u\n", x,y,imgPtr[0],imgPtr[1],imgPtr[2]);
    }

    if (event == CV_EVENT_RBUTTONDOWN) {
        usleep (500000);
    }
}

int main( int argc, char** argv ) {
    unsigned CAM_NUMBER=0, WRITE=0, TEST=0, CARTOON=0,
             LINE=0, CIRCLE=0, LOAD=0;
    unsigned long nframes = 0, t_start, t_end;
    
    if (argc == 1) 
        printf ("For options use --help\n\n");

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"1") || !strcmp(argv[i],"2"))
            CAM_NUMBER = atoi (argv[i]);
        else if (!strcmp (argv[i], "--cartoon"))
            CARTOON = 1;
        else if (!strcmp (argv[i], "--test"))
            TEST = 1;
        else if (!strcmp (argv[i], "--write"))
            WRITE = 1;
        else if (!strcmp (argv[i], "--line"))
            LINE = 1;
        else if (!strcmp (argv[i], "--circle"))
            CIRCLE = 1;
        else if (!strcmp (argv[i], "--load")) {
            LOAD = i+1; // put the next argument index into LOAD
            i++;        // skip next arg
        }
        else if (!strcmp (argv[i], "--help")) {
            printf ("OpenCV based webcam program. Hit 'q' to exit. Defaults to cam0, writes to \"webcam.avi\"\n");
            printf ("Put any integer as an argument (without --) to use that as camera number\n\n");
            printf ("  --write\n    Write captured video to file.\n\n");
            printf ("  --line\n    Run line finding code.\n\n");
            printf ("  --show-hsv\n    Press L/R mouse button on main window to show HSV value.\n\n");
            printf ("  Example: `./webcam 1 --write` will use cam1, and will write to disk\n\n");
            return 0;
        }
    }

    /// initialization
    // init camera
    mvCamera* camera = NULL;
    if (LOAD == 0)
        camera = new mvCamera (CAM_NUMBER);
    else
        camera = new mvCamera (argv[LOAD]);

    mvVideoWriter* writer = NULL;
    if (WRITE)
        writer = new mvVideoWriter ("webcam.avi");
    
    // init windows
    mvWindow* win1 = new mvWindow ("webcam");
    mvWindow* win2 = new mvWindow ("win2");
    mvWindow* win3 = new mvWindow ("win3");

    // declare filters we need
    mvHSVFilter HSVFilter ("test_settings.csv"); // color filter
    mvBinaryMorphology Morphology7 (7,7, MV_KERN_ELLIPSE);
    mvBinaryMorphology Morphology5 (5,5, MV_KERN_ELLIPSE);
    mvHoughLines HoughLines ("test_settings.csv");
    mvLines lines; // data struct to store lines
    mvAdvancedCircles circles ("test_settings.csv");
    mvKMeans kmeans;

    mvAdaptiveFilter3 adaptive ("test_settings.csv");

    // declare images we need
    IplImage* scratch_color = mvCreateImage_Color();
    IplImage* HSV_img = mvCreateImage_Color();
    IplImage* filter_img = mvCreateImage ();
    //IplImage* filter_img2 = mvCreateImage ();
 
    cvSetMouseCallback ("webcam", show_HSV_call_back, HSV_img);

    /// execution
    char c;
    IplImage* frame;

    t_start = clock();

    for (;;) {
        frame = camera->getFrameResized(); // read frame from cam
        if (LOAD) {
            usleep(17000);
        }

        if (!frame) {
            printf ("Video Finished.\n");
            break;
        }
        
        if (nframes < 10) {// || nframes % 2 != 0) {
            nframes++;
            continue;
        }
 
        if (WRITE) {
            writer->writeFrame (frame);
        }

        cvCvtColor (frame, HSV_img, CV_BGR2HSV);

        if (CARTOON) {
            win1->showImage (frame);

            for (int i = 0; i < frame->height; i+=40) {
                for (int j = 0; j < frame->width; j+=40) {
                    unsigned char* pixel = (unsigned char*) (frame->imageData + i*frame->widthStep + j*3);
                    CvScalar mycolor = cvScalar(*(pixel+0), *(pixel+1), *(pixel+2));
                    
                    cvFloodFill(frame, cvPoint(j,i), mycolor, cvScalar(20,20,20), cvScalar(20,20,20), NULL, CV_FLOODFILL_FIXED_RANGE);
                }    
            }

            win2->showImage (frame);
            goto LOOP_BOTTOM;
        }

        win1->showImage (frame);
        
        /*HSVFilter.filter (frame, filter_img); // process it
        win2->showImage (filter_img);
           
        Morphology7.open (filter_img, filter_img);
        Morphology5.gradient (filter_img, filter_img);
        win3->showImage (filter_img);
      */
        
        if (TEST) {             
            mvMeanShift (frame, scratch_color, 5, 10, 25, 30);
            AdaptiveFilter2(scratch_color, filter_img);

            win2->showImage (scratch_color);
            win3->showImage (filter_img);
        }
        else if (LINE) {
            HoughLines.findLines (filter_img, &lines);
            kmeans.cluster_auto (1, 8, &lines, 1);
        
            //lines.drawOntoImage (filter_img);
            kmeans.drawOntoImage (filter_img);
            lines.clearData(); // erase line data and reuse allocated mem
            //kmeans.clearData();
            
            win3->showImage (filter_img);
        }
        else if (CIRCLE) {
            circles.findCircles (filter_img);
            printf ("ncircles = %d\n", circles.ncircles());
            circles.drawOntoImage (filter_img);

            win3->showImage (filter_img);
        }
        
    LOOP_BOTTOM:
        nframes++;
        c = cvWaitKey(5);
        if (c == 'q') 
            break;
    }
    
    t_end = clock ();
    printf ("\nAverage Framerate = %f\n", (float)nframes/(t_end - t_start)*CLOCKS_PER_SEC);
    
    cvReleaseImage (&HSV_img);
    cvReleaseImage (&scratch_color);
    cvReleaseImage (&filter_img);
    delete camera;
    if (writer)
        delete writer;
    delete win1;
    delete win2;
    delete win3;
    return 0;
}