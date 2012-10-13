#include <stdio.h>
#include <cv.h>
#include <time.h>
#include <string.h>

#include "mgui.h"
#include "mv.h"
#include "mvLines.h"
#include "mvCircles.h"

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
            printf ("  --no_write\n    Does not write captured video to file.\n\n");
            printf ("  --no_display\n    Does not display captured video.\n\n");
            printf ("  Example: `./webcam 1 --no_write` will use cam1, and will not write to disk\n\n");
            return 0;
        }
    }

    /// initialization
    // init camera
    mvCamera* camera = NULL;
    if (LOAD == 0)
        camera = new mvCamera (CAM_NUMBER, WRITE);
    else
        camera = new mvCamera (argv[LOAD]);
    
    // init windows
    mvWindow* win1 = new mvWindow ("webcam");
    mvWindow* win2 = new mvWindow ("filtered");
    mvWindow* win3 = new mvWindow ("result");

    // declare filters we need
    mvHSVFilter HSVFilter ("test_settings.csv"); // color filter
    mvHoughLines HoughLines ("test_settings.csv");
    mvLines lines; // data struct to store lines
    mvHoughCircles HoughCircles ("test_settings.csv");
    mvCircles circles; // data struct to store circles
    mvKMeans kmeans;

    // declare images we need
    IplImage* myframe = mvCreateImage_Color();
    IplImage* filter_img = mvCreateImage ();
    IplImage* grad_img = mvCreateImage ();
    
    /// execution
    char c;
    IplImage* frame;
    t_start = clock();    
    
    for (;;) {
        frame = camera->getFrameResized(); // read frame from cam
        if (!frame) {
            printf ("Video Finished.\n");
            break;
        }
        
        if (nframes < 40) {// || nframes % 2 != 0) {
            nframes++;
            continue;
        }

        HSVFilter.filter (frame, filter_img); // process it
        mvDilate (filter_img, filter_img, 5, 5);
        mvErode (filter_img, filter_img, 7, 7);
        
        mvGradient (filter_img, grad_img, 5, 5);
        //cvDilate (grad_img, grad_img);
        
        if (CARTOON) {
            cvZero (filter_img);
            cvCopy (frame, myframe);

            win1->showImage (myframe);

            for (int i = 0; i < myframe->height; i+=40) {
                for (int j = 0; j < myframe->width; j+=40) {
                    unsigned char* pixel = (unsigned char*) (myframe->imageData + i*myframe->widthStep + j*3);
                    CvScalar mycolor = cvScalar(*(pixel+0), *(pixel+1), *(pixel+2));
                    
                    cvFloodFill(myframe, cvPoint(j,i), mycolor, cvScalar(20,20,20), cvScalar(20,20,20), NULL, CV_FLOODFILL_FIXED_RANGE);
                }    
            }

            cvCvtColor (myframe, filter_img, CV_RGB2GRAY);
            mvGradient (filter_img, grad_img, 5, 5);
            mvErode (grad_img, grad_img, 3, 3);

            for (int i = 0; i < myframe->height; i++) {
                for (int j = 0; j < myframe->width; j++) {
                    unsigned char* pixel = (unsigned char*) (myframe->imageData + i*myframe->widthStep + j*3);
                    unsigned char* gpixel = (unsigned char*) (grad_img->imageData + i*grad_img->widthStep + j);

                    if (*gpixel > 40) {
                        *pixel = *(pixel+1) = *(pixel+2) = 0;
                    }
                }    
            }

            win2->showImage (myframe);
            win3->showImage (grad_img);
        }

        if (LINE) {
            HoughLines.findLines (grad_img, &lines);
            kmeans.cluster_auto (1, 8, &lines, 1);
        
            lines.drawOntoImage (grad_img);
            //kmeans.drawOntoImage (grad_img);
            kmeans.drawOntoImage (frame);
            
            lines.clearData(); // erase line data and reuse allocated mem
            //kmeans.clearData();
            
            win1->showImage (frame);
            win2->showImage (filter_img);
            win3->showImage (grad_img);
        }

        if (CIRCLE) {
            HoughCircles.findCircles (grad_img, &circles);
            printf ("ncircles = %d\n", circles.ncircles());
            circles.drawOntoImage (grad_img);
            //circles.clearData();
            win1->showImage (frame);
            win2->showImage (filter_img);
            win3->showImage (grad_img);
        }
        
        if (WRITE)
            camera->writeFrame (frame);
    
        circles.clearData();

        nframes++;
        c = cvWaitKey(20);
        if (c == 'q') 
            break;
    }
    
    t_end = clock ();
    printf ("\nAverage Framerate = %f\n", (float)nframes/(t_end - t_start)*CLOCKS_PER_SEC);

    return 0;
}
