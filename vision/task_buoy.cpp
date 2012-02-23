#include <stdio.h>
#include "task_buoy.h"
#include "common.h"

retcode vision_BUOY (vision_in &Input, vision_out &Output, char flags) {
// Will change real_x, real_y, range in Output.
// return state guide:
//  ERROR = bad, disregard this image. Happens if multiple circles found.
//  NO_DETECT = no detection (not enough pixels to constitute image)
//  DETECT_1 = partial detection (significant number of pixels, but shape did not trigger circle detector. 
//              Returns pixel center of mass of filtered image
//  DETECT_2 = full detection 
/** HS filter to extract object */
    IplImage* img_1;  
    float pix_fraction = HSV_filter (Input.img, img_1, Input.HSV);
/*
    IplConvKernel* kernel = cvCreateStructuringElementEx (3,3,1,1,CV_SHAPE_RECT);
    cvMorphologyEx (img_1, img_1, NULL, 
                    kernel, CV_MOP_OPEN);
    cvMorphologyEx (img_1, img_1, NULL, 
                    kernel, CV_MOP_CLOSE);
    cvReleaseStructuringElement (&kernel);
  */  
    if (flags & _INVERT) img_1->origin = 1;

    // check to see if there are enough pixels to do line finding
    if (pix_fraction < 0.001) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_BUOY: Pixel Fraction Too Low. Exiting.\n");
        return NO_DETECT;
    }
    
    /** use hough circles to look for circles in image */
    CvMemStorage* storage=cvCreateMemStorage(0);  
    CvSeq* circles = 0;
  
    cvSmooth(img_1,img_1, CV_BLUR, 5,5); // smooth to ensure canny will "catch" the circle
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_1);
        
    circles = cvHoughCircles(img_1, storage, CV_HOUGH_GRADIENT,
                             1, //  resolution in accumulator img. > 1 means lower res
                             100, // mindist
                             40, // canny high threshold
                             20 ); // accumulator threshold

    /** decide on output */
    int ncircles=circles->total; 
    printf ("ncircles: %d\n", ncircles);
    if (ncircles != 1) { 
        CvPoint img_centroid = calcImgCentroid (img_1); // always return pixel coords of centroid
        Output.real_x = img_centroid.x;  Output.real_y = img_centroid.y;
        
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage); 
        
        if (ncircles < 1) { // no circles => detect_1
            return DETECT_1;
        }
        else { // many circles => error
            printf ("  vision_BUOY: Multiple Circle Error.\n");
            return ERROR;
        }
    }
    else {
        float* data=(float*)cvGetSeqElem(circles, 0); // grab data for the single circle
        int buoy_pix_rad = cvRound(data[2]);
        
        if (flags & _DISPLAY) {
            CvPoint pt = cvPoint(data[0],data[1]); // x and y coordinate of circle    
            cvCircle(img_1, pt, cvRound(data[2]), CV_RGB(100,200,100), 2, 8);
            cvShowImage(Input.window[1], img_1);
        }
        Output.range = BUOY_REAL_RAD * float(img_1->width) / (buoy_pix_rad * TAN_FOV_X);
        Output.real_x = (data[0] - img_1->width*0.5) * BUOY_REAL_RAD / buoy_pix_rad;
        Output.real_y = (data[1] - img_1->height*0.5) * BUOY_REAL_RAD / buoy_pix_rad;
        
        if (!(flags & _QUIET)) {
            printf ("  vision_BUOY: Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
            printf ("  vision_BUOY: Range: %f\n", Output.range);
        }
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        return DETECT_2;        
    }
}

/** uses contour tests to determine if there are circles */
retcode vision_BUOY2 (vision_in &Input, vision_out &Output, char flags) {
// Will change real_x, real_y, range in Output.
// return state guide:
//  ERROR = bad, disregard this image. Happens if multiple circles found.
//  NO_DETECT = no detection (not enough pixels to constitute image)
//  DETECT_1 = partial detection (significant number of pixels, but shape did not trigger circle detector. 
//              Returns pixel center of mass of filtered image
//  DETECT_2 = full detection 
/** HS filter to extract object */
    IplImage* img_1;  
    float pix_fraction = HSV_filter (Input.img, img_1, Input.HSV);

    if (flags & _INVERT) img_1->origin = 1;
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_1);
    
    // check to see if there are enough pixels
    if (pix_fraction < 0.001) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_BUOY: Pixel Fraction Too Low. Exiting.\n");
        return NO_DETECT;
    }
    
//     /** use cvFindContours to look for contours in the image */
//     CvSeq * contours;
//     CvMemStorage* storage = cvCreateMemStorage (0);
//     
//     int nContours = cvFindContours (
//         img_1,    // input image, will be scratched
//         storage, // storage
//         &contours, // output pointer to CvSeq*, root node of contour tree
//         sizeof (CvContour), // header size, must be this
//         CV_RETR_EXTERNAL, // only retrieve outermost contours
//         CV_CHAIN_APPROX_NONE
//     );
//     
//     if (flags & _DISPLAY) { // display the contours
//         cvZero(img_1); // erase img_1
//         cvDrawContours(
//                     temp,       // img to draw on
//                     contours,   // contour
//                     cvScalar (100,100,100), // solid color
//                     cvScalar (0,0,0),       // hole color
//                     0 // max level. 0 means root level only
//                     );
//         cvShowImage(Input.window[1], img_1);
//     }
    
    
    IplImage* img_2 = cvCreateImage (cvGetSize(img_1), img_1->depth, 1);
    
    cvSmooth(img_1,img_1, CV_BLUR, 3,3);
    cvCanny (img_1, img_2,
             15,
             40,
             3
            );
    
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_2);
    
    /** calculate Central Moments */
    CvMoments m;
    CvHuMoments hm;
    cvMoments (img_2, &m, 1); // calculates all moments, including central moments
    cvGetHuMoments (&m, &hm);
    
    //printf (" m00: %lf\n m11: %lf\n m20: %lf\n m02: %lf\n", m.m00,m.mu11,m.mu20,m.mu02);
    //printf (" m30: %lf\n m03: %lf\n m21: %lf\n m12: %lf\n\n", m.m30,m.mu03,m.mu21,m.mu12);
    
    printf (" m00: %lf\n h1: %lf\n h2: %lf\n h3: %lf\n h4: %lf\n h5: %lf\n\n", m.m00,hm.hu1,hm.hu2,hm.hu3,hm.hu4,hm.hu5);
    
    cvReleaseImage (&img_1);
    return ERROR;
}







void controller_BUOY (vision_in &Input, Mission &m) {
    retcode vcode;
    // vcode table:
//  ERROR = bad, disregard this image. Happens if multiple circles found.
//  NO_DETECT = no detection (not enough pixels to constitute image)
//  DETECT_1 = partial detection (significant number of pixels, but shape did not trigger circle detector. 
//              Returns pixel center of mass of filtered image
//  DETECT_2 = full detection 
    
    vision_out Output;
    vcode = vision_BUOY2 (Input, Output, _INVERT | _DISPLAY);
    
    //enum ESTATE = {START, OFF_CENTER, CENTERED, PAUSE};

    return;
};
