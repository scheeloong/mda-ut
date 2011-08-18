#include <cv.h>
#include <highgui.h>
#include <iostream>

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"

#define N_LINES 20

#define HUE_MIN 60
#define HUE_MAX 70
#define SAT_MIN 90
/*
int find_line_endpoints (IplImage* img, float rad, ang,
                         CvPoint &p1, &p2) {
// given an image and a line overlaid on the image, finds where the lines
// starts and ends by checking each point on the line and seeing if there
// is signal on the image.
    float temp, x[4], y[4];
    int mini, maxi, good1=-1, good2=-1;
    float* dataPtr; 
  
    if (ABS(ang - CV_PI/2.0) < 0.001) { x[0] = -9000;  x[2] = 9000; }
    else {
        x[0] = rad / cos(ang);  y[0] = 0; // intercept with y=0 line
        x[2] = (rad-img->height*sin(ang)) / cos(ang);  y[2] = img->height; // y = max(y) line
    }
        
    if (ABS(ang) < 0.001) { x[1] = -9000;  x[3] = 9000; }
    else {
        x[1] = 0;  y[1] = rad / sin(ang);  // x = 0 line
        x[3] = img->width;  y[3] = (rad-img->width*cos(ang)) / sin(ang); // x = max(x) line
    }

    // find the indices of the min and max x elements
    mini = maxi = 0;
    for (int i = 1; i < 4; i++) {
        if (x[mini] > x[i]) mini = i;
        else if (x[maxi] < x[i]) maxi = i;
    }
    for (int i = 0; i < 4; i++)
        if (i != mini && i != maxi)
            if (good1 == -1) good1 = i;
            else good2 = i;
*/
        
        

void vision_GATE (IplImage* img, 
                  int &gateX, int &gateY,
                  char flags=0)
{
    // HS filter to extract gate object
    IplImage* img_Pipe;
    HueSat_Filter1 (img, img_Pipe, 
                    HUE_MIN, HUE_MAX, SAT_MIN,
                    5, _DISPLAY);
    cvReleaseImage(&img);
    
    // look for line segements
    CvMat* lines = cvCreateMat (N_LINES, 1, CV_32FC2);
    float *datPtr = (float*) (lines->data.ptr);    
    for (int i = 0; i < N_LINES*2; i++)       // give ang/rad ridiculous values
        *datPtr++ = 9000;
    
    cvHoughLines2(img_Pipe, lines,
        CV_HOUGH_STANDARD,
        2,              // radius resolution in pixels
        CV_PI/32.0,     // angle resolution in radians
        100);            // point threshold for line
        
 /*   
    datPtr = (float*) (lines->data.ptr);
    for (int i = 0; i < N_LINES; i++) {
        printf ("%f %f\n", *datPtr, *(datPtr+1));
        datPtr += 2;
    }
   */ 
// LINES_BINNING
    CvMat* binned;    
    int rbounds[2] = {0, img_Pipe->width*1.5};
    float abounds[2] = {-CV_PI/4, CV_PI/4};
    lines_binning (lines, N_LINES, binned, 2, // input line matrix, nlines, output line mat 
                       rbounds, abounds, // min/max bound for rad and ang
                       4, 4);         // bin resolution in rad and ang

    if (flags & 1) {
        IplImage* img_Lines = cvCreateImage ( // temp img for display
            cvGetSize(img_Pipe), img_Pipe->depth, 1);        
        
        cvCopy (img_Pipe, img_Lines);       // copy img     
        drawHoughLines (img_Lines, lines);  // draw lines on img
        cvNamedWindow ("window1",CV_WINDOW_AUTOSIZE);
        cvShowImage ("window1", img_Lines);
        
        cvCopy (img_Pipe, img_Lines);       // copy img     
        drawHoughLines (img_Lines, binned);  // draw lines on img
        cvNamedWindow ("window2",CV_WINDOW_AUTOSIZE);
        cvShowImage ("window2", img_Lines);
        cvWaitKey(0);
    }
}
    
int main( int argc, char** argv ) {
    IplImage* img = cvLoadImage( argv[1], CV_LOAD_IMAGE_ANYCOLOR); // load image with name argv[1], color
    
// convert to HSV, extract Hue and Saturation
    int gateX, gateY;
    vision_GATE (img,gateX,gateY,1);    
    
    /*    
    drawHoughLines (img_Temp, linef, _DISPLAY); 
    cvWaitKey (0);*/

}
