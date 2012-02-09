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
/** HS filter to extract gate object */
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
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_1);

    // check to see if there are enough pixels to do line finding
    if (pix_fraction < 0.002) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_GATE: Pixel Fraction Too Low. Exiting.\n");
        return NO_DETECT;
    }
    
    /** use hough circles to look for circles in image */
    CvMemStorage* storage=cvCreateMemStorage(0);  
    CvSeq* circles = 0;
  
    cvSmooth(img_1,img_1, CV_BLUR, 5,5); // smooth to ensure canny will "catch" the circle
    
    circles = cvHoughCircles(img_1, storage, CV_HOUGH_GRADIENT,
                             1, //  resolution in accumulator img. > 1 means lower res
                             100, // mindist
                             50, // canny high threshold
                             40 ); // accumulator threshold

    /** check that circles were found */
    int ncircles=circles->total; 
    if (ncircles < 1) {
        CvPoint img_centroid = calcImgCentroid (img_1);
        Output.real_x = img_centroid.x;  Output.real_y = img_centroid.y;
        
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_BUOY: No Circles Detected. Exiting.\n");
        return DETECT_1;
    }
    printf("Number of Circles: %d\n", ncircles);
    
    /** decide on output */
    if (ncircles > 1) {
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_BUOY: Multiple Circle Error.\n");
        return ERROR;
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
            printf ("  vision_BUOY: Buoy Detected.\n");
            printf ("  vision_GATE: Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
            printf ("  vision_GATE: Range: %f\n", Output.range);
        }
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        return DETECT_2;        
    }
}

void controller_BUOY (vision_in Input, Mission &m) {
    retcode vcode;
    // vcode table:
    // -1 = error, disregard this image
    // 0 = no detection (not enough pixels to constitute image, or no lines detected). Probably have to move closer.
    // 1 = partial detection (1 segment detected)  gateX, gateY correspond to single segment center. Range valid
    // 2 = full detection (2 segments detected).   gateX, gateY = gate center. Range valid
    // 3 = gate very close, (dist between segments > 3/4 image width). Should stop using vision to navigate. data same as 2.
    //     Alternatively can ignore this and use the range.
    
    Input.HSV.setAll(0, 50, 10, 255, 0, 255);
    vision_out Output;
    vcode = vision_BUOY (Input, Output, _INVERT);

    return;
};
