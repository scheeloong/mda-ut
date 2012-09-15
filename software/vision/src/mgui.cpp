#include "mgui.h"
#include "mv.h"

/** mvWindow methods **/
static bool WINDOWS_ARRAY[NUM_SUPPORTED_WINDOWS] = {false,false,false,false};

mvWindow:: mvWindow (const char name[]) { // this has to be the h file
    assert (strlen(name) < WINDOW_NAME_LEN);
    sprintf (_name, "%s", name);

    cvNamedWindow (_name, CV_WINDOW_AUTOSIZE);

    unsigned i = 0;
    while (i < NUM_SUPPORTED_WINDOWS && WINDOWS_ARRAY[i] == true) 
        i++;                    // find next free slot

    if (i >= NUM_SUPPORTED_WINDOWS) return; // return if no slot
    WINDOWS_ARRAY[i] = true;    // mark slot as used
    _window_number = i;
    
    switch (_window_number) {
        case 0: cvMoveWindow (_name, 500,10); break;
        case 1: cvMoveWindow (_name, 850,10); break;
        case 2: cvMoveWindow (_name, 500,310); break;
        case 3: cvMoveWindow (_name, 850,310); break;
    }
}

mvWindow:: ~mvWindow () { 
    cvDestroyWindow (_name); 
    WINDOWS_ARRAY[_window_number] = false;
}

/** mvCamera methods **/
mvCamera:: mvCamera (unsigned cam_number, unsigned write) {
    unsigned width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);

    _capture = cvCreateCameraCapture (cam_number);
    if (write) {
        _writer = cvCreateVideoWriter (
            "webcam.avi",               // video file name
            CV_FOURCC('P','I','M','1'), // codec
            FRAMERATE,                  // framerate
            cvSize(width,height),       // size
            1
        );
    }
    else
        _writer = NULL;

    _imgResized = mvCreateImage_Color (width, height);
}

mvCamera:: mvCamera (const char* video_file) {
    unsigned width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    
    _capture = cvCreateFileCapture (video_file);   
    _imgResized = mvCreateImage_Color (width, height);
    _writer = NULL;
}

mvCamera:: ~mvCamera () {
    cvReleaseCapture (&_capture);
    cvReleaseImage (&_imgResized);
    if (_writer != NULL) cvReleaseVideoWriter (&_writer);
}

IplImage* mvCamera:: getFrameResized () {
    IplImage* frame = cvQueryFrame (_capture);
    if (!frame)
        return NULL;
    cvResize (frame, _imgResized, CV_INTER_LINEAR); // bilienar interpolation
    return _imgResized;
}
