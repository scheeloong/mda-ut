#include <stdio.h>
#include "highgui.h"
#include <time.h>
#include "task_torp.h"
#include "common.h"

// status: ok, the control(sim) seems to work well. In addition, it shows a cut-out circle to be shot
// before shooting, so it's going well. However that only works on the red side, not the blue side...
// Also, I don't think the move() has a mechanism for shooting...
// declare some global variables, as they should be useful for cut-out circle detection:
int ratio; // initially garbage, this is to set ratio of large to small circle
/* declare the circle to be passed through */
float* small_circle = NULL;
float* big_circle = NULL;

retcode vision_TORP (vision_in &Input, vision_out &Output, char flags) {
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

    // check to see if there are enough pixels to do line finding
    if (pix_fraction < 0.001) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_TORP: Pixel Fraction Too Low. Exiting.\n");
        return NO_DETECT;
    }
    
    
    CvMemStorage* storage=cvCreateMemStorage(0);  
     
    CvSeq* circles = 0;
   
    cvSmooth(img_1,img_1, CV_BLUR, 1,1); // smooth to ensure canny will "catch" the circle
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_1);
    
    circles = cvHoughCircles(img_1, storage, CV_HOUGH_GRADIENT,
                             2, //  resolution in accumulator img. > 1 means lower res
                             100, // mindist
                             50, // canny high threshold
                             25 ); // accumulator threshold  
                             
    /** decide on output */
    int ncircles=circles->total; 
    if (!(flags & _QUIET)) printf ("ncircles: %d\n", ncircles);
    if (ncircles != 2) { 
        CvPoint img_centroid = calcImgCentroid (img_1); // always return pixel coords of centroid
        Output.real_x = 2*float(img_centroid.x) / img_1->width; // equal to 1 if point at the edge
        Output.real_y = 2*float(img_centroid.y) / img_1->height; 
        
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage); 
        
        if (ncircles < 2) { // no circles => detect_1
            return DETECT_1;
        }
        else if(ncircles > 2) { // many circles => error
            printf ("  vision_TORP: Multiple Circle Error.\n");
            return ERROR;
        }
    }
    else {
      
      // "hack" the method of circle finding: find which of the two is to be passed, and set that circle to be 
      // searched in a similar way to buoy searching.
	int radii[2];
	float* elem1 = (float*)cvGetSeqElem(circles,0);
	radii[0] = cvRound(elem1[2]); // first circles
	float* elem2 = (float*)cvGetSeqElem(circles,1);
	radii[1] = cvRound(elem2[2]);
	if(radii[0] > radii[1]) {
	  ratio = radii[0]/radii[1];
	  small_circle = elem2;
	  big_circle = elem1;
	  printf(" ratio: %d\n", ratio);
	  printf(" small radius: %d\n", radii[1]);
	  printf(" big radius: %d\n", radii[0]);
	}
	else {
	  ratio = radii[1]/radii[0];
	  small_circle = elem1;
	  big_circle = elem2;
	  printf(" ratio: %d\n", ratio);
	  printf(" small radius: %d\n", radii[0]);
	  printf(" big radius: %d\n", radii[1]);
	}
	/*if(ratio != 2) {// the ratio of the large to small cut-out circle must be 2
	  printf("   vision_TORP: incorrect circles; ratio not 2!\n");
	  return DETECT_1;
	}*/
	
	
	
        //float* data=(float*)cvGetSeqElem(circles, 0); // grab data for the single circle
	// the rest of this is basically the same, except we concentrate on the "smaller" circle
	// so long as we find two circles that maintain the same ratio 
        int buoy_pix_rad = cvRound(small_circle[2]);
        
        if (flags & _DISPLAY) {
            CvPoint pt = cvPoint(small_circle[0],small_circle[1]); // x and y coordinate of circle    
            cvCircle(img_1, pt, cvRound(small_circle[2]), CV_RGB(100,200,100), 1, 8);
            cvShowImage(Input.window[1], img_1); //cvShowImage(Input.window[2], gray); 
        }
        Output.range = float(BUOY_REAL_RAD * img_1->width) / (buoy_pix_rad * TAN_FOV_X);
        Output.real_x = float(small_circle[0] - img_1->width*0.5) * BUOY_REAL_RAD / buoy_pix_rad;
        Output.real_y = float(small_circle[1] - img_1->height*0.5) * BUOY_REAL_RAD / buoy_pix_rad;
        
        if (!(flags & _QUIET)) {
	    printf ("  vision_TORP: SMALL CUT-OUT CIRCLE\n");
            printf ("  vision_TORP: Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
            printf ("             : Range: %f\n", Output.range);
        }
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        return DETECT_2;        
    }
}

#define NB_t        6   // after travelling foward for this long in NB, go to PAN
#define PAN_t       12  // stay in PAN for this long (pan and back), must be multiple of 4
#define CHARGE_t    2   // Charge forwards for this time   
#define RET_t       10   // return for this much time

#define FWD_SPEED 1
#define SINK_SPEED 2
#define TURN_SPEED 3
#define DONE_RANGE 50

void controller_TORP (vision_in &Input, Motors &m) {
    retcode vcode;
    // vcode table:
//  ERROR = bad, disregard this image. Happens if multiple circles found.
//  NO_DETECT = no detection (not enough pixels to constitute image)
//  DETECT_1 = partial detection (significant number of pixels, but shape did not trigger circle detector. 
//              Returns pixel center of mass of filtered image
//              The coordinates returned is relative: x = 1 refers to edge of image. x = 0 refers to center
//  DETECT_2 = full detection. Coordinates returned are not relative 
    
    vision_out Output;
    vcode = vision_TORP (Input, Output, _INVERT | _DISPLAY);
    
    int t = time(NULL);         // read the current time. The "true" timer reading is t-dt
    static int dt = t; // dt is a time offset, used to find how much time since last event
    static PI_Controller PI_x, PI_y; // for NO_BUOY and GOOD states, respectively
    PI_x.setK1K2 (-24.0/PI, -0.8); // K1 is set so output is 0 if input is tan(30degrees)
    PI_y.setK1K2 (-24.0/PI, -0.8);
    
    enum ESTATE {NO_TORP, GOOD, PAN, CHARGE, SHOOT, DONE, /*PAUSE*/};
    
    static ESTATE state = NO_TORP;
        
    if (state == NO_TORP) {
        if (vcode == DETECT_2) { // found something!
            state = GOOD;
            PI_x.reset(); // reset PI controllers
            PI_y.reset();
            dt = t; // reset timer
        }
       else if (t-dt > NB_t) { // we travelled a long time without finding anything
            state = PAN;
            dt = t;
        }
    }
    else if (state == GOOD) {
        if ((vcode == NO_DETECT) || (vcode == DETECT_1)) { // lost object
            state = NO_TORP;
            dt = t;
        }
        else if (Output.range < DONE_RANGE) { // we are close enough to be considered done
            state = CHARGE;
            dt = t;
        }
    }
    else if (state == PAN) {
        if ((vcode == DETECT_2) || (t-dt > PAN_t)) {
            state = NO_TORP;
            dt = t;
        }
    }
    else if (state == CHARGE) {
        if (t-dt > CHARGE_t) {
            state = SHOOT;
            dt = t;
        }
    }
    else if (state == SHOOT) {
        if (t-dt > RET_t) 
            state = DONE;
    }
    
    float angle_x, angle_y;
    
    /// output logic
    switch (state) {
        case NO_TORP:
            printf ("   torp: NO_TORP\n");
            m.move (STOP);
            m.move (FORWARD, FWD_SPEED);
            
            if (vcode == DETECT_1) {
                if (Output.real_x > 0.7) m.move (RIGHT, 2*TURN_SPEED);
                else if (Output.real_x > 0.3) m.move (RIGHT, TURN_SPEED);
                else if (Output.real_x < -0.7) m.move (LEFT, 2*TURN_SPEED);
                else if (Output.real_x < -0.3) m.move (LEFT, TURN_SPEED);
            
                if (Output.real_y > 0.7) m.move (RISE, 2*SINK_SPEED);
                else if (Output.real_y > 0.3) m.move (RISE, SINK_SPEED);
                else if (Output.real_y < -0.7) m.move (SINK, 2*SINK_SPEED);
                else if (Output.real_y < -0.3) m.move (SINK, SINK_SPEED);
            }
            break;
        case GOOD:
            printf ("   torp: DETECTED\n");
            printf ("   (%f,%f), R=%f\n", Output.real_x,Output.real_y,Output.range);
            
            angle_x = atan(Output.real_x / Output.range); // calculate angles for x/z, y/z
            angle_y = atan(Output.real_y / Output.range);
            
            PI_x.update (angle_x);
            PI_y.update (angle_y);
            
            printf ("   PI_x.out: %f\n", PI_x.out());
            printf ("   PI_y.out: %f\n", PI_y.out());
            
            m.move(STOP);
            if (ABS(angle_x) < 40 && ABS(angle_y) < 40)
                m.move (FORWARD, FWD_SPEED);
            m.move (LEFT, (int)(TURN_SPEED*PI_x.out()));
            m.move (SINK, (int)(SINK_SPEED*PI_y.out()));
            
            break;
        case CHARGE:
            printf ("   torp: FINISHED. Charging Forward.\n");
            m.move (STOP);
            m.move (STOP);
            m.move (FORWARD, FWD_SPEED);
            break;
        case SHOOT:
            printf ("   torp: FINISHED. Preparing to Shoot.\n");
            m.move(STOP);
	    // do something to do the actual shooting
            break;
        /*case PAUSE:
            break;*/
        case DONE:
	    printf("	torp: FINISHED. Shot.\n");
            m.move (STOP);
            break;
        case PAN:
            printf ("   torp: PANNING\n");
            m.move (STOP);
            if (t-dt < PAN_t/4 || t-dt >= 3*PAN_t/4)  // pan left for first half
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