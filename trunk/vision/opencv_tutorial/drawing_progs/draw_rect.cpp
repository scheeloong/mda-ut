#include "highgui.h"
#define RECT_COLOR 2


int main( int argc, char** argv ) {
// cvLoadImage determines an image type and creates datastructure with appropriate size
    IplImage* img = cvCreateImage(cvSize(1000,500), IPL_DEPTH_8U, 4);
    
    char* ptr;
    for (int r = 0; r < img->height; r++) {
        ptr = img->imageData + r*img->widthStep; // point to first pixel of rth row
        for (int c = 0; c < img->width; c++)
            *(ptr+c*img->nChannels+RECT_COLOR) = 255; // make the right color channel 255
    }

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