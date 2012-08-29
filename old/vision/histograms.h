#ifndef HISTOGRAMS_
#define HISTOGRAMS_

#include <cv.h>
#include <highgui.h>

#define _DISPLAY 1

// PURPOSE: displays a histogram with squares. Intensity of each square is the bin value, 
//      normalized so highest bin = 255.
void displayHistogram2D (CvHistogram* hist, int scale=10); 

// PURPOSE: calculates a Hue-Saturation histogram. Hue is first dimension.
void histogram_HS (IplImage* img, CvHistogram* &hist, int convert_HSV, 
                   int h_bins=90, int s_bins=50, int flags=0); 

#endif
