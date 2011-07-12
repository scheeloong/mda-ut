#include "cv.h"
#include "highgui.h"

#define DIM_RATIO 2
#define B_RATIO 1
#define G_RATIO 1
#define R_RATIO 0

int main( int argc, char** argv ) {
    IplImage* img = cvLoadImage( argv[1]); // load image with name argv[1], ASSUME 3 CHANNEL
    //cvNamedWindow( argv[1], CV_WINDOW_AUTOSIZE );
    //cvShowImage( argv[1], img );

// size down
    IplImage* img_Resize = cvCreateImage ( // create second image with diff size
        cvSize(img->width/DIM_RATIO, img->height/DIM_RATIO),
        img->depth,
        img->nChannels);
    
    cvResize (img, img_Resize, CV_INTER_LINEAR); // resize first img to second
 
    printf ("%d %d %d\n", img_Resize->imageData[100], img_Resize->imageData[101], img_Resize->imageData[102]);
    int x = 240*(4/5)*img_Resize->widthStep + 100*img_Resize->nChannels;
    printf ("%d %d %d\n", img_Resize->imageData[x], img_Resize->imageData[x+1], img_Resize->imageData[x+2]);
// greyscale 
    
    char* rowPtr;
    for (int r = 0; r < img_Resize->height; r++) {
        rowPtr = img_Resize->imageData + r*img_Resize->widthStep; // rowPtr points to beginning of rth row
        for (int c = 0; c < img_Resize->width; c++) {
            *(rowPtr++) *= (float)B_RATIO; // scale blue channel
            *(rowPtr++) *= (float)G_RATIO; // green
            *(rowPtr++) *= (float)R_RATIO; // red
        }
    }
   //cvNamedWindow("Resized", CV_WINDOW_AUTOSIZE );
   //cvShowImage("Resized", img_Resize );
    
    IplImage* img_Grey= cvCreateImage ( // create second image with 1 channel
        cvSize(img_Resize->width, img_Resize->height),
        img_Resize->depth,
        1);
        
    cvConvertImage (img_Resize, img_Grey, 0); // convert to img_Grey
    cvNamedWindow("Grey", CV_WINDOW_AUTOSIZE );
    cvShowImage("Grey", img_Grey );
// smooth
    
// Canny
    

// wait indefinitely for keystroke
    cvWaitKey(0);
// release pointer to an object
    //cvReleaseImage( &img );
    
// Destroy a window
    //cvDestroyWindow( argv[1] );
}