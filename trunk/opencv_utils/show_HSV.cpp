#include <stdio.h>
#include "cv.h"
#include "highgui.h"

void callBack (int event, int x, int y, int flags, void* param) {
// param must be the IplImage* pointer, with HSV color space    
    IplImage* img = (IplImage*) param;
    unsigned char * imgPtr;
    
    if (event == CV_EVENT_LBUTTONDOWN) {
        // print the HSV values at x,y
        cvCvtColor (img, img, CV_BGR2HSV);
        imgPtr = (unsigned char*) img->imageData + y*img->widthStep + x*img->nChannels;
        printf ("(%d,%d):  %u  %u  %u\n", x,y,imgPtr[0],imgPtr[1],imgPtr[2]);
        cvCvtColor (img, img, CV_HSV2BGR);
    }
}

int main (int argc, char** argv) {
    if (argc != 2) {
        printf ("Indicate name of image as argument\n");
        return 1;
    } 
    else 
        printf ("Click to Print HSV values. 'q' to Exit\n");
    
    IplImage* img = cvLoadImage (argv[1]);
    cvNamedWindow ("Image");
    cvShowImage ("Image", img);
    
    cvSetMouseCallback ("Image", callBack, img);
    
    char key;
    for (key = 0; key != 'q';) 
        key = cvWaitKey(5);
    
    cvDestroyWindow ("Image");
    cvReleaseImage (&img);
    return 0;
}