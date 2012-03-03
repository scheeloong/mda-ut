#include <stdio.h>
#include <time.h>
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
  
    cvSmooth(img_1,img_1, CV_BLUR, 3,3); // smooth to ensure canny will "catch" the circle
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_1);
     
    circles = cvHoughCircles(img_1, storage, CV_HOUGH_GRADIENT,
                             1, //  resolution in accumulator img. > 1 means lower res
                             100, // mindist
                             60, // canny high threshold
                             30 ); // accumulator threshold

    /** decide on output */
    int ncircles=circles->total; 
    printf ("ncircles: %d\n", ncircles);
    if (ncircles != 1) { 
        CvPoint img_centroid = calcImgCentroid (img_1); // always return pixel coords of centroid
        Output.real_x = 2*float(img_centroid.x) / img_1->width; // equal to 1 if point at the edge
        Output.real_y = 2*float(img_centroid.y) / img_1->height; 
        
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


#define NB_t        6   // after travelling foward for this long in NB, go to PAN
#define PAN_t       8   // stay in PAN for this long (pan and back)
#define CHARGE_t    1   
#define RET_t       3   // return for this much time

#define FWD_SPEED 1
#define SINK_SPEED 1
#define TURN_SPEED 3
#define DONE_RANGE 40

void controller_BUOY (vision_in &Input, Mission &m) {
    retcode vcode;
    // vcode table:
//  ERROR = bad, disregard this image. Happens if multiple circles found.
//  NO_DETECT = no detection (not enough pixels to constitute image)
//  DETECT_1 = partial detection (significant number of pixels, but shape did not trigger circle detector. 
//              Returns pixel center of mass of filtered image
//  DETECT_2 = full detection 
    
    vision_out Output;
    vcode = vision_BUOY (Input, Output, _INVERT | _DISPLAY);
    
    static int dt = time(NULL); // dt is a time offset, used to find how much time since last event
    int t = time(NULL);         // read the current time. The "true" timer reading is t-dt
    
    enum ESTATE {NO_BUOY, GOOD, PAN, CHARGE, RET, DONE, PAUSE};
    /** State Guide:
     * The sub starts in NB and travels foward. If it sees something, it goes to GOOD to 
     * home in on that object.
     * If travels in NB for NB_t without seeing anything, start panning to search for object
     * Once we are close enough to the object, go to CHARGE, which will charge at the buoy
     * for CHARGE_t, and then go to RET, which will go backwards for a bit
     */
    
    static ESTATE state = NO_BUOY;
        
    if (state == NO_BUOY) {
        if (vcode == DETECT_2) { // found something!
            state = GOOD;
            dt = t; // reset timer
        }
       else if (t-dt > NB_t) { // we travelled a long time without finding anything
            state = PAN;
            dt = t;
        }
    }
    else if (state == GOOD) {
        if ((vcode == NO_DETECT) || (vcode == DETECT_1)) { // lost object
            state = NO_BUOY;
            dt = t;
        }
        else if (Output.range < DONE_RANGE) // we are close enough to be considered done
            state = RET;
    }
    else if (state == PAN) {
        if ((vcode == DETECT_2) || (t-dt >= PAN_t)) {
            state = NO_BUOY;
            dt = t;
        }
    }
    else if (state == CHARGE) {
        if (t-dt > CHARGE_t) {
            state = RET;
            dt = t;
        }
    }
    else if (state == RET) {
        if (t-dt > RET_t) 
            state = DONE;
    }
    
    /// output logic
    
    switch (state) {
        case NO_BUOY:
            printf ("   buoy: NO_BUOY\n");
            m.move (STOP);
            m.move (FORWARD, FWD_SPEED);
            
            if (vcode == DETECT_1) {
                if (Output.real_x > 0.5)
                    m.move (RIGHT, TURN_SPEED);
                else if (Output.real_x < -0.5)
                    m.move (LEFT, TURN_SPEED);
                if (Output.real_y > 0.5)
                    m.move (RISE, SINK_SPEED);
                else if (Output.real_y < -0.5)
                    m.move (SINK, SINK_SPEED);
            }
            break;
        case GOOD:
            printf ("   buoy: DETECTED\n");
            printf ("   (%f,%f), R=%f\n", Output.real_x,Output.real_y,Output.range);
            
            m.move(STOP);
            if (Output.real_x > 0.4) 
                m.move (RIGHT, TURN_SPEED);
            else if (Output.real_x < -0.4)
                m.move (LEFT, TURN_SPEED);
            if (Output.real_y > 0.3)
                m.move (RISE, SINK_SPEED);
            else if (Output.real_y < -0.3)
                m.move (SINK, SINK_SPEED);
            
            if ((Output.real_x*Output.real_x + Output.real_y*Output.real_y) / (Output.range*Output.range) < 0.35)  
                m.move (FORWARD, FWD_SPEED); // less than 20 degrees
            break;
        case CHARGE:
            printf ("   buoy: FINISHED\n");
            m.move (FORWARD, FWD_SPEED);
            break;
        case RET:
            printf ("   buoy: FINISHED\n");
            m.move (REVERSE, FWD_SPEED);
            break;
        case PAUSE:
            break;
        case DONE:
            m.move (STOP);
            break;
        case PAN:
            printf ("   buoy: PANNING\n");
            m.move (STOP);
            if (t-dt < PAN_t/2)  // pan left for first half
                m.move(LEFT, TURN_SPEED);
            else 
                m.move (RIGHT, TURN_SPEED);
            break;
        default:
            printf ("\n\n I dont know what state Im in!!\n");
            while (1);
    }       
                          
    return;
};













/** uses contour tests to determine if there are circles.
 * This was coded as an experiment and will not be actually used */
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
