#include "cv.h"
#include "highgui.h"
#include <cmath>

#include "calc_draw.h"
#include "preproc_filters.h"

#define DIM_RATIO 2

#define HUE_MIN 35 //30,40
#define HUE_MAX 60 //55
#define SAT_MIN 100 //45

#define GRAD_ROWS 3  // kernal rows for gradient
#define GRAD_COLS 3
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
                  7, _DISPLAY);         // kernal dimension for close operation
    
    cvReleaseImage(&img_Resize);
    printf ("%f\n", fraction);
        
// gradient
    
    IplImage* img_Grad= cvCreateImage ( // create second image with diff size
        cvSize(img_Sat->width, img_Sat->height),
        img_Sat->depth,
        img_Sat->nChannels);;
    
    cvGradient_Custom (img_Sat, img_Grad,
                        GRAD_ROWS, GRAD_COLS, // dims of rectangular kernel
                        1, _DISPLAY);

    cvReleaseImage(&img_Sat);
    
// Hough Line Transform: R = x*cosT + y*sinT gives normal to line from origin. R = rad to line, T = ang of normal  
    CvMat* lines = cvCreateMat (1, 1, CV_32FC2); // two channel 32 bit float

    cvHoughLines2(img_Grad,
        lines,
        CV_HOUGH_STANDARD,
        2, // res in R value in pixels
        CV_PI/64.0, // res in theta in rad
        75, // pixel threshold to make a line
        0,0 // unused        
    );
 /*
    float* dataPtr = (float*)(lines->data.db); 
    float rad = *dataPtr;
    float ang = *(dataPtr+1);
    printf ("%f %f\n", rad, ang);*/
    drawHoughLines (img_Grad, lines, _DISPLAY);
    
    
}
