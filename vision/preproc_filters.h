#ifndef PREPROCESSING_FILTERS_
#define PREPROCESSING_FILTERS_

#define _DISPLAY 0x1
#define MODULAR_255(X) (((X) < 0) ? (-X) : (X))

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
int satThreshold (IplImage* img, int sat_guess=90, 
                  int step=3, float threshold=0.1) {
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
}

// NAME: HueSat_Filter1
//
// PURPOSE: converts an image to HSV. All pixels with H in {H_MIN,H_MAX} and S > S_MIN are set to
//      255 and others to 0. Does a close operation to get rid of stray pixels. Returns fraction of
//      high pixels (approximate).
//      
// ARGUMENTS:
//      img - source img, must be 8 bit, 3 channel, and in BGR.
//      dst - destination img. Is 8 bit and 1 channel. All pixels either 0 or 255
//      H_MIN,H_MAX - range of allowed hue values
//      S_MIN - minimum allowed saturation
//      CLOSE_DIM - kernel size of the close operation
//      flags - 8 bitflags to pass in options. Setting bit 1 makes it display the dst image.
//
// METHOD: Uses cvCvtColor to convert to HSV mode, cvSplit to get the Hue and Sat planes, 
//      then steps thru each pixel to see which are acceptable. Then performs a CLOSE operation
//      with cvMorphologyEx to get rid of stray pixels.
//
// EXAMPLE: See gate_test1.cpp
float HueSat_Filter1 (IplImage* img, IplImage* &dst, // source and dest images. Do no allocate dst  
                  int H_MIN, int H_MAX,         // hue min and max
                  int S_MIN,                    // saturation min and max
                  int CLOSE_DIM = 3,
                  int flags = 0)              // kernal dimension for close operation
{
    //assert (img != NULL);
    //assert (img->nChannels == 3);
    
// convert to HSV, extract Hue and Saturation        
    IplImage* img_Hue = cvCreateImage ( // image to store the Hue values
        cvSize(img->width, img->height),
        IPL_DEPTH_8U,
        1);
    IplImage* img_Sat = cvCreateImage ( // image to store the Saturation values
        cvSize(img->width, img->height),
        IPL_DEPTH_8U,
        1);
    
    cvCvtColor (img, img, CV_BGR2HSV); // convert to Hue,Saturation,Value 
    
    cvSplit (img, img_Hue, img_Sat, NULL,NULL);   // extract Hue, Saturation
       
// Isolate Color. Set all Pipe pixels to 1 and non-Pipe pixels to 0
    float goodpix = 0; // keeps track of how many good pixels
    // look through both H and S images, pick out pixels where Hue is between H_MIN,H_MAX and Sat > S_MIN
    char *huePtr, *satPtr;
    for (int r = 0; r < img_Hue->height; r++) {                         
        huePtr = (char*) (img_Hue->imageData + r*img_Hue->widthStep);
        satPtr = (char*) (img_Sat->imageData + r*img_Sat->widthStep);
        for (int c = 0; c < img_Hue->width; c++) {
            // note that saturation values are from 0 to 255 but is interpreted as 
            if ((*huePtr >= H_MIN) && (*huePtr <= H_MAX) && (MODULAR_255(*satPtr) >= S_MIN)) {
                *huePtr = 255;
                goodpix++;
            }
            else *huePtr = 0;
            
            huePtr++;
            satPtr++;
        }
    }
    
    dst = cvCreateImage ( // allocate dst image
        cvSize(img->width, img->height),
        IPL_DEPTH_8U, 1);

    // close
    cvMorphologyEx (img_Hue, //src
        dst, // dst
        NULL, // temp
        cvCreateStructuringElementEx (CLOSE_DIM, CLOSE_DIM, (CLOSE_DIM+1)/2, (CLOSE_DIM+1)/2, CV_SHAPE_RECT),
        CV_MOP_CLOSE, 1
    );
    
    if (flags & _DISPLAY) {
        cvNamedWindow ("Preprocessing_Filter_1", CV_WINDOW_AUTOSIZE);
        cvShowImage("Preprocessing_Filter_1", dst);
        cvWaitKey(0);
        cvDestroyWindow ("Preprocessing_Filter_1");
    }
    cvReleaseImage (&img_Hue);
    cvReleaseImage (&img_Sat);
    
    return goodpix / img->width / img->height;
}

// NAME: cvGradient_Custom
//
// PURPOSE: Outputs morphological gradient of image. A simple wrapper of cvMorphologyEx
//
// ARGUMENTS:
//      img - source image. Must be 1 channel.
//      dst - destination. Also 1 channel 8 bit
//      KERNEL_H,KERNEL_W - dimensions of the gradient kernel. Bigger = slower and more accurate
//      iterations - I still dont know why you'd want more than 1.
//      flags - 8 bitflags. bit1 = displays dst.
//
// METHOD: Just a wrapper for cvMorphologyEx set to gradient mode. 
//      
void cvGradient_Custom (IplImage* img, IplImage* &dst,
                        int KERNEL_W, int KERNEL_H, // dims of rectangular kernel
                        int iterations = 1, // number of times erode and dialate applied. 
                        int flags = 0)
// Name: Custom Gradient
// Outputs gradient of input image. Expects 1 channel image. A wrapper for cvMorphologyEx
{
    //assert (img != NULL);
    //assert (img->nChannels == 1);
    
    dst = cvCreateImage ( // create second image with 1 channel
        cvSize(img->width, img->height),
        IPL_DEPTH_8U,
        1);
    IplImage* temp = cvCreateImage ( // create temp with 1 channel
        cvSize(img->width, img->height),
        IPL_DEPTH_8U,
        1);
    
    cvMorphologyEx (img, //src
        dst, // dst
        temp, // temp
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
#endif
