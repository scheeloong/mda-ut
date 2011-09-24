#ifndef PREPROCESSING_FILTERS_
#define PREPROCESSING_FILTERS_

#include "cv.h"
#include "highgui.h"

#define _DISPLAY 1
//#define ABS(X) (((X) < 0) ? (-(X)) : (X))

// holds functions that will be used to do basic image processing.
// For example extract object of interest from background, pick out outline of object

// PURPOSE: From a guessed saturation value, Finds the sat value that filters out at least
//      a given fraction (threshold) of the image.
int satThreshold (IplImage* img, int sat_guess=90, 
                  int step=0, float threshold=0.1);

// PURPOSE: converts an image to HSV. All pixels with H in {H_MIN,H_MAX} and S > S_MIN are set to
//      255 and others to 0. Does a close operation to get rid of stray pixels. Returns fraction of
//      high pixels (approximate).
float HueSat_Filter1 (IplImage* img, IplImage* &dst, // source and dest images. Do no allocate dst  
                  int H_MIN, int H_MAX,         // hue min and max
                  unsigned S_MIN, unsigned S_MAX,                   // saturation min and max
                  char flags = 0);              

// PURPOSE: Outputs morphological gradient of image. A simple wrapper of cvMorphologyEx
void cvGradient_Custom (IplImage* img, IplImage* &dst,
                        int KERNEL_W, int KERNEL_H, // dims of rectangular kernel
                        int iterations = 1, // number of times erode and dialate applied. 
                        char flags = 0);
#endif
