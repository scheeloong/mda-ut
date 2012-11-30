/** mvCircles.cpp  */

#include "mvCircles.h"
#include "mgui.h"
#include "mv.h"
#include "math.h"
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
mvHoughCircles:: mvHoughCircles (const char* settings_file) :
    bin_findcircles ("mvHoughCircles - findCircles")
{
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
    
    bin_findcircles.start();
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
    bin_findcircles.stop();
}

mvAdvancedCircles:: mvAdvancedCircles () : 
    bin_findcircles ("mvAdvancedCircles")
{
    int width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);

    grid_width = width / PIXELS_PER_GRID_POINT;
    grid_height = height / PIXELS_PER_GRID_POINT;
    grid = cvCreateImageHeader (cvSize(grid_width, grid_height), IPL_DEPTH_8U, 1);

    IplImage* temp = mvGetScratchImage2();
    grid->imageData = temp->imageData;
}

mvAdvancedCircles:: ~mvAdvancedCircles () {
    cvReleaseImageHeader(&grid);
    mvReleaseScratchImage2();
}

void mvAdvancedCircles:: findCircles ( IplImage* img) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    assert (img->imageData != grid->imageData);

    bin_findcircles.start();

    /// massively downsample the image
    cvResize (img, grid, CV_INTER_NN);

    /// randomly select 3 points to generate a circle


    /// solve the following system of equations to find the the centerpoint of the circle
    CvPoint p1 = cvPoint(100,100);
    CvPoint p2 = cvPoint(300,100);
    CvPoint p3 = cvPoint(200,250);
    CvPoint center;
    float radius;
    get_circle_from_3_points (p1, p2, p3, center, radius);

    cvCircle (img, p1, 3, CV_RGB(100,100,100));
    cvCircle (img, p2, 3, CV_RGB(100,100,100));
    cvCircle (img, p3, 3, CV_RGB(100,100,100));
    cvCircle (img, center, (int)radius, CV_RGB(200,200,200));

    bin_findcircles.stop();
/*
    char* ptr;
    for (int i = 0; i < grid->height; i++) {
        ptr = (char*) grid->imageData + i*grid->widthStep;
        for (int j = 0; j < grid->width; j++) {
            if (*ptr != 0) 
                printf ("#");
            else 
                printf (".");
            ptr++;
        }
        printf ("\n");
    }
    printf ("\n\n");
    */
}

void mvAdvancedCircles::get_circle_from_3_points (CvPoint p1, CvPoint p2, CvPoint p3, CvPoint &center, float &radius) {
    int x1=p1.x,  y1=p1.y;
    int x2=p2.x,  y2=p2.y;
    int x3=p3.x,  y3=p3.y;

    int DET = (x1-x2)*(y1-y3)-(y1-y2)*(x1-x3);
    int B1 = x1*x1 + y1*y1 - x2*x2 - y2*y2;
    int B2 = x1*x1 + y1*y1 - x3*x3 - y3*y3;

    int X = ( (y1-y3)*B1 + (y2-y1)*B2 ) / 2 / DET;
    int Y = ( (x3-x1)*B1 + (x1-x2)*B2 ) / 2 / DET;

    center = cvPoint(X,Y);
    radius = sqrt((float)((x1-X)*(x1-X) + (y1-Y)*(y1-Y)));
}