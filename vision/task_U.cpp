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
//  DETECT_3 = object recofnized as U and all parts visible. Same as DETECT_2 but
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
    int thresh = 60; //(int)(sqrt(pix_fraction*img_1->imageSize * U_SKINNYNESS) / 4.0); 
    CvMemStorage* storage = cvCreateMemStorage(0); // create memstorage for line finidng, delete later
    CvSeq* lines = 0;
    
    int minlen=img_1->height/10, mindist=minlen;
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
    
/** Orient vertical lines so first point has smaller y value, 
    same for horiz lines and x value */
    CvPoint* temp; int swap;
    for (int i = nlines-1; i >= 0; i--) { // for each line
        temp = (CvPoint*)cvGetSeqElem(lines, i);  
 
        if (fabs(temp[1].y-temp[0].y) < fabs(temp[1].x-temp[0].x)) {  // horiz line
	    if (temp[0].x > temp[1].x) {
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
	}
        else
	    if (temp[0].y > temp[1].y) { // sort so lower Y value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
    }

/** perform clustering */
    int nseeds=0;
    CvPoint** cseed=0;  // this memory needs to be freed later
    KMcluster_auto_K (cseed, nseeds, 1,3, lines, nlines, 2, _QUIET); // 1 to 3 segments
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
        if (float(dx)/dy > 5.0) { // horiz line, a bit more than tan(10deg)
            if (float(dx) > img_1->width*0.3) { // use horiz seperation to estimate range and (x,y)
                int obj_pix_width = cseed[0][1].x - cseed[0][0].x;
                pix_x = (cseed[0][1].x + cseed[0][0].x)/2 - img_1->width/2;
                // displace pix_y to estimated location of U center
                pix_y = (cseed[0][1].y + cseed[0][0].y)/2 + obj_pix_width * U_HEIGHT/(2*U_WIDTH) - img_1->height/2; 
                
                Output.range = U_WIDTH * float(img_1->width) / obj_pix_width / TAN_FOV_X;
                Output.real_x = pix_x * U_HEIGHT / obj_pix_width;
                Output.real_y = pix_y * U_WIDTH / obj_pix_width;
        
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
            pix_x = (cseed[0][0].x+cseed[0][1].x)/2;
            if ((pix_x > img_1->width/5) && (pix_x < img_1->width*5/6)) 
                goto RETURN_ERROR;
        }
    }
    else if (nseeds == 2) {
        float v_len_ratio = float(cseed[0][1].y-cseed[0][0].y) / (cseed[1][1].y-cseed[1][0].y+1);
        printf ("%f\n", v_len_ratio);
        if (v_len_ratio < 1.4 && v_len_ratio > 0.75) {// two vertical lines
        // average the points to get center of U
            pix_x = (cseed[0][0].x+cseed[0][1].x+cseed[1][0].x+cseed[1][1].x)/4 - img_1->width/2;
            pix_y = (cseed[0][0].y+cseed[0][1].y+cseed[1][0].y+cseed[1][1].y)/4 - img_1->height/2;
            int obj_pix_width = (fabs(cseed[0][0].x-cseed[1][0].x)+fabs(cseed[0][1].x-cseed[1][1].x))/2;
    
            // estimate range using horiz seperation
            Output.range = U_WIDTH * float(img_1->width) / obj_pix_width / TAN_FOV_X;
            Output.real_x = pix_x * U_HEIGHT / obj_pix_width;
            Output.real_y = pix_y * U_WIDTH / obj_pix_width;
        
            if (!(flags & _QUIET)) {
                printf ("  vision_U: Two Segements Detected.\n");
                printf ("  vision_U: Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
                printf ("  vision_U: Range: %f\n", Output.range);
            }
            ret = DETECT_3;
        }
        else { // 1 horiz and 1 vertical line  
            // dont need to do anything
        } 
    }
    else if (nseeds == 3) {
        int long1=0; // long1 = index of longest line
        for (int i = 1; i < nseeds; i++)
            if (sqr_length(cseed,i) > sqr_length(cseed,long1)) long1 = i;
            
        int i2 = (long1+1)%3,   i3 = (long1+2)%3; // index of other 2 lines
            
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
            pix_y = (cseed[long1][0].y+cseed[long1][1].y+cseed[i2][0].y+cseed[i3][0].y)/4 - img_1->height/2;
            int obj_pix_width = (ABS(cseed[i2][0].x-cseed[i3][0].x)+ABS(cseed[i2][1].x-cseed[i3][1].x))/2;
    
            Output.range = U_WIDTH * float(img_1->width) / obj_pix_width / TAN_FOV_X;
            Output.real_x = pix_x * U_HEIGHT / obj_pix_width;
            Output.real_y = pix_y * U_WIDTH / obj_pix_width;
        
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
        printf ("  vision_U: Cant Identify Object.\n");
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
// Control Code below
// 1. Each control function is expected to call its corresponding vision function, then use the
//    results to determine what to do.
// 2. The machine must output a character which controls the simulator (same as keyboard commands for now).
// 3. The machine can act on a state variable which will be pass in by reference. Basically this allows the 
//    controller function to behave as a finite state machine, with "state var" being the thing that records
//    what state the machine is in.
// 4. When first called "state" will have a value of 0.
// 5. The vision function will always be of the form:
//      char code = vision_X (img, data0,data1,data2,...,dataN, HSV, window, flags);
//    code = a code that tells you how well the object was identified (fully in view vs half in view, etc).
//           !! Also depending on the code the data may have different meanings. !!
//    img = IplImage* pointer. Just pass in the image.
//    data0...dataN = numbers that characterize the object being seen. You need to make decisions based on these.
//    HSV = the color filter settings. These are done for you.
//    window = names of windows to allow OpenCV to display stuff. Just pass in the ones passed to the control function.
//    flags = Bit flags for the vision function
//            _DISPLAY to display things
//            _INVERT to invert image (must set to properly display simulator imgs
//            _QUIET to suppress printed messages.
//            Multiple flags can be set using the | operator.
// 6. List of commands to send back:
//    'w','s' = foward / backward
//    'a','d' = turn left, right
//    'i','k' = rise, sink
//    The above is all the commands the sub can do in real life
//    'r','f' = roll left, roll right
//    't','g' = pitch foward, pitch backwards

void controller_U (vision_in &Input, Mission &m) {
// the gate task. We have to assume we are pointed approximate at the gate from the beginning.        
    retcode vcode;
    // vcode table:
    // -1 = error, disregard this image
    // 0 = no detection (not enough pixels to constitute image, or no lines detected). Probably have to move closer.
    
    vision_out Output;
    vcode = vision_U (Input, Output, _INVERT | _DISPLAY);
  
    /** Control code starts here */    
    //m.translate(FORWARD);
}

