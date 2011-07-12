#include "cv.h"
#include "highgui.h"
#include <cmath>

#include "calc_draw.h"
#include "preproc_filters.h"

#define ANG_RNG(X) (((X) > 90) ? ((X)-180) : (X))

#define DIM_RATIO 2
#define PIX_COUNT_THRESHOLD 0.01

#define HUE_MIN 35 //30,40
#define HUE_MAX 60 //55
#define SAT_MIN 100 //45

#define OP_DIM 7  // kernal rows for gradient
#define CLOSE_DIM 11

int main( int argc, char** argv ) {
    IplImage* img = cvLoadImage( argv[1], CV_LOAD_IMAGE_ANYCOLOR); // load image with name argv[1], color

// size down
    IplImage* img_Resize = cvCreateImage ( // create second image with diff size
        cvSize(img->width/DIM_RATIO, img->height/DIM_RATIO),
        img->depth,
        img->nChannels);
    cvPyrDown (img, img_Resize, CV_GAUSSIAN_5x5); // resize img 1/2
    
    cvNamedWindow("Img", CV_WINDOW_AUTOSIZE);
    cvShowImage("Img", img_Resize );
    cvWaitKey(0);
    cvReleaseImage(&img);
    
// convert to HSV, extract Hue
    IplImage* img_Sat;    
    float fraction = HueSat_Filter1 (img_Resize, img_Sat,       // source and dest images 
                  HUE_MIN, HUE_MAX,     // hue min and max
                  SAT_MIN,              // saturation min and max
                  CLOSE_DIM, _DISPLAY);         // kernal dimension for close operation
    
    cvReleaseImage(&img_Resize);
    printf ("%f\n", fraction);
    
    if (fraction < PIX_COUNT_THRESHOLD) {
        printf ("Not Enuf Pipes\n");
        
        CvMoments* moments;
        cvMoments (img_Sat, moments, 1); // third arg = isbinary
        double mass = cvGetSpatialMoment (moments, 0,0);
        double Mx = cvGetSpatialMoment (moments, 1,0) / mass;
        double My = cvGetSpatialMoment (moments, 0,1) / mass;
        
        printf ("%f  %f\n", Mx-img_Sat->width, My-img_Sat->height);
        return 0;
    }
    
// Hough Line Transform: R = x*cosT + y*sinT gives normal to line from origin. R = rad to line, T = ang of normal  
    CvMat* lines = cvCreateMat (1, 1, CV_32FC2); // two channel 32 bit float

    cvHoughLines2(img_Sat,
        lines,
        CV_HOUGH_STANDARD,
        2, // res in R value in pixels
        CV_PI/64.0, // res in theta in rad
        75, // pixel threshold to make a line
        0,0 // unused        
    );
 
    float* dataPtr = (float*)(lines->data.db); 
    float rad = *dataPtr;
    float ang = *(dataPtr+1);
    CvPoint * temp = calcLineDist (img_Sat, rad, ang);
    printf ("X: %f   Y: %f\nCW Angle from Vertical: %f\n", rad*sin(ang)-img_Sat->width/2, rad*sin(ang)-img_Sat->height/2, 
            ANG_RNG(ang*180/CV_PI));
    drawHoughLines (img_Sat, lines, _DISPLAY);
    return 0;
}
