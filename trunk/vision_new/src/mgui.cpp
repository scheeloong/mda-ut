#include "mgui.h"
#include "mv.h"

/** mvWindow methods **/
static bool windows_array[NUM_SUPPORTED_WINDOWS] = {false,false,false,false};

mvWindow:: mvWindow (const char _name[]) { // this has to be the h file
    assert (strlen(_name) < WINDOW_NAME_LEN);
    sprintf (name, "%s", _name);

    cvNamedWindow (name, CV_WINDOW_AUTOSIZE);

    int i = 0;
    while (windows_array[i] == true && i < NUM_SUPPORTED_WINDOWS) 
        i++;                    // find next free slot

    if (i >= NUM_SUPPORTED_WINDOWS) return; // return if no slot
    windows_array[i] = true;    // mark slot as used
    window_number = i;
    
    switch (window_number) {
        case 0: cvMoveWindow (name, 500,10); break;
        case 1: cvMoveWindow (name, 850,10); break;
        case 2: cvMoveWindow (name, 500,310); break;
        case 3: cvMoveWindow (name, 850,310); break;
    }
}

mvWindow:: ~mvWindow () { 
    cvDestroyWindow (name); 
    windows_array[window_number] = false;
}

/** mvCamera methods **/
mvCamera:: mvCamera (const char* settings_file) {
    unsigned width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);

    int cam_number, framerate; 
    // note the framerate gets stored in with the video file
    // it doesnt dictate the speed of reading frames from the camera
    read_mv_setting (settings_file, "CAMERA_NUMBER", cam_number);
    read_mv_setting (settings_file, "CAMERA_WRITE", _WRITE_);
    read_mv_setting (settings_file, "FRAMERATE", framerate);

    capture = cvCreateCameraCapture (cam_number);
    if (_WRITE_) {
        writer = cvCreateVideoWriter (
            "webcam.avi",			    // video file name
            CV_FOURCC('P','I','M','1'),	// codec
            30,					        // framerate
            cvSize(width,height),	    // size
            1
        );
    }

    imgResized = mvCreateImage_Color (width, height);
}

mvCamera:: ~mvCamera () {
    cvReleaseCapture (&capture);
    cvReleaseImage (&imgResized);
    if (_WRITE_) cvReleaseVideoWriter (&writer);
}

IplImage* mvCamera:: getFrameResized () {
    IplImage* frame = cvQueryFrame (capture);
    assert (frame != NULL); // only fails if something wrong with camera
    cvResize (frame, imgResized, CV_INTER_LINEAR); // bilienar interpolation
    return imgResized;
}