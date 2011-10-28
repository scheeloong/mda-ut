#include "highgui.h"

int main( int argc, char** argv ) {
// cvLoadImage determines an image type and creates datastructure with appropriate size
    IplImage* img = cvCreateImage(cvSize(1000,500), IPL_DEPTH_8U, 3);
    cvCircle(img, cvPoint(img->width/2, img->height/2), img->height/4, CV_RGB(0,255,255));

// create a window. Window name is determined by a supplied argument
    cvNamedWindow("window_1", CV_WINDOW_AUTOSIZE );
// Display an image inside and window. Window name is determined by a supplied argument
    cvShowImage("window_1", img );
// wait indefinitely for keystroke
    cvWaitKey(0);

// release pointer to an object
    cvReleaseImage( &img );
// Destroy a window
    cvDestroyWindow("window_1");
}