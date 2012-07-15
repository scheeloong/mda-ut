#include <stdio.h>
#include <cv.h>
#include <string.h>

#include "mgui.h"
#include "mv.h"

int main( int argc, char** argv ) {
    unsigned CAM_NUMBER = 0, DISPLAY = 1, WRITE = 1;
    
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

    mvCamera* camera = NULL;
	if (CAM_NUMBER == 0)
		camera = new mvCamera ("settings/camera_0_settings.csv");
    else if (CAM_NUMBER == 1)
		camera = new mvCamera ("settings/camera_1_settings.csv");
    else if (CAM_NUMBER == 2)
	    camera = new mvCamera ("settings/camera_2_settings.csv");
    
    mvWindow* window = NULL;
    if (DISPLAY)
        window = new mvWindow ("webcam");

    char c;
    IplImage* frame;

    for (;;) {
        frame = camera->getFrame();
    
        if (DISPLAY)
            window->showImage (frame);

        if (WRITE)
            camera->writeFrame (frame);
    
        c = cvWaitKey(10);
        if (c == 'q') 
            break;
    }

    return 0;
}
