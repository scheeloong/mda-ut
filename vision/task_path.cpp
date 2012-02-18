#include <stdio.h>
#include "task_path.h"
#include "common.h"

/**######### PATH FOLLOW TASKK ############*/
retcode vision_PATH (vision_in &Input, vision_out &Output, char flags) {
// state guide:
// ERROR = found lines in img, but they are not correct number/orientation to represent a pipe
// NO_DETECT = no detection (obj not in view)
// DETECT_1 = partial detection (pipe found, but too much off to the side). Will return pixel x,y and
//            not real x,y since range cannot be determined.
// DETECT_2 = full detection (pipe oriented almost directly below)
/** HS filter to extract object */
    IplImage* img_1;  
    if (flags & _DISPLAY) cvShowImage(Input.window[0], Input.img);
   
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
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_1);
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
        if ((len_ratio < 1.33) && (len_ratio > 0.75) && (tan_ratio < 1.25) && (tan_ratio > 0.8)) {
        // lines appear to form a pipe, so calculate range and angle
            int pix_x = (cseed[long1][0].x+cseed[long1][1].x+cseed[long2][0].x+cseed[long2][1].x)/4 - img_1->width/2;  
            int pix_y = (cseed[long1][0].y+cseed[long1][1].y+cseed[long2][0].y+cseed[long2][1].y)/4 - img_1->height/2;
            Output.range = PIPE_REAL_LEN * float(img_1->width) / (len1 * TAN_FOV_X);
            Output.real_x = pix_x * PIPE_REAL_LEN / len1;
            Output.real_y = pix_y * PIPE_REAL_LEN / len1;
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
        Output.real_x = float(img_centroid.x) / img_1->width;
        Output.real_y = float(img_centroid.y) / img_1->height; 
    
        ret = DETECT_1;
    }

    cvReleaseMemStorage(&storage);
    cvReleaseImage (&img_1); 
    delete cseed[0]; delete cseed;
    return ret;
}

/* ##############################################################################################
   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   ############################################################################################## */
// Control Code below
// 1. Each control function is expected to call its corresponding vision function, then use the
//    results to determine what to do.
// 2. The machine must output a character which controls the simulator (same as keyboard commands for now).
// 3. The machine can act on a state variable which will be pass in by reference. Basically this allows the 
//    controller function to behave as a finite state machine, with "state var" being the thing that records
//    what state the machine is in.
// 4. When first called "state" will have a value of 0.
// 5. The vision function will always be of the form:
//      retcode code = vision_X (vision_in, vision_out, flags);
//    code = a code that tells you how well the object was identified (fully in view vs half in view, etc).
//           !! Also depending on the code the data may have different meanings. !!
//           the code is enummed. See common.h
//    vision_in, vision_out - see common.h
//    flags = Bit flags for the vision function
//            _DISPLAY to display things
//            _INVERT to invert image (must set to properly display simulator imgs
//            _QUIET to suppress printed messages.
//            Multiple flags can be set using the | operator.
// 6. List of commands to send back:
//    'w','s' = forward / backward
//    'a','d' = turn left, right
//    'i','k' = rise, sink
//    The above is all the commands the sub can do in real life
//    'r','f' = roll left, roll right
//    't','g' = pitch foward, pitch backwards

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
    static const ESTATE lookup[8][3] = {
// vcode =         NO_DETECT,  DETECT_1,     DETECT_2
/* START */       {NO_PIPE,    OFF_CENTER,   OFF_CENTER},  
/* NO_PIPE */     {NO_PIPE,    OFF_CENTER,   OFF_CENTER},  
/* OFF_CENTER */  {NO_PIPE,    OFF_CENTER,   ERROR},  // variable output
/* CENTERED */    {PAUSE,      PAUSE,   ERROR},     // variable state for DETECT_2
/* UNALIGNED */   {PAUSE,      PAUSE,   ERROR},     // variable state for DETECT_2
/* ALIGNED */     {ALIGNED,    ALIGNED,      ALIGNED},   
/* ERROR */       {ERROR,      ERROR,        ERROR},
/* PAUSE */       {PAUSE,      OFF_CENTER,   CENTERED}};
    
    static ESTATE state = START; // starting state is start
    
    if ((state == ERROR) || (state == ALIGNED)) {
    }
    else if (vcode == NO_DETECT) { // cant see anything
        if ((state == START) || (state == NO_PIPE))
            state = NO_PIPE;
        else 
            state = PAUSE;
    }
    else if (vcode == DETECT_2) { // can recognize object as pipe
        if (Output.real_x*Output.real_x + Output.real_y*Output.real_y > 1)
            state = OFF_CENTER;
        else if (Output.range > 3.0)
            state = CENTERED; // but too high up
        else if (fabs(Output.tan_PA) > 0.03) // within 5ish degrees of vertical
            state = UNALIGNED;
        else
            state = ALIGNED;
    }
    else if (vcode == DETECT_1) { // cannot recognize obj
        if (Output.real_x*Output.real_x + Output.real_y*Output.real_y > 0.1)
            state = OFF_CENTER;
        else 
            state = CENTERED;
    }
    
    /** figure out output */
    switch (state) {
        case START: // no change to output
            return;
        case PAUSE:
            printf ("    State: PAUSE\n");
            return;
        case NO_PIPE:
            printf ("    State: NO_PIPE\n");
            m.move(FORWARD);
            return;
        case OFF_CENTER:
            printf ("    State: OFF_CENTER\n");
            if (Output.real_x == 0) Output.real_x = 0.0001;
            if (fabs(Output.real_y/Output.real_x) < 11.5) { // if obj outside of +-5 degrees from vertical
                m.move(STOP);	       
                m.move(FORWARD);
                if (Output.real_x > 0) // turn towards the obj
                    m.move(RIGHT,4);
                else
                    m.move(LEFT,4);
            }
            else {
                m.move(STOP);
                if (Output.real_y > 0)
                    m.move(FORWARD);
                else
                    m.move(REVERSE);
            }
            return;
        case CENTERED:
            printf ("    State: CENTERED\n");
            m.move(STOP);
            m.move(SINK,4);
            return;
        case UNALIGNED:
            printf ("    State: UNALIGNED\n");
            m.move(STOP);
            if (Output.tan_PA > 0)
                m.move(LEFT,6);
            else 
                m.move(RIGHT,6);
            return;
        case ALIGNED:
            printf ("    State: ALIGNED\n");
            m.move(STOP);
            m.move(FORWARD);
            m.move(RISE,3);
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




