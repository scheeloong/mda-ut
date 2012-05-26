#include <stdio.h>
#include "task_U.h"
#include "common.h"

/**######### THE U SHAPE DETECTOR TASKK ############*/
retcode vision_U (vision_in &Input, vision_out &Output, char flags) {
// Will change real_x, real_y, range in Output.
// return state guide:
//  ERROR = bad, disregard this image
//  NO_DETECT = no detection (not enough pixels to constitute image, or no lines detected)
//  DETECT_1 = sufficient pixels, but unable to recognize object. real_x,real_y refer to pixel 
//	    coordinates of the image centroid. No other valid output.
//      The returned pix_x will have origin at the center of the img and normalized so a 
//      pixel at the edge of the image has pix_x == 1.0;
//  DETECT_2 = object recognized as U, but can only see part of it. real_x,real_y refer to real
//	    space coordinates of U center (best guess), range is valid. 
//  DETECT_3 = object recognized as U and all parts visible. Same as DETECT_2 but
//	    real_x,real_y should be very accurate
/** HS filter to extract object */
    IplImage* img_1;  
    float pix_fraction;
    if (flags & _ADJ_COLOR)
        pix_fraction = HSV_adjust_filter (Input.img, img_1, Input.HSV);
    else
        pix_fraction = HSV_filter (Input.img, img_1, Input.HSV);
    assert ((pix_fraction) >= 0);

    if (flags & _INVERT) img_1->origin = 1;
    if (flags & _DISPLAY) cvShowImage(Input.window[0], img_1);

    // check to see if there are enough pixels to do line finding
    if (pix_fraction < 0.001) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_U: Pixel Fraction %5.3f Too Low. Exiting.\n", pix_fraction);
        return NO_DETECT;
    }

/** probabilistic Hough line finder. Determine the threshold using the number of high pixels */
    int thresh = 40; //(int)(sqrt(pix_fraction*img_1->imageSize * U_SKINNYNESS) / 4.0); 
    CvMemStorage* storage = cvCreateMemStorage(0); // create memstorage for line finidng, delete later
    CvSeq* lines = 0;
    
    int minlen=img_1->height/15, mindist=minlen;
    lines = cvHoughLines2(img_1, storage,
        CV_HOUGH_PROBABILISTIC,
        2, CV_PI/180.0,
        thresh, minlen, mindist);

/** check if lines were found, if not quit */
    int nlines=lines->total;
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_U: No Lines Detected. Exiting.\n");
        return NO_DETECT;
    }
    
/** perform clustering */
    int nseeds=0;
    CvPoint** cseed=0;  // this memory needs to be freed later
    KMcluster_auto_K (cseed, nseeds, 1,3, lines, nlines, 1, _QUIET); // 1 to 3 segments
    cvReleaseMemStorage (&storage);

// display clustered lines
    if (flags & _DISPLAY) {
        for (int i = 0; i < nseeds; i++) 
            cvLine (img_1, cseed[i][0],cseed[i][1], CV_RGB(0,50,50), 2);
        cvShowImage(Input.window[1], img_1);
    }
    
/** perform sanity check to see what we are looking at */
    //const float obj_real_width = 54, obj_real_height = 40;
    int pix_x, pix_y; // x and y location and Range of target in pixel space
    retcode ret = ERROR; 
    // note: if ret still == error at the end of the nested if structure below 
    // we automatically calculate centroid and return DETECT_1

    if (nseeds == 1) { 
        int dx = ABS(cseed[0][1].x-cseed[0][0].x)+1; // +1 to make sure not zero 
        int dy = ABS(cseed[0][1].y-cseed[0][1].y)+1;
        
        // first check if single line is horiz or vert
        if (float(dx)/dy > 4.0) { // horiz line, a bit more than tan(10deg)
            if (sqr_length(cseed, 0) > img_1->width*0.2) { 
                // use horiz seperation to estimate range and (x,y)
                int obj_pix_width = cseed[0][1].x - cseed[0][0].x;
                pix_x = (cseed[0][1].x + cseed[0][0].x)/2 - img_1->width/2;
                // displace pix_y to estimated location of U center
                pix_y = (cseed[0][1].y + cseed[0][0].y)/2 + obj_pix_width * U_HEIGHT/(2*U_WIDTH) - img_1->height/2; 
                
                Output.range = float(U_WIDTH * img_1->width) / obj_pix_width / TAN_FOV_X;
                Output.real_x = float(pix_x * U_HEIGHT) / obj_pix_width;
                Output.real_y = float(pix_y * U_WIDTH) / obj_pix_width;
        
                // if (range > ??) goto RETURN_ERROR;
                
                if (!(flags & _QUIET)) {
                    printf ("  vision_U: Single Horiz Segements.\n");
                    printf ("  vision_U: Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
                    printf ("  vision_U: Range: %f\n", Output.range);
                }
                ret = DETECT_2;
            }
            else 
                goto RETURN_ERROR;
        }
        else { // vertical line
            /*pix_x = (cseed[0][0].x+cseed[0][1].x)/2;
            if ((pix_x > img_1->width/5) && (pix_x < img_1->width*5/6)) 
              */  goto RETURN_ERROR;
        }
    }
    else if (nseeds == 2) {
        float dy1 = ABS(cseed[0][1].y-cseed[0][0].y), dy2 = ABS(cseed[1][1].y-cseed[1][0].y);
        float dx1 = ABS(cseed[0][1].x-cseed[0][0].x);
        float v_len_ratio = dy1 / (dy2+1);
        
        /// check we are looking at 2 verticle lines 
        if (v_len_ratio < 1.4 && v_len_ratio > 0.75 && dy1 > 4*dx1) {
            // average the points to get center of U
            pix_x = (cseed[0][0].x+cseed[0][1].x+cseed[1][0].x+cseed[1][1].x)/4 - img_1->width/2;
            pix_y = (cseed[0][0].y+cseed[0][1].y+cseed[1][0].y+cseed[1][1].y)/4 - img_1->height/2;
            int obj_pix_width = (fabs(cseed[0][0].x-cseed[1][0].x)+fabs(cseed[0][1].x-cseed[1][1].x))/2;
    
            // estimate range using horiz seperation
            Output.range = float(U_WIDTH * img_1->width) / obj_pix_width / TAN_FOV_X;
            Output.real_x = float(pix_x * U_HEIGHT) / obj_pix_width;
            Output.real_y = float(pix_y * U_WIDTH) / obj_pix_width;
        
            if (!(flags & _QUIET)) {
                printf ("  vision_U: Two Segements Detected.\n");
                printf ("          : Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
                printf ("          : Range: %f\n", Output.range);
            }
            ret = DETECT_3;
        }
        else { // 1 horiz and 1 vertical line  
            // dont need to do anything
        } 
    }
    else if (nseeds > 2) {
        int i1=0, i2=0, i3=0; // first,2nd,4rd longest lines
        float sl1=0,sl2=0,sl3=0; // square length of each longest line
        float temp;
        
        for (int i = 0; i < nseeds; i++) { // find the 3 longest lines
            temp = sqr_length(cseed,i);
            if (temp > sl1) {
                i3 = i2; sl3 = sl2;
                i2 = i1; sl2 = sl1;
                i1 = i; sl1 = temp;
            }
            else if (temp > sl2) {
                i3 = i2; sl3 = sl2;
                i2 = i; sl2 = temp;               
            }
            else if (temp > sl3) {
                i3 = i; sl3 = temp;
            }
        }
        
        int long1 = i1; // legacy code thingy
        
        int dx = ABS(cseed[long1][1].x-cseed[long1][0].x)+1;
        int dy = ABS(cseed[long1][1].y-cseed[long1][0].y)+1;
        int dx2 = ABS(cseed[i2][1].x-cseed[i2][0].x)+1;
        int dy2 = ABS(cseed[i2][1].y-cseed[i2][0].y)+1;
        int dx3 = ABS(cseed[i3][1].x-cseed[i3][0].x)+1;
        int dy3 = ABS(cseed[i3][1].y-cseed[i3][0].y)+1;
        
        if ((float(dx)/dy > 5) && // longest line = horiz
            (float(dy2)/dx2 > 4) && (float(dy3)/dx3 > 4) && // other 2 lines = vert
            (cseed[i2][1].y > cseed[long1][0].y) && (cseed[i3][1].y > cseed[long1][0].y)) // vert segments above horiz
        {
            // estimate range using vertical line seperation
            pix_x = (cseed[i2][0].x+cseed[i2][1].x+cseed[i3][0].x+cseed[i3][1].x)/4 - img_1->width/2;
            pix_y = (0.5*(cseed[long1][0].y+cseed[long1][1].y)+cseed[i2][0].y+cseed[i3][0].y)/3 
		    - (img_1->height - dy2)/2;
            int obj_pix_width = (ABS(cseed[i2][0].x-cseed[i3][0].x)+ABS(cseed[i2][1].x-cseed[i3][1].x))/2;
    
            Output.range = float(U_WIDTH * img_1->width) / obj_pix_width / TAN_FOV_X;
            Output.real_x = float(pix_x * U_HEIGHT) / obj_pix_width;
            Output.real_y = float(pix_y * U_WIDTH) / obj_pix_width;
        
            if (!(flags & _QUIET)) {
                printf ("  vision_U: Full 3 Segements Detected.\n");
                printf ("  vision_U: Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
                printf ("  vision_U: Range: %f\n", Output.range);
            }
            ret = DETECT_3;
        }
   }
    else 
        goto RETURN_ERROR;
   
    if (ret == ERROR) { // no idea what the object is
        /** can only return pixel space info based on img centroid */
        if (!(flags & _QUIET)) printf ("  vision_U: Cant Identify Object.\n");
        CvPoint img_centroid = calcImgCentroid (img_1);  
        Output.real_x = 2*float(img_centroid.x) / img_1->width;
        Output.real_y = 2*float(img_centroid.y) / img_1->height; 
    
        ret = DETECT_1;
    }
    
    RETURN_ERROR:
    cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage); // no leakingz
    for (int i = 0; i < nseeds; i++) 
        delete cseed[i];
    delete cseed;
    return ret;
}

/* ##############################################################################################
   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   ############################################################################################## */

#define DONE_RANGE 200
#define FWD_SPEED 1
#define SINK_SPEED 2
#define TURN_SPEED 2
#define TURN_SPEED2 3
void controller_U (vision_in &Input, Mission &m) {
// return state guide:
//  ERROR = bad, disregard this image
//  NO_DETECT = no detection (not enough pixels to constitute image, or no lines detected)
//  DETECT_1 = sufficient pixels, but unable to recognize object. real_x,real_y refer to pixel 
//      coordinates of the image centroid. No other valid output.
//      The returned pix_x will have origin at the center of the img and normalized so a 
//      pixel at the edge of the image has pix_x == 1.0;
//  DETECT_2 = object recognized as U, but can only see part of it. real_x,real_y refer to real
//      space coordinates of U center (best guess), range is valid. 
//  DETECT_3 = object recognized as U and all parts visible. Same as DETECT_2 but
//      real_x,real_y should be very accurate
    retcode vcode;    
    vision_out Output;
    vcode = vision_U (Input, Output, _INVERT | _DISPLAY | _QUIET);
    static PI_Controller PI_x, PI_y; // for NO_BUOY and GOOD states, respectively
    PI_x.setK1K2 (-24.0/PI, -0.6); // K1 is set so output is 0 if input is tan(30degrees)
    PI_y.setK1K2 (-24.0/PI, -0.6);
    
    enum ESTATE {NO_OBJ, GOOD, DONE};
    static ESTATE state = NO_OBJ;    
    /** State Guide:
     * The sub starts in NO and travels foward. If it sees something, it goes to GOOD to 
     * home in on that object.
     * Once we are close enough to the object, go to CHARGE, which will charge at the OBJ to go thru it
     * we treat DETECT_2 and DETECT_3 basically the same
     */
    
    if (state > (ESTATE)2 || state < (ESTATE)0) /// I DONT KNOW WHY BUT WE HAVE TO HAVE THIS OR state initializes to junk value
        state = NO_OBJ;
    
    if (state == NO_OBJ) {
        if (vcode == DETECT_2 || vcode == DETECT_3) { // found something!
            state = GOOD;
            PI_x.reset(); // reset PI controllers
            PI_y.reset();
        }
    }
    else if (state == GOOD) {
        if ((vcode == NO_DETECT) || (vcode == DETECT_1)) { // lost object
            state = NO_OBJ;
        }
        else if (Output.range < DONE_RANGE) // we are close enough to be considered done
            state = DONE;
    }
    
    float angle_x, angle_y;
    /// output logic
    switch (state) {
        case NO_OBJ:
            printf ("   U: NO_OBJ\n");
            m.move (STOP);
            m.move (FORWARD, FWD_SPEED);
            
            if (vcode == DETECT_1) {
                printf ("   U: Homing on Centroid\n");
                if (Output.real_x > 0.7) m.move (RIGHT, TURN_SPEED2);
                else if (Output.real_x > 0.3) m.move (RIGHT, TURN_SPEED);
                else if (Output.real_x < -0.7) m.move (LEFT, TURN_SPEED2);
                else if (Output.real_x < -0.3) m.move (LEFT, TURN_SPEED);
            
                if (Output.real_y > 0.7) m.move (RISE, 2*SINK_SPEED);
                else if (Output.real_y > 0.3) m.move (RISE, SINK_SPEED);
                else if (Output.real_y < -0.7) m.move (SINK, 2*SINK_SPEED);
                else if (Output.real_y < -0.3) m.move (SINK, SINK_SPEED);
            }
            return;
        case GOOD:
            printf ("   U: DETECTED\n");
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
            return;
        case DONE:
            printf ("DONE!!\n");
            m.move (STOP);
            m.move (FORWARD, 1);
            return;
        default:
            printf ("\n\n I dont know what state Im in!!\n");
            while (1);
    }
}

