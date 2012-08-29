#include "Find_Circles.h"
#include <stdio.h>


void Find_Circles(IplImage* img, CvSeq* results, char* name) {
  
  int buoy_x, buoy_y;
  float radius;
  
  img = cvLoadImage(name);
  
  int edge_thresh = 1;
  IplImage* gray = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
  IplImage* edge = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
  CvMemStorage* storage=cvCreateMemStorage(0);
  
  //show original image
  cvShowImage("original", img);
  cvWaitKey(0);
  
  //convert image colour
  cvCvtColor(img, gray, CV_BGR2GRAY); 
  
  //set the converted image's origin
  gray->origin=0;
  
  //color threshold
  cvThreshold(gray,gray,100,255,CV_THRESH_BINARY); 
  
  //smooth the image to reduce unneccesary results
  cvSmooth( gray, gray, CV_GAUSSIAN, 11, 11 );
  
  //get edges
  cvCanny(gray, edge, (float)edge_thresh, (float)edge_thresh*3, 5);
   
  // show image produced after Canny is applied
  cvShowImage("edge", edge);
  cvWaitKey(0);
  
  //get circles
  results = cvHoughCircles(gray, storage, CV_HOUGH_GRADIENT, 1, 100, 50, 40);
  
  // output all circles detected
  int ncircles=results->total; 
  printf("Number of Circles: %d\n", ncircles);
  // if any circles are found, draw them as you loop through them.
  for(int i = 0; i<ncircles; i++ ){
      float* p = (float*)cvGetSeqElem( results, i );
      cvCircle( img, cvPoint(cvRound(p[0]),cvRound(p[1])), 3, CV_RGB(255,0,0), -1, 8, 0 );
      cvCircle( img, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), CV_RGB(200,0,0), 1, 8, 0 );
      buoy_x = p[0]; buoy_y = p[1];
      printf("Centre: %f, %f. Radius: %f\n", p[0], p[1], p[2]);
  }  
  cvReleaseMemStorage(&storage);
  cvReleaseImage (&gray); 
  cvReleaseImage (&edge);  
}


int main (int argc, char** argv) {
  IplImage* img;
  CvSeq* circles;
  Find_Circles(img, circles, argv[1]);
  return 0;
}