#include "cv.h"
#include "highgui.h"
#include "fstream"

#define FILENAME "pix_vert.txt"
#define COL 200
#define COL2 250
#define COL3 300

int main( int argc, char** argv ) {
    IplImage* img = cvLoadImage( argv[1], 
        (CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH));
    // load image with name argv[1]
    //cvNamedWindow( argv[1], CV_WINDOW_AUTOSIZE );
    //cvShowImage( argv[1], img );
    
    IplImage* img_HSV = cvCreateImage ( // create second image with diff size
        cvSize(img->width, img->height),
        IPL_DEPTH_8U,
        img->nChannels);
    
    cvCvtColor (img, img, CV_BGR2HSV);

    FILE* fp = fopen (FILENAME, "w");
// greyscale 
    
    char* rowPtr;
    for (int r = 0; r < img->height; r++) {
        // points to rth rol and COL column
        rowPtr = img->imageData + r*img->widthStep + COL*img->nChannels; 
        for (int ch = 0; ch < img->nChannels; ch++) // print each channel
            fprintf (fp, "%d ", *rowPtr++);
        fprintf (fp, "   ");
        
        rowPtr = img->imageData + r*img->widthStep + COL2*img->nChannels;
        for (int ch = 0; ch < img->nChannels; ch++) 
            fprintf (fp, "%d ", *rowPtr++);
        fprintf (fp, "   ");
        
        rowPtr = img->imageData + r*img->widthStep + COL3*img->nChannels;
        for (int ch = 0; ch < img->nChannels; ch++) 
            fprintf (fp, "%d ", *rowPtr++);
        fprintf (fp, "   ");
        
        fprintf (fp, "\n");
    }
    return 0;
}
