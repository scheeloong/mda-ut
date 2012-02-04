#include <stdio.h>
#include "common.h"
#include "preproc_filters.h"

// holds functions that will be used to do basic image processing.
// For example extract object of interest from background, pick out outline of object

// NAME: satThreshold (IplImage* img, int sat_guess, 
//                     int step = 5, float threshold = 0.1)
//
// PURPOSE: From a guessed saturation value, Finds the sat value that filters out at least
//      a given fraction (threshold) of the image.
//
// ARGUMENTS:
//      img: 3 channel image, HSV type
//      sat_guess: guessed saturation value
//      step: determines how much the saturation value changes per iteration. The changes
//            could be larger than step but not smaller. 
//      threshold: desired fraction of good / total pixels.
// 
// METHOD: calculates fraction of pixels accepted by sat_guess. If larger than threshold, then
//      iteratively increase sat_guess until accept_fraction is less than threshold. Do one 
//      final check to make sure not too few pixels are accepted.
/*int satThreshold (IplImage* img, int sat_guess, 
                  int step, float threshold) {
    //assert (img != NULL);
    //assert (img->nChannels == 3);

    int pixCounter, adjStep;
    float pixFraction;
    char *satPtr;
            
    for (;;) { // step thru 1 out of every 4 pixels, calculate fraction of accepted pixels
        pixCounter = 0;
        for (int r = 0; r < img->height/2; r++) {                         
            satPtr = (char*) (img->imageData + 2*r*img->widthStep + 1); 
            for (int c = 0; c < img->width/2; c++) {
                pixCounter += (*satPtr >= sat_guess);
                satPtr += 2*img->nChannels;
            }
        }
        pixFraction = 4*(float) pixCounter / img->width / img->height; 
        //printf ("%f\n", pixFraction);
        
        //calculate how much to change sat_guess
        adjStep = (int)(pixFraction-threshold) * 5 * step; // adjStep > step if diff > 0.2
        if (pixFraction > threshold) 
            sat_guess += ((adjStep > step) ? adjStep : step); // add larger of step and adjstep
        else break;
    }     
    
    if (pixFraction < threshold/5.0) // if pixFraction too small subtract step and break
        sat_guess -= step;
    
    return sat_guess;
}*/

// NAME: HueSat_Filter1
//
// PURPOSE: converts an image to HSV. All pixels with H in {H_MIN,H_MAX} and S in {S_MIN,S_MAX} are set to
//      255 and others to 0. Does a close operation to get rid of stray pixels. Returns fraction of
//      high pixels (approximate).
//      
// ARGUMENTS:
//      img - source img, must be 8 bit, 3 channel, and in BGR.
//      dst - destination img. Is 8 bit and 1 channel. All pixels either 0 or 255
//      H_MIN,H_MAX - min and max allowed hue values. Hues between these will be accepted
//      S_MIN, S_MAX - min and max allowed saturation
//      V_MIN, V_MAX - min and max allowed value
//      CLOSE_DIM - kernel size of the close operation
//      flags - 8 bitflags to pass in options. Setting bit 1 makes it display the dst image.
//
// METHOD: Uses cvCvtColor to convert to HSV mode, cvSplit to get the Hue and Sat planes, 
//      then steps thru each pixel to see which are acceptable. Then performs a CLOSE operation
//      with cvMorphologyEx to get rid of stray pixels if CLOSE_DIM > 0.
//
float HSV_filter (IplImage* img, IplImage* &dst, // source and dest images. Do no allocate dst  
                  HSV_settings HSV,
                  char flags)              // kernal dimension for close operation
{
// convert to HSV, extract Hue and Saturation        
    cvCvtColor (img, img, CV_BGR2HSV); // convert to Hue,Saturation,Value 
// create new 1 channel image
    dst = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);  // image to return
// Set all pixels with HSV values within the target range to 1 and non target pixels to 0
    float goodpix = 0; // keeps track of how many good pixels
    // look through both H and S images, pick out pixels where Hue is between H_MIN,H_MAX and Sat > S_MIN
    unsigned char *imgPtr, *dstPtr;
    for (int r = 0; r < img->height; r++) {                         
        imgPtr = (unsigned char*) (img->imageData + r*img->widthStep);
        dstPtr = (unsigned char*) (dst->imageData + r*dst->widthStep);
        for (int c = 0; c < img->width; c++) {
            // note that saturation values are from 0 to 255 but is interpreted as 
            if ( HSV.HueInRange (*imgPtr) && 
                (*(imgPtr+1) >= HSV.S_MIN) && (*(imgPtr+1) <= HSV.S_MAX) &&
                (*(imgPtr+2) >= HSV.V_MIN) && (*(imgPtr+2) <= HSV.V_MAX)) {
                *dstPtr = 255;
                goodpix++;
            }
            else *dstPtr = 0;
            
            imgPtr+=3; dstPtr++;
        }
    }

    if (flags & _DISPLAY) {
        cvNamedWindow ("HSV_Filter", CV_WINDOW_AUTOSIZE);
        cvShowImage("HSV_Filter", dst);
        cvWaitKey(0);
        cvDestroyWindow ("HSV_Filter");
    }
    
    return goodpix / (img->width * img->height);
}


// Generates histogram of all the hue values of pixels in the image. Ignores pixels whose S,V
// are outside the SV range in HSV variable. Uses histogram to estimate true hue hue range.
// Algorithm first finds a significant local maximum around the estimated hue value. 
// 
//      img = input image
//      dst = output binary image after HSV filtering. Will only be valid if flag 8 is set
//      HSV = HSV settings. Hue values are averaged to get the estimate
//      hueStep = bin size for histogram
//      flags: 1=display, 8=calculate filtered image.
float HSV_adjust_filter (IplImage* img, IplImage* &dst, 
                         HSV_settings &HSV, 
                         int h_bins, char flags) 
{
    /** extract hue plane. Mark all invalid pixels as Hue=255, which is outside histogram range */
    // convert to HSV       
    cvCvtColor (img, img, CV_BGR2HSV); // convert to Hue,Saturation,Value 
    IplImage* hueImg = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1); // will be converted to dst later if successful
    
    unsigned char *imgPtr, *huePtr;
    for (int i = 0; i < img->height; i++) {
        imgPtr = (unsigned char*) img->imageData + i*img->widthStep;
        huePtr = (unsigned char*) hueImg->imageData + i*hueImg->widthStep;
        
        for (int j = 0; j < img->width; j++) {
            if ((*(imgPtr+1) >= HSV.S_MIN) && (*(imgPtr+1) <= HSV.S_MAX) &&
                (*(imgPtr+2) >= HSV.V_MIN) && (*(imgPtr+2) <= HSV.V_MAX)) 
                *huePtr = *imgPtr;
            else
                *huePtr = 255;
            huePtr++; imgPtr+=3;
        }
    }
    
    /** genereate hue histogram */ 
    int size[] = {h_bins};  // constants required for histogram functions
    float h_range[] = {0, 180};  float* ranges[] = {h_range};
    IplImage* planes[] = {hueImg};
    
    CvHistogram* hist = cvCreateHist (1, //dims
        size, // num of bins per dims
        CV_HIST_ARRAY, // dense matrix
        ranges, // upper & lower bound for bins
        1); // uniform
    
    cvCalcHist (planes, hist,
        0,  // accumulate
        NULL); // possible boolean mask image
    cvNormalizeHist (hist, 100); // normalize so total count is 100
       
    /** find nearby local max, exit if fail to find */
    //int Hguess = (HSV.H_MIN + HSV.H_MAX)/2; // estimated hue
    int srch_min = HSV.H_MIN-20, srch_max = HSV.H_MAX+20; // search estimate +- 20
    if (srch_min < 0) srch_min += 180;
    if (srch_max > 179) srch_max -= 180;
    // the min and max bins to search
    int bin_min = srch_min*h_bins / 180.0, bin_max = srch_max*h_bins / 180.0; 
    
    // now find the local maximum
    int temp, LM=-1, LMbin; // LMbin = local max bin
    for (int i = bin_min; i <= bin_max; i++) {
        if (i == 180) i = 0; // this line allows looping from 170-20, for example
            
        temp = cvQueryHistValue_1D (hist, i);
        if (temp > LM) {
            LM = temp;
            LMbin = i;
        }
    }
    
    /** mark bins neighbouring the max until integral of marked bins >> those of nearby unmarked bins */
    bin_min = bin_max = LMbin; // bin_min and bin_max now refer to the range of "marked" bins
    // a marked bin means the hue values inside is part of the target
    int marked = 1; 
    for (; marked <= h_bins/6-1; marked++) { // h_bins / 6  =  30 / (180/h_bins)
        int e_marked=0, e_unmarked=0;
        // calculate energy of marked bins 
         for (int i = bin_min; i <= bin_max; i++) {
             if (i == 180) i = 0;
             e_marked += cvQueryHistValue_1D (hist, i);
         }
         e_marked /= marked; // average of the integral.
         
         // caculate integral of neighbouring unmakred bins (those +- 2 from marked bins)
         int temp, min_1, max_1;
         temp = bin_min-1;   if (temp < 0) temp += 180;
          min_1 = cvQueryHistValue_1D (hist, temp);   e_unmarked += min_1;
         temp = bin_max+1;   if (temp >= 180) temp -= 180;
          max_1 = cvQueryHistValue_1D (hist, temp);   e_unmarked += max_1;
         /*temp = bin_min-2;   if (temp < 0) temp += 180;
          e_unmarked += cvQueryHistValue_1D (hist, temp);
         temp = bin_max+2;   if (temp >= 180) temp -= 180;
          e_unmarked += cvQueryHistValue_1D (hist, temp);
         */e_unmarked /= 2;
         
         // if integral of marked bins >> integral of unmarked
         if ((e_marked > e_unmarked*4) && (e_marked > e_unmarked + 10)) {
             marked = -1;
             break;
         }
         // else mark one more bin
         if (min_1 > max_1)
             bin_min = (bin_min-1 >= 0) ? (bin_min-1) : 179;
         else
             bin_max = (bin_max+1 < 180) ? (bin_max+1) : 0;
    }
    
    /** if successful, generate dst image */
    int goodPix=0;  float good_fraction;
    if (marked < 0) {// successful
        HSV.H_MIN = bin_min * 180 / h_bins;
        HSV.H_MAX = (bin_max+1) * 180 / h_bins;
    
        int goodpix=0;
        for (int i = 0; i < img->height; i++) {
            huePtr = (unsigned char*) hueImg->imageData + i*hueImg->widthStep;
            for (int j = 0; j < img->width; j++) {
                if ((*huePtr != 255) && HSV.HueInRange (*huePtr))
                    *huePtr = 255;
                else {                    
                    *huePtr = 0;
                    goodPix++;
                }
                huePtr++; 
            }
        }
        good_fraction = goodpix / (img->imageSize);
        dst = hueImg;
    }
    else { // not enough of a peak => object too faint
        cvReleaseHist (&hist);
        cvReleaseImage (&hueImg);
        return -1;
    }
    
    if (flags & _DISPLAY) {
        printf ("GoodPix Fraction: %f\n", good_fraction);
        for (int i = 0; i < h_bins; i++) {
            int val = cvQueryHistValue_1D (hist, i);
            printf ("Bin %2.1d (%3.1d-%3.1d): %d\n", i, 180/h_bins*i, 180/h_bins*(i+1)-1, val);    
        }
        printf ("Bin Range: %d-%d\n", bin_min, bin_max);
        printf ("Hue Range: %d-%d\n", HSV.H_MIN, HSV.H_MAX);
    }
    
    cvReleaseHist (&hist);
    //cvReleaseImage (&hueImg);
    return good_fraction;
}


// NAME: cvGradient_Custom
//
// PURPOSE: Outputs morphological gradient of image. A simple wrapper of cvMorphologyEx
//
// ARGUMENTS:
//      img - source image. Must be 1 channel.
//      dst - destination. Also 1 channel 8 bit. Can be in place.
//      KERNEL_H,KERNEL_W - dimensions of the gradient kernel. Bigger = slower and more accurate
//      iterations - I still dont know why you'd want more than 1.
//      flags - 8 bitflags. bit1 = displays dst.
//
// METHOD: Just a wrapper for cvMorphologyEx set to gradient mode. 
//      
void cvGradient_Custom (IplImage* img, IplImage* &dst,
                        int KERNEL_W, int KERNEL_H, // dims of rectangular kernel
                        int iterations, // number of times erode and dialate applied. 
                        char flags)
// Name: Custom Gradient
// Outputs gradient of input image. Expects 1 channel image. A wrapper for cvMorphologyEx
{
    //assert (img != NULL);
    //assert (img->nChannels == 1);
    
    dst = cvCreateImage ( // create second image with 1 channel
        cvGetSize(img), IPL_DEPTH_8U, 1);
    dst->origin = img->origin;
    IplImage* temp = cvCreateImage ( // create temp with 1 channel
        cvGetSize(img), IPL_DEPTH_8U, 1);
    
    cvMorphologyEx (img, dst, temp, 
        cvCreateStructuringElementEx (KERNEL_W, KERNEL_H, (KERNEL_W+1)/2, (KERNEL_H+1)/2, CV_SHAPE_RECT),
        CV_MOP_GRADIENT,
        iterations);   
    
    if (flags & _DISPLAY) {
        cvNamedWindow ("cvGradient_Custom", CV_WINDOW_AUTOSIZE);
        cvShowImage("cvGradient_Custom", dst);
        cvWaitKey(0);
        cvDestroyWindow ("cvGradient_Custom");
    }
    cvReleaseImage (&temp);
}
