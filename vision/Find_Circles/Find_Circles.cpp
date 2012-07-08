#include "Find_Circles.h"
#include <stdio.h>
using namespace cv;

void Find_Circle(Mat img, vector<Vec3f> results) {
  Mat gray;
  cvtColor( img, gray, CV_BGR2GRAY );
  GaussianBlur( gray, gray, Size(9, 9), 2, 2 );
  HoughCircles( gray, results, CV_HOUGH_GRADIENT, 1, gray.rows/8, 200, 100, 0, 0 );
  ncircles = (int)results.size();
  printf("	Number of circles: %d\n", ncircles );
  // Loop through circles detected, and draw them
  for( int i = 0; i < results.size(); i++ )
  {
      Point pt(cvRound(results[i][0]), cvRound(results[i][1]));
      int radius = cvRound(results[i][2]);
      
      circle( img, pt, radius, Scalar(100,50,100), 3, 8, 0 );
      printf("	Coordinates of circle: (%f, %f)\n", results[i][0], results[i][1]);
      printf("	Radius of circle: %d\n", radius);
   }
  
  /// Show your results
  namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
  imshow( "Hough Circle Transform Demo", img );
  
  waitKey(0);
  //return 0;
}

int main (int argc, char** argv) {
  
  Mat img;
  vector<Vec3f> circles;
  
  /// MAKE SURE YOU LOAD THE IMAGE FIRST BEFORE PASSING IN!!!!!!!!!!!
  img = imread(argv[1], 1);
  if( !img.data )
    { printf("unable to read img\n"); }
  /// here, to read the circle, you just pass in the image read (Mat type) and the "circles" Vec3f vector (to define coordinates and radius)
  Find_Circle(img, circles);
  return 0;
}