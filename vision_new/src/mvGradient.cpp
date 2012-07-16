#include "mv.h"
#include "mgui.h"
#include <cv.h>
#include <stdlib.h>

mvGradient:: mvGradient (const char* settings_file) {   
    read_common_mv_setting ("IMG_WIDTH_COMMON", IMG_WIDTH);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", IMG_HEIGHT);
    read_mv_setting (settings_file, "KERNEL_WIDTH", KERNEL_WIDTH);
    read_mv_setting (settings_file, "KERNEL_HEIGHT", KERNEL_HEIGHT);
    read_mv_setting (settings_file, "QUIET", _QUIET_);
    
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
}

mvGradient:: ~mvGradient () {
    cvReleaseImage (&scratch);
    cvReleaseStructuringElement (&kernel);
}

void mvGradient:: filter (const IplImage* img, IplImage* result) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    assert (result != NULL);
    assert (result->nChannels == 1);
        
    cvMorphologyEx (
        img, result, scratch, 
        kernel,
        CV_MOP_GRADIENT,
        1       // iterations 
    );   
}
