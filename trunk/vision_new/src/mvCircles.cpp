/** mvCircles.cpp  */

#include "mvCircles.h"
#include "mgui.h"
#include <cv.h>

void mvCircles:: drawOntoImage (IplImage* img) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    
    float* circleData;
    for (int i = 0; i < data->total; i++) {
        circleData = (float*)cvGetSeqElem(data, i); 
        CvPoint center = cvPoint(circleData[0],circleData[1]); // [0],[1] are x and y of center
        // [2] is radius
        
        cvCircle(img, center, cvRound(circleData[2]), CV_RGB(50,50,50), CIRCLE_THICKNESS);
    }
}


/** mvHoughCircles methods */
mvHoughCircles:: mvHoughCircles (const char* settings_file) {
    read_mv_setting (settings_file, "ACCUMULATOR_THRESHOLD", ACCUMULATOR_THRESHOLD);
    read_mv_setting (settings_file, "MIN_CENTER_DIST", MIN_CENTER_DIST);
    read_mv_setting (settings_file, "CANNY_HIGH_THRESHOLD", CANNY_HIGH_THRESHOLD);
    read_mv_setting (settings_file, "PIXEL_RESOLUTION", PIX_RESOLUTION);
}

void mvHoughCircles:: findCircles (IplImage *img, mvCircles* circles) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    assert (circles != NULL);
    assert (circles->data == NULL); // make sure there isnt already data
    
    circles->data = cvHoughCircles(
        img, 
        circles->storage, 
        CV_HOUGH_GRADIENT,      // method
        PIX_RESOLUTION,
        MIN_CENTER_DIST,
        CANNY_HIGH_THRESHOLD,
        ACCUMULATOR_THRESHOLD
    );
}