#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <cstdio>

#include "calc_draw.h"
#include "preproc_filters.h"
#include "obj_detection.h"

#define ANG_RNG(X) (((X) > 90) ? ((X)-180) : (X))

#define DIM_RATIO 2
#define PIX_COUNT_THRESHOLD 0.01

#define HUE_MIN 30 //30,40
#define HUE_MAX 60 //55
#define SAT_MIN 70 //45
#define SAT_MAX 200

#define OP_DIM 7  // kernal rows for gradient
#define CLOSE_DIM 11

#define NLINES 30

int main( int argc, char** argv ) {
    
    IplImage* img = cvLoadImage( argv[1], CV_LOAD_IMAGE_ANYCOLOR); // load image with name argv[1], color

// size down
    IplImage* img_Resize = cvCreateImage ( // create second image with diff size
        cvSize(img->width/DIM_RATIO, img->height/DIM_RATIO),
        img->depth,
        img->nChannels);
    cvPyrDown (img, img_Resize, CV_GAUSSIAN_5x5); // resize img 1/2
    
    cvNamedWindow("Img", CV_WINDOW_AUTOSIZE);
    cvShowImage("Img", img_Resize );
    cvWaitKey(0);
    cvReleaseImage(&img);
    
// convert to HSV, extract Hue
    IplImage* img_Sat;    
    float fraction = HueSat_Filter1 (img_Resize, img_Sat,       // source and dest images 
                  HUE_MIN, HUE_MAX,     // hue min and max
                  SAT_MIN, SAT_MAX,     // saturation min and max
                  CLOSE_DIM, _DISPLAY);         // kernal dimension for close operation
    
    cvReleaseImage(&img_Resize);
    printf ("%f\n", fraction);
    
    if (fraction < PIX_COUNT_THRESHOLD) {
        printf ("Not Enuf Pipes\n");
        
        CvMoments* moments = 0;
        cvMoments (img_Sat, moments, 1); // third arg = isbinary
        double mass = cvGetSpatialMoment (moments, 0,0);
        double Mx = cvGetSpatialMoment (moments, 1,0) / mass;
        double My = cvGetSpatialMoment (moments, 0,1) / mass;
        
        printf ("%f  %f\n", Mx-img_Sat->width, My-img_Sat->height);
        return 0;
    }

// Probablitic hough lines
    IplImage* img_2 = cvCreateImage ( // create second image with diff size
        cvGetSize(img_Sat),
        img_Sat->depth,
        img_Sat->nChannels);
    cvCopy (img_Sat, img_2);
   // int cnt = cvCountNonZero (img_Sat);
    
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    
    int minlen=50, mindist=30;
    
    lines = cvHoughLines2(img_Sat, storage,
        CV_HOUGH_PROBABILISTIC,
        1, CV_PI/360.0,
        80, minlen, mindist);
    
    CvPoint* temp;
    int swap;
    for (int i = 0; i < NLINES; i++) {
        temp = (CvPoint*)cvGetSeqElem(lines, i);  
        
        if (temp[0].y > temp[1].y) {
            swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
            swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
        }
        
        printf ("%d %d      %d %d\n", temp[0].x,temp[0].y, temp[1].x,temp[1].y);
        cvLine (img_2, temp[0],temp[1], CV_RGB(100,200,100), 1);
    }
    cvShowImage("Img", img_2);
    cvWaitKey(0);
    
    int nseeds=0;
    CvPoint** cseed=0;
    
    addClusterSeed (cseed, nseeds, lines, NLINES);
    addClusterSeed (cseed, nseeds, lines, NLINES);
    addClusterSeed (cseed, nseeds, lines, NLINES);
    KMcluster (cseed, nseeds, lines, NLINES, 1);
    
    for (int i = 0; i < 3; i++) {
        //printf ("%d %d      %d %d\n", cseed[i][0].x,cseed[i][0].y, cseed[i][1].x,cseed[i][1].y);
        cvLine (img_Sat, cseed[i][0],cseed[i][1], CV_RGB(100,200,100), 2);
    }
    
    cvShowImage("Img", img_Sat);
    cvWaitKey(0);
    
return 0;
}
