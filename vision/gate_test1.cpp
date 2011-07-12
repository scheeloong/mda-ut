#include "cv.h"
#include "highgui.h"

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"

#define DIM_RATIO 2

#define HUE_MAX 70
#define HUE_MIN 60
#define SAT_MIN 90

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
    
// convert to HSV, extract Hue and Saturation
    IplImage* img_Pipe;
    HueSat_Filter1 (img_Resize, img_Pipe, 
                    HUE_MIN, HUE_MAX, SAT_MIN,
                    11, _DISPLAY);
    cvReleaseImage(&img_Resize);

// look for line segements
    /*
    CvPoint* endPoints;
    detect_LLane (img_Pipe, endPoints);
    
    printf ("%d %d\n", endPoints[1].x, endPoints[1].y);
    printf ("%d %d\n", endPoints[2].x, endPoints[2].y);
    printf ("%d %d\n", endPoints[3].x, endPoints[3].y);
    */
    
// corner detector
    CvPoint2D32f* corners;
    cvCornerST_Custom (img_Pipe, NULL, corners);
    
    CvPoint center = calcRectCenter (corners, img_Pipe, 0); 
    printf ("%d %d\n", center.x, center.y);
    cvCircle (img_Pipe, center, 5, CV_RGB(100,200,100), -1);
    
    for (int i = 0; i < 4; i++) {
        printf ("%f %f\n", corners[i].x, corners[i].y);    
        cvCircle (img_Pipe, cvPoint(corners[i].x,corners[i].y), 3, CV_RGB(100,100,100), -1);
    }
    
    cvShowImage("Img", img_Pipe );
    cvWaitKey(0);
// gradient
    /*
    IplImage* img_Grad; 
    cvGradient_Custom (img_Pipe, img_Grad, 
                       5, 5, // kernel size
                       1, 1); // iterations, display
    cvReleaseImage(&img_Pipe);
    */
    
}