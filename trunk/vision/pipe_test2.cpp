#include "cv.h"
#include "highgui.h"

#define DIM_RATIO 1
#define GRAD_ROWS 5  // kernal rows for gradient
#define GRAD_COLS 5
#define CLOSE_ROWS 3
#define CLOSE_COLS 3

int main( int argc, char** argv ) {
    IplImage* img = cvLoadImage( argv[1], 0); // load image with name argv[1], greyscale
    //cvNamedWindow( argv[1], CV_WINDOW_AUTOSIZE );
    //cvShowImage( argv[1], img );

// size down
    IplImage* img_Resize = cvCreateImage ( // create second image with diff size
        cvSize(img->width/DIM_RATIO, img->height/DIM_RATIO),
        img->depth,
        img->nChannels);
    
    cvResize (img, img_Resize, CV_INTER_LINEAR); // resize first img to second
    cvNamedWindow("Img", CV_WINDOW_AUTOSIZE);
    cvShowImage("Img", img_Resize );
    cvWaitKey(0);
    cvReleaseImage(&img);
   /* 
// greyscale 

    IplImage* img_Grey= cvCreateImage ( // create second image with 1 channel
        cvSize(img_Resize->width, img_Resize->height),
        IPL_DEPTH_8U,
        1);
        
    cvConvertImage (img_Resize, img_Grey, 0); // convert to img_Grey
    cvReleaseImage(&img_Resize);
     
    cvNamedWindow("Grey", CV_WINDOW_AUTOSIZE );
    cvShowImage("Grey", img_Grey );
    */
// gradient
    IplImage* img_Grad = cvCreateImage ( // create second image with 1 channel
        cvSize(img_Resize->width, img_Resize->height),
        IPL_DEPTH_8U,
        1);
    IplImage* temp = cvCreateImage ( // create temp with 1 channel
        cvSize(img_Resize->width, img_Resize->height),
        IPL_DEPTH_8U,
        1);
    
    cvMorphologyEx (img_Resize, //src
        img_Grad, // dst
        temp, // temp
        cvCreateStructuringElementEx (GRAD_ROWS, GRAD_COLS, (GRAD_ROWS+1)/2, (GRAD_COLS+1)/2, CV_SHAPE_RECT),
        CV_MOP_GRADIENT,
        1
    );   
    
    cvReleaseImage (&temp);
    cvShowImage("Img", img_Grad );
    cvWaitKey(0);
    
// threshold
    
    IplImage* img_Thresh = cvCreateImage ( // create second image with 1 channel
        cvSize(img_Grad->width, img_Grad->height),
        IPL_DEPTH_8U,
    1);

    cvThreshold (img_Grad, img_Thresh, 
        20, // threshold
        255, // max_value that pix above thresh are set to
        CV_THRESH_BINARY//CV_THRESH_TOZERO
    ); 
/*
    cvErode (img_Thresh, img_Thresh, 
        NULL,
        1
    );
*/
    cvShowImage("Img", img_Thresh);
    //cvSaveImage("grad_camFrame1.jpg", img_Grad, 0);
    cvWaitKey(0);
    
// Hough Line Transform: R = x*cosT + y*sinT gives normal to line from origin. R = rad to line, T = ang of normal
    
    CvMat* lines = cvCreateMat (3, 1, CV_32FC2); // two channel 32 bit float

    cvHoughLines2(img_Thresh,
        lines,
        CV_HOUGH_STANDARD,
        5, // res in R value in pixels
        CV_PI/8.0, // res in theta in rad
        75, // pixel threshold to make a line
        0,0 // unused        
    );

    float* dataPtr = (float*)(lines->data.db);
    float degs = *(dataPtr+1) * 180.0 / CV_PI;
    if (degs > 90) degs -= 90;    
    printf ("Angle: %f degrees\n", degs);
    
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
// smooth
    /*
    IplImage* grad_Smooth = cvCreateImage ( // create second image with 1 channel
        cvSize(img_Grad->width, img_Grad->height),
        IPL_DEPTH_8U,
        1);
    */
  /*  cvMorphologyEx (img_Grad, //src
        img_Grad, // dst
        NULL, // temp
        cvCreateStructuringElementEx (CLOSE_ROWS, CLOSE_COLS, (CLOSE_ROWS+1)/2, (CLOSE_COLS+1)/2, CV_SHAPE_RECT),
        CV_MOP_CLOSE,
        2
    ); 
    */
    //cvSmooth (img_Grad, img_Grad, CV_GAUSSIAN, 5, 5);
}