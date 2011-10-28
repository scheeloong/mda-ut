#include <highgui.h>
#include <cv.h>
#include <iostream>
#include "preproc_filters.h"

char vision_BUOY1(IplImage *img, int &buoy_x, int &buoy_y, float &radius)
{
  /* return state guide: */
  // 0 = no detection 
  // 1 = buoy, return strong circle with Hough
  // 2 = neither, we could be looking at random stuff
  // 3 = more than one circle. We are supposed to be looking at one
  /** HS filter to extract object seen */
  IplImage* img_grad = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
  IplImage* img_1 = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
  cvConvertImage (img, img_1);
  cvConvertImage (img_1, img_grad);
  cvCanny (img_1, img_grad, 50, 25);
  
  
  //HueSat_Filter1 (img, img_grad, 70,140, 50, 255);
  
  cvShowImage("img_1", img_grad);
  
  cvWaitKey(0);
  
  /** Use cvHoughCircles(). Determine the threshold using the number of high pixels */
  //int thresh = (int)(sqrt(pix/BUOY_SKINNYNESS)); //not sure if this is right
  CvMemStorage* storage=cvCreateMemStorage(0);
  //IplImage* img_grad = cvCreateImage (cvGetSize(img_1), IPL_DEPTH_8U, 1);
  
  
  CvSeq* circles = 0;
  
  circles = cvHoughCircles(img_grad, storage, CV_HOUGH_GRADIENT,
			   2, //  resolution in center pos
			   200, // mindist
			   50, // canny high threshold
			   120.0 ); // accumulator threshold
  /** check if (a) circle is found, if not...quit */
  int ncircles=circles->total; 
  printf("Number of Circles: %d\n", ncircles);
  if(ncircles < 0) {
    cvReleaseImage(&img_grad); cvReleaseMemStorage(&storage);
    printf("  vision_BUOY: No Circles Detected. Exiting.\n");
    return 0; }
  /*else if(ncircles >= 1) {
    // loop through all found circles and display them
    for(int i = 0; i<ncircles; i++) {
      cvShowImage("img_grad", img_grad);
      cvWaitKey(0);
    }
    for(int i=0; i<ncircles; i++) {
      cvCircle(img_grad
    }
    cvReleaseImage(&img_grad); cvReleaseMemStorage(&storage);
    printf("   vision_BUOY: Multiple Circles Detected. Exiting.\n");
    return 3; }*/
  else {
    float* p=(float*)cvGetSeqElem(circles, 0);
    for (int i = 0; i<ncircles; i++)
    {
      printf ("X: %f   Y: %f  R: %f\n", p[0],p[1],p[2]);
      CvPoint pt = cvPoint(p[0],p[1]);    
      cvCircle(img_grad, pt, /* draw the small circle */
	cvRound(p[2]),		  /* on the detected centre */
	CV_RGB(100, 200, 100), 2, 8);
    }
    
    if(ncircles >= 1) {
      printf("	vision_BUOY: Multiple Circles Detected. Exiting.\n");
      cvShowImage("img_grad", img_grad); cvWaitKey(0);
      cvReleaseImage(&img_grad); cvReleaseMemStorage(&storage);
      return 3;
    }
    else if(ncircles == 1) {
      printf("Centre: %f, %f. Radius: %f\n", p[0], p[1], p[2]);
      printf("  vision_BUOY: Successful.\n");
      cvReleaseImage(&img_grad); cvReleaseMemStorage(&storage);
      return 1; // this is the best case
    }
  }
  // if neither of the above cases apply, we could be looking at random image, destroy the image anyway
  cvReleaseMemStorage(&storage);
  cvReleaseImage (&img_1); 
  cvReleaseImage (&img_grad);
  return 2;
}
int main(int argc, char **argv) {
    int x, y; // they can initially be garbage...
    float radius; // this can initially be garbage
    IplImage* img = cvLoadImage(argv[1]);
    
    vision_BUOY1(img, x, y, radius);
    return 0;
}
