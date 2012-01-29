#include "highgui.h"
#include "cv.h"
#include "stdio.h"
#include <iostream>

/* this program finds circles in the image. So far it works for all possible cases given in the return state guide
 * but does not apply to very dark images (blue background and brown circle)
 */
char vision_BUOY1(IplImage *img, int &buoy_x, int &buoy_y, float &radius)
{
  /* return state guide: */
  // 0 = no detection 
  // 1 = buoy, return strong circle with Hough
  // 2 = neither, we could be looking at random stuff
  // 3 = more than one circle. We are supposed to be looking at one
  /** HS filter to extract object seen */
  
  int edge_thresh = 1;
  IplImage* gray = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
  IplImage* edge = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
  CvMemStorage* storage=cvCreateMemStorage(0);
  

  
  //convert image colour
  cvCvtColor(img, gray, CV_BGR2GRAY); 
  
  //set the converted image's origin
  gray->origin=0;
  
  //color threshold
  cvThreshold(gray,gray,50,255,CV_THRESH_BINARY); 
   //show original image
  
  //smooth the image to reduce unneccesary results
  cvSmooth( gray, gray, CV_BLUR, 7, 7 );
  
  cvShowImage("original", gray);
  cvWaitKey(0); 
  //get edges
  //cvCanny(gray, edge, (float)edge_thresh, (float)edge_thresh*3, 5);
   
  // show image produced after Canny is applied
  //cvShowImage("edge", edge);
 // cvWaitKey(0);

  //get circles
  CvSeq* circles = cvHoughCircles(gray, storage, CV_HOUGH_GRADIENT, 1, 100, 50, 40);
  
  // show the image of the circle detected
  //cvShowImage("new", gray);
  //cvWaitKey(0);
  /** check if (a) circle is found, if not...quit */
  // output all circles detected
  int ncircles=circles->total; 
  printf("Number of Circles: %d\n", ncircles);
  if(ncircles < 1) { //first case: no circles
    cvReleaseImage(&gray); cvReleaseMemStorage(&storage);
    printf("  vision_BUOY: No Circles Detected. Exiting.\n");
    return 0; 
  }
  else if(ncircles==1){ //best case: 1 circle correctly detected
      float* p = (float*)cvGetSeqElem( circles, 0 );
      cvCircle( img, cvPoint(cvRound(p[0]),cvRound(p[1])), 3, CV_RGB(255,0,0), -1, 8, 0 );
      cvCircle( img, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), CV_RGB(200,0,0), 1, 8, 0 );
      // update the values buoy_x and buoy_y 
      buoy_x = p[0]; buoy_y = p[1];
      // we have successfully found our circle
      printf("Centre: %f, %f. Radius: %f\n", p[0], p[1], p[2]);
      printf("  vision_BUOY: Successful.\n");
      cvReleaseImage(&gray); cvReleaseMemStorage(&storage);
      return 1; 
    }
  else { //bad case: multiple circles detected
    for(int i = 0; i<ncircles; i++ ){
      float* p = (float*)cvGetSeqElem( circles, i );
      cvCircle( img, cvPoint(cvRound(p[0]),cvRound(p[1])), 3, CV_RGB(255,0,0), -1, 8, 0 );
      cvCircle( img, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), CV_RGB(200,0,0), 1, 8, 0 );
      printf("Centre: %f, %f. Radius: %f\n", p[0], p[1], p[2]);
    }
    printf("	vision_BUOY: Multiple Circles Detected. Exiting.\n");
    cvReleaseImage(&gray); cvReleaseMemStorage(&storage);
    return 3;
  }
  // if neither of the above cases apply, we could be looking at random image, destroy the image anyway
  cvReleaseMemStorage(&storage);
  cvReleaseImage (&gray); 
  cvReleaseImage (&edge);
  return 2;
}

int main(int argc, char **argv) {
    int x, y; // they can initially be garbage...
    float radius; // this can initially be garbage
    IplImage* img = cvLoadImage(argv[1]);
    
    vision_BUOY1(img, x, y, radius);
    return 0;
}