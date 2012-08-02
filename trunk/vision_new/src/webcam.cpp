#include <stdio.h>
#include <cv.h>
#include <time.h>
#include <string.h>

#include "mgui.h"
#include "mv.h"
#include "mvLines.h"
#include "mvCircles.h"

int main( int argc, char** argv ) {
    unsigned CAM_NUMBER = 0, DISPLAY = 1, WRITE = 1;
    unsigned long nframes = 0, t_start, t_end;
    
    if (argc == 1) 
        printf ("For options use --help\n\n");

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i],"1") || !strcmp(argv[i],"2"))
            CAM_NUMBER = atoi (argv[i]);
        else if (!strcmp (argv[i], "--no_display") || !strcmp (argv[i], "--no_disp"))
            DISPLAY = 0;
        else if (!strcmp (argv[i], "--no_write"))
            WRITE = 0;
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
	if (CAM_NUMBER == 0)
		camera = new mvCamera ("settings/camera_0_settings.csv");
    else if (CAM_NUMBER == 1)
		camera = new mvCamera ("settings/camera_1_settings.csv");
    else if (CAM_NUMBER == 2)
	    camera = new mvCamera ("settings/camera_2_settings.csv");
    
    // init windows
    mvWindow* win1 = DISPLAY ? new mvWindow ("webcam") : NULL;
    mvWindow* win2 = DISPLAY ? new mvWindow ("result") : NULL;

    // declare filters we need
    mvHSVFilter HSVFilter ("settings/HSVFilter_settings.csv"); // color filter
    mvGradient gradient ("settings/gradient_settings.csv");
    mvHoughLines HoughLines ("settings/HoughLines_settings.csv");
    mvLines lines; // data struct to store lines
    mvHoughCircles HoughCircles ("settings/HoughCircles_settings.csv");
    mvCircles circles; // data struct to store circles
    mvKMeans kmeans;

    // declare images we need
    IplImage* filter_img = mvCreateImage ();
    //IplImage* grad_img = mvCreateImage ();
    
    /// execution
    char c;
    IplImage* frame;
    t_start = clock();    
    
    for (;;) {
        frame = camera->getFrameResized(); // read frame from cam
	if (nframes < 50 || nframes % 3 != 0) {
		nframes++;
		continue;
	}

        HSVFilter.filter (frame, filter_img); // process it
        cvErode (filter_img, filter_img);
        //gradient.filter (filter_img, grad_img);
        //grad_img = filter_img;
        HoughLines.findLines (filter_img, &lines);
        //lines.drawOntoImage (filter_img);
        kmeans.cluster_auto (1, 2, &lines);
        kmeans.drawOntoImage (filter_img);
        //HoughCircles.findCircles (grad_img, &circles);
        //circles.drawOntoImage (grad_img);

        if (DISPLAY) {
            win1->showImage (frame);
            win2->showImage (filter_img);
        }

        if (WRITE)
            camera->writeFrame (frame);
    
        lines.clearData(); // erase line data and reuse allocated mem
        circles.clearData();

        nframes++;
        c = cvWaitKey(2);
        if (c == 'q') 
            break;
    }
    
    t_end = clock ();
    printf ("\nAverage Framerate = %f\n", (float)nframes/(t_end - t_start)*CLOCKS_PER_SEC);

    return 0;
}
