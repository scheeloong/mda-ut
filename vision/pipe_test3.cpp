#include "cv.h"
#include "highgui.h"
#include <cmath>

#define DIM_RATIO 2

#define HUE_MAX 60
#define HUE_MIN 35
#define SAT_MIN 100

#define GRAD_ROWS 5  // kernal rows for gradient
#define GRAD_COLS 5
#define CLOSE_DIM 5

int main( int argc, char** argv ) {
    IplImage* img = cvLoadImage( argv[1], CV_LOAD_IMAGE_ANYCOLOR); // load image with name argv[1], color

// size down
    
    IplImage* img_Resize = cvCreateImage ( // create second image with diff size
        cvSize(img->width/DIM_RATIO, img->height/DIM_RATIO),
        img->depth,
        img->nChannels);
    /*IplImage* img_Temp = cvCreateImage ( // create second image with diff size
        cvSize(img->width/DIM_RATIO, img->height/DIM_RATIO),
        img->depth,
        img->nChannels);
    cvPyrDown (img, img_Temp, CV_GAUSSIAN_5x5); // resize img 1/2*/
    cvPyrDown (img, img_Resize, CV_GAUSSIAN_5x5); // resize img 1/2
    
    cvNamedWindow("Img", CV_WINDOW_AUTOSIZE);
    cvShowImage("Img", img_Resize );
    cvWaitKey(0);
    cvReleaseImage(&img);
    
// convert to HSV, extract Hue
    IplImage* img_Hue = cvCreateImage ( // image to store the Hue value
        cvSize(img_Resize->width, img_Resize->height),
        IPL_DEPTH_8U,
        1);
    IplImage* img_Sat = cvCreateImage ( // image to store the Hue value
        cvSize(img_Resize->width, img_Resize->height),
        IPL_DEPTH_8U,
        1);
    
    cvCvtColor (img_Resize, img_Resize, CV_BGR2HSV); // convert to Hue,Saturation,Value 
    cvSplit (img_Resize, img_Hue, img_Sat, NULL,NULL);   // extract Hue, Saturation
   
    cvShowImage("Img", img_Sat);
    cvWaitKey(0);
    cvReleaseImage(&img_Resize);
    
// Isolate Color. Set all Pipe pixels to 1 and non-Pipe pixels to 0
    
    char *huePtr, *satPtr;
    for (int r = 0; r < img_Hue->height; r++) {
        huePtr = (char*) (img_Hue->imageData + r*img_Hue->widthStep);
        satPtr = (char*) (img_Sat->imageData + r*img_Sat->widthStep);
        for (int c = 0; c < img_Hue->width; c++) {
            *huePtr = 255 * ((*huePtr >= HUE_MIN) && (*huePtr <= HUE_MAX));//&& (*satPtr+255 >= SAT_MIN));
            huePtr++;
            satPtr++;
        }
    }

// Connect light segments by close
    
    cvMorphologyEx (img_Hue, //src
        img_Hue, // dst
        NULL, // temp
        cvCreateStructuringElementEx (CLOSE_DIM, CLOSE_DIM, (CLOSE_DIM+1)/2, (CLOSE_DIM+1)/2, CV_SHAPE_RECT),
        CV_MOP_CLOSE,
        1
    );
            
    cvShowImage("Img", img_Hue);
    cvWaitKey(0);
    
// gradient
    
    IplImage* img_Grad = cvCreateImage ( // create second image with 1 channel
        cvSize(img_Hue->width, img_Hue->height),
        IPL_DEPTH_8U,
        1);
    IplImage* temp = cvCreateImage ( // create temp with 1 channel
        cvSize(img_Hue->width, img_Hue->height),
        IPL_DEPTH_8U,
        1);
    
    cvMorphologyEx (img_Hue, //src
        img_Grad, // dst
        temp, // temp
        cvCreateStructuringElementEx (GRAD_ROWS, GRAD_COLS, (GRAD_ROWS+1)/2, (GRAD_COLS+1)/2, CV_SHAPE_RECT),
        CV_MOP_GRADIENT,
        1);   

// Hough Line Transform: R = x*cosT + y*sinT gives normal to line from origin. R = rad to line, T = ang of normal
    
    CvMat* lines = cvCreateMat (2, 1, CV_32FC2); // two channel 32 bit float

    cvHoughLines2(img_Grad,
        lines,
        CV_HOUGH_STANDARD,
        5, // res in R value in pixels
        CV_PI/32.0, // res in theta in rad
        75, // pixel threshold to make a line
        0,0 // unused        
    );

    float* dataPtr = (float*)(lines->data.db);
    float rad = (*dataPtr + *(dataPtr+2)) / 2;
    float ang = (*(dataPtr+1) + *(dataPtr+3))/2;

// draw line onto image    
    float x1 = rad / cos(ang);
    float x2 = (rad - img_Grad->height*sin(ang)) / cos(ang);

    if ((x1 < 0) || (x2 > img_Grad->width)) {
        float y1 = rad / sin(ang);
        float y2 = (rad - img_Grad->width*sin(ang)) / cos(ang);
        cvLine(img_Grad, cvPoint (0,int(y1)), cvPoint(img_Grad->width,int(y2)), 
            cvScalar (100,100,0,0));
    } else {
        cvLine(img_Grad, cvPoint (int(x1),0), cvPoint(int(x2),img_Grad->height), 
            cvScalar (100,100,0,0));
    }
    
    printf ("%f %f\n", rad, ang*180/CV_PI);
    //printf ("%d %d\n", img_Grad->width, img_Grad->height);
    //printf ("%f %f\n", x1,x2);
    cvShowImage("Img", img_Grad);
    cvWaitKey(0);
    
    /* // Print all lines
    float *rowPtr;
    for (int r = 0; r < lines->rows; r++) {
        rowPtr = (float*)(lines->data.db + r*lines->step);
        for (int c = 0; c < lines->width; c++) {
            printf ("%f %f\n", *rowPtr, *(rowPtr+1));
            rowPtr += 2;
        }
    }
      */  

}
