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
#include "mvContours.h"
#include "profile_bin.h"

unsigned CAM_NUMBER=0;
unsigned WRITE=0;
unsigned TEST=0;
unsigned GRAD=0;
unsigned FLOOD=0;
unsigned WATERSHED=0;
unsigned LINE=0;
unsigned CIRCLE=0;
unsigned RECT=0;
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
        else if (!strcmp (argv[i], "--rect"))
            RECT = 1;
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
    mvWatershedFilter watershed_filter;
    mvContours contour_filter;

    // declare images we need
    IplImage* scratch_color = mvCreateImage_Color();
    IplImage* scratch_color_2 = mvCreateImage_Color();
    IplImage* filter_img = mvCreateImage ();
    IplImage* filter_img_2 = mvCreateImage ();
 
    /// execution
    char c = 0;
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
            if (c == 'L') { 
                advanced_filter.flood_image(frame, filter_img, true);
            }
            else {
                advanced_filter.flood_image(frame, filter_img);
            }
            Morphology5.open(filter_img, filter_img);
            Morphology7.close(filter_img, filter_img);
            
            win2->showImage (filter_img);
        }
        else if (WATERSHED) {
            watershed_filter.watershed(frame, filter_img);
            win1->showImage (frame);
            win2->showImage (filter_img);
            
            int seg = 0;
            const double COLOR_DIVISION_FACTOR = 200;
            COLOR_TRIPLE color;
            //COLOR_TRIPLE color_template (160,95,157,0);
            COLOR_TRIPLE color_template (155,120,60,0);

            CvPoint best_centroid;
            float best_length, best_angle;
            double best_diff = 1000000;
            
            while ( watershed_filter.get_next_watershed_segment(filter_img_2, color) ) {
                printf ("\nSegment %d\n", ++seg);
                printf ("\tColor (%3d,%3d,%3d)\n", color.m1, color.m2, color.m3);

                // calculate color diff
                double color_diff = static_cast<double>(color.diff(color_template)) / COLOR_DIVISION_FACTOR;

                // calculate shape diff
                CvPoint centroid;
                float length, angle;
                double shape_diff = contour_filter.match_rectangle(filter_img_2, centroid, length, angle);
                if (shape_diff < 0) // error from the shape matching
                    continue;

                double diff = color_diff + shape_diff;
                //double diff = shape_diff;
                printf ("\tColor_Diff=%6.4f  Shape_Diff=%6.4f\n\tFinal_Diff=%6.4f\n", color_diff, shape_diff, diff);

                if (seg == 1 || diff < best_diff) {
                    best_diff = diff;
                    best_centroid = centroid;
                    best_length = length;
                    best_angle = angle;
                    cvCopy (filter_img_2, filter_img);
                }
                //cvWaitKey(0);
            }

            win3->showImage (filter_img);
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
            /*circles.find (filter_img);
            printf ("ncircles = %d\n", circles.ncircles());
            circles.drawOntoImage (filter_img);

            win3->showImage (filter_img);
            */
            CvPoint centroid;
            float radius;
            contour_filter.match_circle(filter_img, centroid, radius);
            win3->showImage (filter_img);
        }
        else if (RECT) {
            CvPoint centroid;
            float length, angle;
            contour_filter.match_rectangle(filter_img, centroid, length, angle);
            win3->showImage (filter_img);
        }
        
        nframes++;
        if (BREAK)
            c = cvWaitKey(0);
        else if (LOAD)
            c = cvWaitKey(66); // go for about 15 frames per sec
        else
            c = cvWaitKey(5);

        if (c == 'q')
            break;
        else if (c == 'w') {
            mvDumpPixels (frame, "webcam_img_pixel_dump.csv");
            mvDumpHistogram (frame, "webcam_img_histogram_dump.csv");
        }
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
