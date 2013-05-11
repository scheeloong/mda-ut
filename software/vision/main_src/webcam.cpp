#include <stdio.h>
#include <cv.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "mgui.h"
#include "mv.h"
#include "mvColorFilter.h"
#include "mvLines.h"
#include "mvShapes.h"
#include "profile_bin.h"

unsigned CAM_NUMBER=0;
unsigned WRITE=0;
unsigned TEST=0;
unsigned GRAD=0;
unsigned FLOOD=0;
unsigned WATERSHED=0;
unsigned LINE=0;
unsigned CIRCLE=0;
unsigned LOAD=0;
unsigned BREAK=0;

int main( int argc, char** argv ) {
    unsigned long nframes = 0, t_start, t_end;
    
    if (argc == 1) 
        printf ("For options use --help\n\n");

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"1") || !strcmp(argv[i],"2"))
            CAM_NUMBER = atoi (argv[i]);
        else if (!strcmp (argv[i], "--test"))
            TEST = 1;
        else if (!strcmp (argv[i], "--grad"))
            GRAD = 1;
        else if (!strcmp (argv[i], "--flood"))
            FLOOD = 1;
        else if (!strcmp (argv[i], "--watershed"))
            WATERSHED = 1;
        else if (!strcmp (argv[i], "--write"))
            WRITE = 1;
        else if (!strcmp (argv[i], "--break"))
            BREAK = 1;
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
            printf ("  --load\n    Use a video file (following --load) instead of a webcam.\n\n");
            printf ("  --line\n    Run line finding code.\n\n");
            printf ("  --circle\n    Run circle finding code.\n\n");
            printf ("  --break\n    Pause the input (press any key to go to the next frame).\n\n");
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
    mvBinaryMorphology Morphology7 (9,9, MV_KERN_ELLIPSE);
    mvBinaryMorphology Morphology5 (5,5, MV_KERN_ELLIPSE);
    mvHoughLines HoughLines ("test_settings.csv");
    mvLines lines; // data struct to store lines
    mvAdvancedCircles circles ("test_settings.csv");
    mvKMeans kmeans;
    mvHistogramFilter histogram_filter ("test_settings.csv");
    mvAdvancedColorFilter advanced_filter("test_settings.csv");

    // declare images we need
    IplImage* scratch_color = mvCreateImage_Color();
    IplImage* scratch_color_2 = mvCreateImage_Color();
    IplImage* filter_img = mvCreateImage ();
    IplImage* filter_img_2 = mvCreateImage ();
 
    CvMemStorage* storage = cvCreateMemStorage(0);

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
        
        if (nframes < 10) {
            nframes++;
            continue;
        }
 
        if (WRITE) {
            writer->writeFrame (frame);
        }

        cvCopy (frame, scratch_color);
        win1->showImage (scratch_color);
        
        if (TEST) {             
        }
        else if (FLOOD) {
            advanced_filter.flood_image(frame, filter_img);
            Morphology5.open(filter_img, filter_img);
            Morphology7.close(filter_img, filter_img);
            win2->showImage (filter_img);
        
            CvSeq * contours;

            cvFindContours (
                    filter_img,
                    storage,
                    &contours,
                    sizeof(CvContour),
                    CV_RETR_EXTERNAL,
                    CV_CHAIN_APPROX_SIMPLE
                );
/*
            CvSeq* hull = cvConvexHull2 (
                    contours,
                    storage,
                    CV_CLOCKWISE,
                    0
                );
*/          

            cvZero (filter_img);

            CvBox2D ellipse = cvFitEllipse2(contours);
            cvEllipseBox(filter_img, ellipse, cvScalar(200,200,200));

            //float centroid_x = ellipse.center.x - filter_img->width*0.5;
            //float centroid_y = ellipse.center.y - filter_img->height*0.5;

            float centroid_x_alt = 0, centroid_y_alt = 0;
            double Area = 0;

            if (contours->total != 0){
                Area = cvContourArea(contours);

                CvMoments mom;
                cvMoments(contours, &mom);

                centroid_x_alt = mom.m10/mom.m00 - filter_img->width*0.5;
                centroid_y_alt = mom.m01/mom.m00 - filter_img->height*0.5;
            }


            printf("Angle is: %5.2f\n", ellipse.angle);
            //printf("Center is: (%5.2f, %5.2f)\n",centroid_x,centroid_y);
            printf("Alt Center is: (%5.2f, %5.2f)\n",centroid_x_alt, centroid_y_alt);
            printf("Area is: %5.2fpxsq\n",Area);

            CvSeq* hull = cvApproxPoly(
                contours,
                sizeof(CvContour),
                storage,
                CV_POLY_APPROX_DP,
                5,
                0
                );

            cvDrawContours (
                    filter_img,
                    hull,
                    cvScalar(200,200,200),
                    cvScalar(0,0,0),
                    0
            );

            win3->showImage (filter_img);
        }
        else if (WATERSHED) {
            advanced_filter.watershed(frame, scratch_color_2);
            
            win2->showImage (scratch_color_2);   
        }

        if (GRAD) {
            Morphology5.gradient(filter_img, filter_img);
        }

        if (LINE) {
            HoughLines.findLines (filter_img, &lines);
            kmeans.cluster_auto (1, 8, &lines, 1);
        
            //lines.drawOntoImage (filter_img);
            kmeans.drawOntoImage (filter_img);
            lines.clearData(); // erase line data and reuse allocated mem
            //kmeans.clearData();
            
            win3->showImage (filter_img);
        }
        else if (CIRCLE) {
            circles.find (filter_img);
            printf ("ncircles = %d\n", circles.ncircles());
            circles.drawOntoImage (filter_img);

            win3->showImage (filter_img);
        }
        
        nframes++;
        if (BREAK)
            c = cvWaitKey(0);
        else
            c = cvWaitKey(5);
        
        if (c == 'q')
            break;
        else if (c == 'w')
            mvDumpPixels (frame, "webcam_img_dump.csv");
    }
    
    t_end = clock ();
    printf ("\nAverage Framerate = %f\n", (float)nframes/(t_end - t_start)*CLOCKS_PER_SEC);
    
    cvReleaseImage (&scratch_color);
    cvReleaseImage (&scratch_color_2);
    cvReleaseImage (&filter_img);
    cvReleaseImage (&filter_img_2);
    delete camera;
    if (writer)
        delete writer;
    delete win1;
    delete win2;
    delete win3;
    return 0;
}
