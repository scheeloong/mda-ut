#ifndef CALCULATIONS_DRAWING_
#define CALCULATIONS_DRAWING_

#include <cv.h>
#include <highgui.h>

#define _display 1

// holds functions to draw stuff and calculate things

// averages the 4 points of a rectangle to find center wrt image center
// rect is 4 elements, contains corner of the rect, offset is the output 
// positon of the center wrt the image center
CvPoint calcRectCenter (CvPoint2D32f* rect, IplImage* img, int origin_center=0);

// for the Lovers Lane thingy, the object will be an L shape. This calculates
// the "center" of the L shape given the 3 points that define it.
// All it does is find the 4th point and call calcRectCenter
CvPoint calcLCenter (CvPoint2D32f* L, IplImage* img, int origin_center=0);

// given a line from cvHoughLines2, finds the perpendicular dist to it from img
// center. Assumes mat is CV_32FC2
// To do this, find the vector from point on line lp to img center. Then dot
// with line's unit direction vector
CvPoint calcLineDist (IplImage* img, float rad, float ang);

// draw lines output by cvHoughLines2. Does so by calculating where line meets
// image edges. Must check to see if it meets x or y edge.
void drawHoughLines (IplImage* img, CvMat* lines, int display=0);

#endif
