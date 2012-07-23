/** mvLines.cpp - contains implementation of mLines class and
 *  mvHoughLines line finding algorithm
 */
#include "mvLines.h"
#include "mgui.h"
#include <cv.h>

#define ABS(x) (((x)>0) ? (x) : (-(x)))

/** mvLines methods */
void mvLines:: removeHoriz () {
    if (data == NULL) return;
    
    CvPoint* temp;
    for (int i = 0; i < data->total; i++) { // for each line
        temp = (CvPoint*)cvGetSeqElem(data, i);
        if (ABS(temp[1].y-temp[0].y) < ABS(temp[1].x-temp[0].x)) {  // horiz line
            cvSeqRemove(data,i); // if it is a horiz line, delete it 
        }
    }
}

void mvLines:: removeVert () {
    if (data == NULL) return;
    
    CvPoint* temp;
    for (int i = 0; i < data->total; i++) { // for each line
        temp = (CvPoint*)cvGetSeqElem(data, i);
        if (ABS(temp[1].y-temp[0].y) > ABS(temp[1].x-temp[0].x)) {  // vert line
            cvSeqRemove(data,i); // if it is a vert line, delete it 
        }
    }
}

void mvLines:: sortXY () { // sort horiz lines by X, vert lines by Y
    if (data == NULL) return;
    
    CvPoint* temp; 
    int swap;
    
    for (int i = 0; i < data->total; i++) { // for each line
        temp = (CvPoint*)cvGetSeqElem(data, i);
        
        if (ABS(temp[1].y-temp[0].y) < ABS(temp[1].x-temp[0].x)) {  // horiz line
            if (temp[0].x > temp[1].x) { // sort so lower X value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
        }   
        else {
            if (temp[0].y > temp[1].y) { // sort so lower Y value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
        }
    }
}

void mvLines:: drawOntoImage (IplImage* img) {
    assert (img != NULL);
    assert (data != NULL);
    assert (img->nChannels == 1);
    
    CvPoint* point;
    for (int i = 0; i < data->total; i++) {
        point = (CvPoint*)cvGetSeqElem(data, i);
        cvLine (img, point[0],point[1], CV_RGB(50,50,50), LINE_THICKNESS);
    }
}

/** mvHoughLines methods */
mvHoughLines:: mvHoughLines (const char* settings_file) {
    read_mv_setting (settings_file, "ACCUMULATOR_THRESHOLD", ACCUMULATOR_THRESHOLD);
    read_mv_setting (settings_file, "MIN_LINE_LENGTH", MIN_LINE_LENGTH);
    read_mv_setting (settings_file, "MIN_COLINEAR_LINE_DIST", MIN_COLINEAR_LINE_DIST);
    read_mv_setting (settings_file, "PIXEL_RESOLUTION", PIX_RESOLUTION);
    read_mv_setting (settings_file, "ANGULAR_RESOLUTION", ANG_RESOLUTION);
}

void mvHoughLines:: findLines (IplImage *img, mvLines* lines) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    assert (lines != NULL);
    assert (lines->data == NULL); // make sure there isnt already data
    
    lines->data = cvHoughLines2 (
        img, 
        lines->storage,
        CV_HOUGH_PROBABILISTIC,
        PIX_RESOLUTION,
        ANG_RESOLUTION,
        ACCUMULATOR_THRESHOLD,
        MIN_LINE_LENGTH,
        MIN_COLINEAR_LINE_DIST
    );
}
