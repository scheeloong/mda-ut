#ifndef _FIND_CIRCLE
#define _FIND_CIRCLE

#include "highgui.h"
#include "cv.h"
#include <iostream>
#include <stdio.h>


using namespace cv;

/* A couple of notes:
 * This is a C++ based circle-finding function that is generally more capable of finding circles than its C counterpart
 * (cvHoughCircles()). It immediately takes in the image (you have to read it first, look in main() of Find_Circles.cpp) and the vector 
 * datatype that represents the circle data structure.
 * What the function does it basically covnert to grayscale and applying a Gaussian blur to reduce noise and detection inaccuracies.
 * It then uses the hough function to find it, and as usual, draws the circles detected.
 * Unlike the traditional cvHoughCircles() function, this works well on detecting circles on coloured images as well.
 * I (Vincent) haven't applied the gradients and morphologies yet, but at least it doesn't find any small (false) circles 
 * within a larger (real) circle...
 */

int	ncircles; // this is initially garbage. However, it is used to count the number of circles	
		  // if used outside the function.

void Find_Circles(IplImage* img, CvSeq* results, char* name);

#endif