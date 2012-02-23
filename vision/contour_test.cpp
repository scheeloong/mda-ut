#include <iostream>
#include <cv.h>
#include <highgui.h>

int main (int argc, char** argv) {
    IplImage* img = cvLoadImage (argv[1], 0); // load greyscale image

    IplImage* temp = cvCreateImage (cvGetSize(img), img->depth, 1); // FindContours uses the input img as scratch space
    cvCopy (img, temp);
    
    CvSeq * contours;
    CvMemStorage* storage = cvCreateMemStorage (0);
    
    int nContours = cvFindContours (
        temp,    // input image, will be scratched
        storage, // storage
        &contours, // output pointer to CvSeq*, root node of contour tree
        sizeof (CvContour), // header size, must be this
        CV_RETR_EXTERNAL, // only retrieve outermost contour
        CV_CHAIN_APPROX_NONE
    );
    
    cout << "NContours: " << nContours;
    
    cvDrawContours(
        temp,       // img to draw on
        contours,   // contour
        cvScalar (100,100,100), // solid color
        cvScalar (0,0,0),       // hole color
        0 // max level. 0 means root level only
    );
        
    cvNamedWindow ("img", 1);
    cvNamedWindow ("contours", 1);
    cvShowImage ("img", img);
    cvShowImage ("contours", temp);
    cvWaitKey(0);    
    return 0;
}