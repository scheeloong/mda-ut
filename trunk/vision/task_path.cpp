#include <stdio.h>
#include "task_path.h"
#include "common.h"

/**######### PATH FOLLOW TASKK ############*/
retcode vision_PATH (vision_in &Input, vision_out &Output, char flags) {
// state guide:
// ERROR = found lines in img, but they are not correct number/orientation to represent a pipe
// NO_DETECT = no detection (obj not in view)
// DETECT_1 = partial detection (pipe found, but too much off to the side). Will return pixel_x and
//            not real x,y since range cannot be determined. The returned pix_x will have origin at the
//            center of the img and normalized so a pixel at the edge of the image has pix_x == 1.0;
// DETECT_2 = full detection (pipe oriented almost directly below)
/** HS filter to extract object */
    IplImage* img_1;  
   
    float pix_fraction = HSV_filter (Input.img, img_1, Input.HSV); // need to delete img_1
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
        printf ("  vision_PATH: Pixel Fraction Too Low. Exiting.\n");
        return NO_DETECT;
    }
    
/** take gradient of image */    
    cvGradient_Custom (img_1, img_1, 3, 3, 1);
    //if (flags & _DISPLAY) cvShowImage(window[1], img_1);
    
/** probabilistic Hough line finder. Determine the threshold using the number of high pixels */
    int thresh = 10;//(int)(sqrt(pix/PATH_SKINNYNESS)); // guessed length of pipe in pixels
    CvMemStorage* storage = cvCreateMemStorage(0); // create memstorage for line finidng, delete later
    CvSeq* lines = 0;
    
    int minlen=img_1->height*0.1, mindist=img_1->height*0.1;
    lines = cvHoughLines2(img_1, storage,
        CV_HOUGH_PROBABILISTIC,
        1, CV_PI/360.0,
        thresh, minlen, mindist);
/** check if lines were found, if not quit */
    int nlines=lines->total;
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_PATH: No Lines Detected. Exiting.\n");
        return NO_DETECT;
    }
    
/** arrange lines by X and Y value. Will bug if 45 degree lines encountered. Assume no horiz lines. */
    CvPoint* temp; int swap;
    for (int i = 0; i < nlines; i++) { // for each line
        temp = (CvPoint*)cvGetSeqElem(lines, i);  
 
        if (fabs(temp[1].y-temp[0].y) < fabs(temp[1].x-temp[0].x)) {  // horiz line
            if (temp[0].x > temp[1].x) { // sort so lower X value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
        }   
        else {
            if (temp[0].y > temp[1].y) { // sort so lower Y value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
        }
    }

/** perform clustering */
    int nseeds=0;
    CvPoint** cseed=0;
    KMcluster_auto_K (cseed, nseeds, 1,4, lines, nlines, 2, _QUIET);    // 2 iterations needed I think
// display clustered lines
    if (flags & _DISPLAY) {
        for (int i = 0; i < nseeds; i++) 
            cvLine (img_1, cseed[i][0],cseed[i][1], CV_RGB(0,50,50), 2);
    }
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_1);
/** calculations and cleanup */
// if the 2 longest lines are parallel and equal length then OK
// So first find the length of 2 longest lines
    retcode ret=ERROR;
  
    if (nseeds >= 2) { // definitely reject img if less than 2 lines
        int long1=0, long2=0; // long1 = index of longest line. long2 = index of second longest
        for (int i = 1; i < nseeds; i++)
            if (sqr_length(cseed,i) > sqr_length(cseed,long1)) long1 = i;
        for (int i = 1; i < nseeds && i != long1; i++) 
            if (sqr_length(cseed,i) > sqr_length(cseed,long2)) long2 = i;
            
        // now check that these two lines have similar lengths and angles
        float len1 = sqr_length(cseed,long1), len2 = sqr_length(cseed,long2); // calculate lengths and tan of angles
        float ang1 = tangent_angle(cseed,long1), ang2 = tangent_angle(cseed, long2); 
        float len_ratio = len1 / len2; 
        float tan_ratio = ang1 / ang2; // ratios used to calculate shape of the object

        /** check for rectangular-ness and correctness of dimensions */
        if ((len_ratio < 4) && (tan_ratio < 1.5) && (tan_ratio > 0.66)) {
        // lines appear to form a pipe, so calculate range and angle
            int pix_x = (cseed[long1][0].x+cseed[long1][1].x+cseed[long2][0].x+cseed[long2][1].x)/4 - img_1->width/2;  
            int pix_y = (cseed[long1][0].y+cseed[long1][1].y+cseed[long2][0].y+cseed[long2][1].y)/4 - img_1->height/2;
            Output.range = float(PIPE_REAL_LEN * img_1->width) / (len1 * TAN_FOV_X);
            Output.real_x = float(pix_x * PIPE_REAL_LEN) / len1;
            Output.real_y = float(pix_y * PIPE_REAL_LEN) / len1;
            Output.tan_PA = (ang1+ang2) * 0.5;
            
            if (!(flags & _QUIET)) {
                printf ("  vision_PATH: Successful.\n  Range&Angle:  %f  %f\n",Output.range,atan(Output.tan_PA));
                printf ("  RealX,RealY:  %f  %f\n", Output.real_x, Output.real_y);
            }
            if (flags & _DISPLAY) {
                float ang = atan(Output.tan_PA) - CV_PI/2;
                CvPoint p1 = cvPoint (pix_x+img_1->width/2 - len1*cos(ang),pix_y+img_1->height/2 - len1*sin(ang));
                CvPoint p2 = cvPoint (pix_x+img_1->width/2 + len1*cos(ang),pix_y+img_1->height/2 + len1*sin(ang));
                cvLine (img_1, p1, p2, CV_RGB(255,100,100), 2);
                cvShowImage(Input.window[1], img_1);
            }            
            ret = DETECT_2;
        }
    }
    
    if (ret == ERROR) { // the object is not a pipe?
        /** can only return pixel space info based on img centroid */
        printf ("  vision_PATH: Cant Identify Object.\n");
        CvPoint img_centroid = calcImgCentroid (img_1);  
        Output.real_x = 2*float(img_centroid.x) / img_1->width; // equal to 1 if point at the edge
        Output.real_y = 2*float(img_centroid.y) / img_1->height; 
    
        ret = DETECT_1;
    }

    cvReleaseMemStorage(&storage);
    cvReleaseImage (&img_1); 
    delete cseed[0]; delete cseed;
    return ret;
}

#define FWD_SPEED 3
#define SINK_SPEED 3
#define TURN_SPEED 5
#define OBS_DEPTH -6 // depth to look for path

void controller_PATH (vision_in &Input, Mission &m) {
// following the path - fairly complex controls. Need to sink to right depth after acquiring the pipe.
// Then rotate until pipe aligned. Then rise and move foward. 
    retcode vcode;
    // cvode guide:
    // ERROR = found lines in img, but they are not correct number/orientation to represent a pipe
    // NO_DETECT = no detection (pipe not in view, not enough pixels)
    // DETECT_1 = partial detection (pipe found, but too much off to the side). pathX,pathY = pixel location of pipe center
    //     relative to img center. tan_angle and length not defined.
    // DETECT_2 = full detection (pipe oriented almost directly below). pathX,pathY same as befoer. 
    //     length = length of pipe normalized to the diagonal of the image (so length is 0.5 if pipe is half the diagonal length). 
    //     tan_angle = tangent of the angle of the pipe. (So ratio of the vertical length to horiz length). Is 0 if pipe 
    //     is horizontal.
    
    vision_out Output;
    //Input.HSV.setAll(10, 30, 130, 255, 40, 255);
    vcode = vision_PATH (Input, Output, _DISPLAY);
        
    // control starts here
    /** state machine
     * START = starting state
     * NO_PIPE = cannot see pipe, assume it is directly in front
     * OFF_CENTER = pipe too off center
     * CENTERED = pipe centered but not correct depth
     * UNALIGNED = pipe correct depth but unaligned
     * ALIGNED = pipe aligned.
     * ERROR 
     * PAUSE = state with no output. Go here if unsure of correctness of vision but not ERROR.
    */
    
    /// remember tan_angle is 0 if a line is vertical, and +ve in the counter clockwise dir
    
    enum ESTATE {START, NO_PIPE, OFF_CENTER, CENTERED, UNALIGNED, ALIGNED, ERROR, PAUSE};    
    static ESTATE state = NO_PIPE;
    PI_Controller C;
    C.setK1K2 (-12.0/PI, -0.8); 

    float real_rad_sqr;
    if ((state == ERROR) || (state == ALIGNED)) {
    }
    else if (vcode == NO_DETECT) { // cant see anything
        if ((state == START) || (state == OFF_CENTER)) {
            state = NO_PIPE;
            C.reset();
        }
        else if (state != NO_PIPE) 
            state = NO_PIPE;
    }
    else if (vcode == DETECT_2) { // can recognize object as pipe
        real_rad_sqr = Output.real_x*Output.real_x + Output.real_y*Output.real_y;
        
        if ((state == CENTERED || state == UNALIGNED) && real_rad_sqr > 1.0) // off center if outside 1.0
            state = OFF_CENTER;
        else if (state == OFF_CENTER && real_rad_sqr > 0.6) // keep state as off centered until within 0.6
            state = OFF_CENTER;
        
        else if (Output.range > 4.0)
            state = CENTERED; // but too high up
        else if (fabs(Output.tan_PA) > 0.03) { // within 5ish degrees of vertical
            if (state != UNALIGNED) {
                C.reset();
                state = UNALIGNED;
            }
        }
        else
            state = ALIGNED;
    }
    else if (vcode == DETECT_1) { // cannot recognize obj
        if (state == CENTERED && Output.real_x*Output.real_x + Output.real_y*Output.real_y > 0.6)
            state = OFF_CENTER;
        if (state == OFF_CENTER && Output.real_x*Output.real_x + Output.real_y*Output.real_y < 0.3)
            state = CENTERED;
    }
    
    float angle;
    /** figure out output */
    switch (state) {
        case START: // no change to output
            return;
        case PAUSE:
            printf ("    State: PAUSE\n");
            return;
        case NO_PIPE:
            printf ("    State: NO_PIPE\n");
            m.move(STOP);
            m.move(FORWARD, FWD_SPEED);
            return;
        case OFF_CENTER:
            printf ("    State: OFF_CENTER\n");
            if (Output.real_x == 0) Output.real_x = 0.00001;
            angle = atan (Output.real_y / Output.real_x);
            C.update (angle);
            
            m.move (STOP);
            if (ABS(angle) < 30) {
                if (Output.real_y > 0)
                    m.move (FORWARD, 1);
                else 
                    m.move (REVERSE, 1);
            }
            m.move (LEFT, (int)(3*C.out()));
            
            /*temp = Output.real_y/Output.real_x; // temp is y/x ratio
            m.move(STOP);

            if ((temp < 11.5) && (temp >= 0)) { 
            // more than 5 degrees from vertical, 1st or 3rd quadrant
                m.move (RIGHT,TURN_SPEED);

                if (temp > 1.7) { // not more than 30 degrees from vert
                    if (Output.real_y >= 0)
                        m.move(FORWARD, FWD_SPEED);
                    else 
                        m.move(REVERSE, FWD_SPEED);
                }
            }
            else if ((temp < 0) && (temp > -11.5)) {
            // 2nd or 4th quadrant
                m.move (LEFT,TURN_SPEED);

                if (temp > 1.7) { // not more than 30 degrees off
                    if (Output.real_y >= 0)
                        m.move (FORWARD, FWD_SPEED);
                    else
                    m.move (REVERSE, FWD_SPEED);
                }
            }
            else {
                if (Output.real_y >= 0)
                    m.move(FORWARD, FWD_SPEED);
                else
                    m.move(REVERSE, FWD_SPEED);
            }*/
            return;
        case CENTERED:
            printf ("    State: CENTERED\n");
            m.move(STOP);
            if (!(vcode == DETECT_2 && Output.range < 3.8))
                m.move(SINK, SINK_SPEED);
            return;
        case UNALIGNED:
            printf ("    State: UNALIGNED\n");
            if (vcode == DETECT_2) 
                C.update(atan(-Output.tan_PA)); // take the negative due to how tangent_angle works
            
            m.move (STOP);
            m.move (LEFT, (int)(3*C.out()));
            return;
        case ALIGNED:
            printf ("    State: ALIGNED\n");
            m.move(STOP);
            m.move(FORWARD, FWD_SPEED);
            m.move(RISE, SINK_SPEED);
            return;       
        case ERROR:
            printf ("    ERROR!!\n");
            return;
        default:
            printf ("\ntask_path: NO IDEA WHAT STATE I'M IN!\n\n");
            for (;;);
    }
    return;
}

