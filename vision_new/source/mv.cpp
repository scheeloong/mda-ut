#include "mv.h"
#include "mgui.h"
#include <cv.h>
#include <stdlib.h>

mvGradient:: mvGradient (const char* settings_file_name) {   
    read_common_mv_setting ("IMG_WIDTH_COMMON", IMG_WIDTH);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", IMG_HEIGHT);
    read_mv_setting (settings_file_name, "KERNEL_WIDTH", KERNEL_WIDTH);
    read_mv_setting (settings_file_name, "KERNEL_HEIGHT", KERNEL_HEIGHT);
    read_mv_setting (settings_file_name, "DISPLAY", _DISPLAY_);
    read_mv_setting (settings_file_name, "QUIET", _QUIET_);
    
    assert (KERNEL_WIDTH % 2 == 1); // the kernel size must be odd
    assert (KERNEL_HEIGHT % 2 == 1); 
    
    scratch = cvCreateImage ( 
        cvSize(IMG_WIDTH,IMG_HEIGHT),
        IPL_DEPTH_8U,     // depth
        1                 // nChannels
    );
    kernel = cvCreateStructuringElementEx (
        KERNEL_WIDTH, KERNEL_HEIGHT, 
        (KERNEL_WIDTH+1)/2, (KERNEL_HEIGHT+1)/2, 
        CV_SHAPE_RECT
    );
    
    if (_DISPLAY_) {
        sprintf (window_name, "%s", "mvGradient");
        window = new mvWindow(window_name);
    }
}

mvGradient:: ~mvGradient () {
    cvReleaseImage (&scratch);
    cvReleaseStructuringElement (&kernel);
    if (_DISPLAY_) delete window;
}

void mvGradient:: filter (const IplImage* img, IplImage* result)
{
    assert (img != NULL);
    assert (img->nChannels == 1);
        
    cvMorphologyEx (
        img, result, scratch, 
        kernel,
        CV_MOP_GRADIENT,
        1       // iterations 
    );   
    
    if (_DISPLAY_)
        window->showImage (result);
}