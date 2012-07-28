/** mvCircles.cpp  */

#include "mvCircles.h"
#include "mgui.h"
#include <cv.h>

void mvCircles:: drawOntoImage (IplImage* img) {
    assert (img != NULL);
    assert (_data != NULL);
    assert (img->nChannels == 1);
    
    float* circleData;
    for (int i = 0; i < _data->total; i++) {
        circleData = (float*)cvGetSeqElem(_data, i); 
        CvPoint center = cvPoint(circleData[0],circleData[1]); // [0],[1] are x and y of center
        // [2] is radius
        
        cvCircle(img, center, cvRound(circleData[2]), CV_RGB(50,50,50), CIRCLE_THICKNESS);
    }
}


/** mvHoughCircles methods */
mvHoughCircles:: mvHoughCircles (const char* settings_file) {
    read_mv_setting (settings_file, "_ACCUMULATOR_THRESHOLD_", _ACCUMULATOR_THRESHOLD_);
    read_mv_setting (settings_file, "_MIN_CENTER_DIST_", _MIN_CENTER_DIST_);
    read_mv_setting (settings_file, "CANNY_HIGH_THRESHOLD", CANNY_HIGH_THRESHOLD);
    read_mv_setting (settings_file, "PIXEL_RESOLUTION", PIX_RESOLUTION);
}

void mvHoughCircles:: findCircles (IplImage *img, mvCircles* circles) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    assert (circles != NULL);
    assert (circles->_data == NULL); // make sure there isnt already data
    
    unsigned imgwidth = img->width;
    
    circles->_data = cvHoughCircles(
        img, 
        circles->_storage, 
        CV_HOUGH_GRADIENT,      // method
        PIX_RESOLUTION,
        _MIN_CENTER_DIST_ * imgwidth,
        CANNY_HIGH_THRESHOLD,
        _ACCUMULATOR_THRESHOLD_ * imgwidth
    );
}
