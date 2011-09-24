#include "cv.h"
#include "highgui.h"
#include <cmath>

#include "calc_draw.h"
#include "preproc_filters.h"
#include "cv_tasks.h"

#define ANG_RNG(X) (((X) > 90) ? ((X)-180) : (X))

#define DIM_RATIO 2
#define PIX_COUNT_THRESHOLD 0.01

#define HUE_MIN 40 //30,40
#define HUE_MAX 70 //55
#define SAT_MIN 70 //45
#define SAT_MAX 150

#define OP_DIM 7  // kernal rows for gradient
#define CLOSE_DIM 11

#define NLINES 30

#define WIN0 "window0"
#define WIN1 "window1"
#define WIN2 "window2"

#define PATH_SKINNYNESS 0.12


int main( int argc, char** argv ) {
    cvNamedWindow(WIN0,1);   // create 3 windows for cv to use
    cvMoveWindow(WIN0, 650, 0);
    cvNamedWindow(WIN1,1);
    cvMoveWindow(WIN1, 1000, 0);
    cvNamedWindow(WIN2,1);
    cvMoveWindow(WIN2, 650, 300);
    
    char* cv_windows[3];
    cv_windows[0]=(char*)malloc(10); cv_windows[1]=(char*)malloc(10); cv_windows[2]=(char*)malloc(10);
    strcpy(cv_windows[0], WIN0); strcpy(cv_windows[1], WIN1); strcpy(cv_windows[2], WIN2);
    
    IplImage* img = cvLoadImage( argv[1], CV_LOAD_IMAGE_ANYCOLOR); // load image with name argv[1], color

// size down
    IplImage* img_Resize = cvCreateImage ( // create second image with diff size
        cvSize(img->width/DIM_RATIO, img->height/DIM_RATIO),
        img->depth,
        img->nChannels);
    cvPyrDown (img, img_Resize, CV_GAUSSIAN_5x5); // resize img 1/2
    
    cvNamedWindow("Img", CV_WINDOW_AUTOSIZE);
    cvShowImage("Img", img_Resize);
    cvWaitKey(0);
    cvReleaseImage(&img);
    
/** HS filter to extract object */
    IplImage* img_1;
    HueSat_Filter1 (img_Resize, img_1, HUE_MIN, HUE_MAX, SAT_MIN, SAT_MAX); // need to delete
    int pix = cvCountNonZero(img_1);
    
/** take gradient of image */    
    cvGradient_Custom (img_1, img_1, 3, 3, 1);
    cvShowImage(cv_windows[1], img_1);
    
/** probabilistic Hough line finder. Determine the threshold using the number of high pixels */
    int thresh = (int)(sqrt(pix/PATH_SKINNYNESS)); // guessed length of pipe in pixels
    CvMemStorage* storage = cvCreateMemStorage(0); // create memstorage for line finidng, delete later
    CvSeq* lines = 0;
    
    int minlen=20, mindist=50;
    lines = cvHoughLines2(img_1, storage,
        CV_HOUGH_PROBABILISTIC,
        2, CV_PI/180.0,
        thresh, minlen, mindist);
/** check if lines were found, if not quit */
    int nlines=lines->total;
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_PATH: No Lines Detected. Exiting.\n");
        return 0;
    }
/** arrange lines by Y value. Will bug if horizontal lines encountered. Assume no horiz lines. */
    CvPoint* temp; int swap;
    for (int i = 0; i < nlines; i++) { // for each line
        temp = (CvPoint*)cvGetSeqElem(lines, i);  
 
        if (ABS(temp[1].y-temp[0].y) < ABS(temp[1].x-temp[0].x)) {  // horiz line
            if (temp[0].x > temp[1].x) { // sort so lower X value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
        }
        else {
            if (temp[0].y > temp[1].y) { // sort so lower Y value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
            //cvLine (img_1, temp[0],temp[1], CV_RGB(100,200,100), 1);
        }
    }

/** perform clustering */
    int nseeds=0;
    CvPoint** cseed=0;
    KMcluster_auto_K (cseed, nseeds, 1,4, lines, nlines, 1);   
// display clustered lines
    for (int i = 0; i < nseeds; i++) 
        cvLine (img_1, cseed[i][0],cseed[i][1], CV_RGB(0,50,50), 2);
    cvShowImage(cv_windows[1], img_1);
    
    cvWaitKey(0);
    return 0;
}
