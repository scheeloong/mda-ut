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

HSV_settings:: HSV_settings () {
    H_MIN = S_MIN  = V_MIN = 0;
    H_MAX = 180;    S_MAX = V_MAX = 255;
}
HSV_settings:: HSV_settings (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax) {
    H_MIN = hmin; H_MAX = hmax; S_MIN = smin; S_MAX = smax; V_MIN = vmin; V_MAX = vmax;
}
void HSV_settings:: setAll (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax) {
    H_MIN = hmin; H_MAX = hmax; S_MIN = smin; S_MAX = smax; V_MIN = vmin; V_MAX = vmax;
}
void HSV_settings:: setHue (int hmin, int hmax) { H_MIN = hmin; H_MAX = hmax; }
void HSV_settings:: setSat (unsigned smin, unsigned smax) { S_MIN = smin; S_MAX = smax; }
void HSV_settings:: setVal (unsigned vmin, unsigned vmax) { V_MIN = vmin; V_MAX = vmax; }
void HSV_settings:: setRange1 () { H_MIN=40; H_MAX=70; S_MIN=125; S_MAX=255; V_MIN=110; V_MAX=255;}

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
float HSV_Filter (IplImage* img, IplImage* &dst, // source and dest images. Do no allocate dst  
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
            if ((*imgPtr >= HSV.H_MIN) && (*imgPtr <= HSV.H_MAX) && 
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
    
    return goodpix / img->width / img->height;
}
/*
float HSV_Filter (IplImage* img, IplImage* &dst, // source and dest images. Do no allocate dst  
                  HSV_settings HSV,
                  char flags)              // kernal dimension for close operation
{
    //assert (img != NULL);
    //assert (img->nChannels == 3);
    
// convert to HSV, extract Hue and Saturation        
    IplImage* img_Hue = cvCreateImage ( // image to store the Hue values
        cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* img_Sat = cvCreateImage ( // image to store the Saturation values
        cvGetSize(img), IPL_DEPTH_8U, 1);
    IplImage* img_Val = cvCreateImage ( // image to store the Saturation values
        cvGetSize(img), IPL_DEPTH_8U, 1);
    
    cvCvtColor (img, img, CV_BGR2HSV); // convert to Hue,Saturation,Value 
    cvSplit (img, img_Hue, img_Sat, img_Val, NULL);   // extract Hue, Saturation
       
// Isolate Color. Set all Pipe pixels to 1 and non-Pipe pixels to 0
    float goodpix = 0; // keeps track of how many good pixels
    // look through both H and S images, pick out pixels where Hue is between H_MIN,H_MAX and Sat > S_MIN
    unsigned char *huePtr, *satPtr, *valPtr;
    for (int r = 0; r < img_Hue->height; r++) {                         
        huePtr = (unsigned char*) (img_Hue->imageData + r*img_Hue->widthStep);
        satPtr = (unsigned char*) (img_Sat->imageData + r*img_Sat->widthStep);
        valPtr = (unsigned char*) (img_Val->imageData + r*img_Val->widthStep);
        for (int c = 0; c < img_Hue->width; c++) {
            // note that saturation values are from 0 to 255 but is interpreted as 
            if ((*huePtr >= HSV.H_MIN) && (*huePtr <= HSV.H_MAX) && 
                (*satPtr >= HSV.S_MIN) && (*satPtr <= HSV.S_MAX) &&
                (*valPtr >= HSV.V_MIN) && (*valPtr <= HSV.V_MAX)) {
                *huePtr = 255;
                goodpix++;
            }
            else *huePtr = 0;
            
            huePtr++; satPtr++; valPtr++;
        }
    }
    
    dst = img_Hue;         
    
    if (flags & _DISPLAY) {
        cvNamedWindow ("Preprocessing_Filter_1", CV_WINDOW_AUTOSIZE);
        cvShowImage("Preprocessing_Filter_1", dst);
        cvWaitKey(0);
        cvDestroyWindow ("Preprocessing_Filter_1");
    }
    cvReleaseImage (&img_Sat);
    cvReleaseImage (&img_Val);
    
    return goodpix / img->width / img->height;
}
*/
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