#include <stdio.h>
#include "task_gate.h"
#include "common.h"

/**######### MAIN GATE TASKK ############*/
retcode vision_GATE (vision_in &Input, vision_out &Output, char flags) {
// Will change real_x, real_y, range in Output.
// return state guide:
//  ERROR = bad, disregard this image
//  NO_DETECT = no detection (not enough pixels to constitute image, or no lines detected)
//  DETECT_1 = partial detection (1 segment detected) gateX, gateY correspond to segment center. Range valid
//  DETECT_2 = full detection (2 segments detected). gateX, gateY = gate center. Range valid
/** HS filter to extract gate object */
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
    if (pix_fraction < 0.002) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_GATE: Pixel Fraction %5.3f Too Low. Exiting.\n", pix_fraction);
        return NO_DETECT;
    }

/** probabilistic Hough line finder. Determine the threshold using the number of high pixels */
    int thresh = (int)(sqrt(pix_fraction*img_1->imageSize * GATE_SKINNYNESS) / 4.0); // guessed length of gate segment in pixels
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
        printf ("  vision_GATE: No Lines Detected. Exiting.\n");
        return NO_DETECT;
    }
    
/** Remove all horizontal lines, since we are interested only in vertical part of gate
    Reorient vertical lines so the smaller y value comes first */
    CvPoint* temp; int swap;
    for (int i = nlines-1; i >= 0; i--) { // for each line
        temp = (CvPoint*)cvGetSeqElem(lines, i);  
 
        if (fabs(temp[1].y-temp[0].y) < fabs(temp[1].x-temp[0].x)) {  // horiz line
            cvSeqRemove(lines,i); // if it is a horiz line, delete it 
        }
        else if (temp[0].y > temp[1].y) { // sort so lower Y value comes first
            swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
            swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
        }
    }
/** recheck that there are lines found */
    nlines=lines->total; // recalculate number of lines
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_GATE: No Lines Detected. Exiting.\n");
        return NO_DETECT;
    }
    if (!(flags & _QUIET)) printf ("Lines: %d\n", nlines);

/** perform clustering */
    int nseeds=0;
    CvPoint** cseed=0;  // this memory needs to be freed later
    //float min_valid = KMcluster_auto_K (cseed, nseeds, 1,2, lines, nlines, 2, _QUIET);
    KMcluster_auto_K (cseed, nseeds, 1,2, lines, nlines, 2, _QUIET);
    cvReleaseMemStorage (&storage);    

// display clustered lines
    if (flags & _DISPLAY) {
        for (int i = 0; i < nseeds; i++) 
            cvLine (img_1, cseed[i][0],cseed[i][1], CV_RGB(0,50,50), 2);
        cvShowImage(Input.window[1], img_1);
    }
    
/** decide on how many segments detected, return results */
// perform some sort of decision using min_valid

    const float obj_real_width = 54, obj_real_height = 40;
    int pix_x, pix_y; // x and y location and Range of target in pixel space
    retcode ret = ERROR;
    
    if (nseeds == 1) { // if lines are very close (< guessed line length), only 1 line visible
        pix_x = (cseed[0][0].x+cseed[0][1].x)/2 - img_1->width/2;
        pix_y = (cseed[0][0].y+cseed[0][1].y)/2 - img_1->height/2;
        int obj_pix_height = fabs(cseed[0][0].y-cseed[0][1].y);
    
        // estimate range using vertical length
        Output.range = obj_real_height * float(img_1->width) / (obj_pix_height * TAN_FOV_X);
        Output.real_x = pix_x * obj_real_height / obj_pix_height;
        Output.real_y = pix_y * obj_real_height / obj_pix_height;
        
        if (!(flags & _QUIET)) {
            printf ("  vision_GATE: One Segement Detected.\n");
            printf ("  vision_GATE: Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
            printf ("  vision_GATE: Range: %f\n", Output.range);
        }
        
        ret = DETECT_1;
    }
    else {
        // average the endpoints to get center of gate
        pix_x = (cseed[0][0].x+cseed[0][1].x+cseed[1][0].x+cseed[1][1].x)/4 - img_1->width/2;
        pix_y = (cseed[0][0].y+cseed[0][1].y+cseed[1][0].y+cseed[1][1].y)/4 - img_1->height/2;
        int obj_pix_width = (fabs(cseed[0][0].x-cseed[1][0].x)+fabs(cseed[0][1].x-cseed[1][1].x))/2;
    
        // estimate range with horiz width
        Output.range = obj_real_width * float(img_1->width) / obj_pix_width / TAN_FOV_X;
        Output.real_x = pix_x * obj_real_width / obj_pix_width;
        Output.real_y = pix_y * obj_real_width / obj_pix_width;
        
        if (!(flags & _QUIET)) {
            printf ("  vision_GATE: Two Segements Detected.\n");
            printf ("  vision_GATE: Lateral Pos: %f , %f\n", Output.real_x, Output.real_y);
            printf ("  vision_GATE: Range: %f\n", Output.range);
        }
        
        ret = DETECT_2; 
    }
   
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

void controller_GATE (vision_in &Input, Mission &m) {
// the gate task. We have to assume we are pointed approximate at the gate from the beginning.        
    retcode vcode;
    // vcode table:
    // -1 = error, disregard this image
    // 0 = no detection (not enough pixels to constitute image, or no lines detected). Probably have to move closer.
    // 1 = partial detection (1 segment detected)  gateX, gateY correspond to single segment center. Range valid
    // 2 = full detection (2 segments detected).   gateX, gateY = gate center. Range valid
    // 3 = gate very close, (dist between segments > 3/4 image width). Should stop using vision to navigate. data same as 2.
    //     Alternatively can ignore this and use the range.
    
//    Input.HSV.setAll(0, 50, 10, 255, 0, 255);
    vision_out Output;
    vcode = vision_GATE (Input, Output, _INVERT);
  
    /** Control code starts here */
    
    /* TODO: Re-implement old control code */
    // Input: vcode from the above call to vision_GATE and Output
    // Output: give commands to the submarine, ie:
    m.translate(FORWARD);

    // old control code    
    /** state machine
     * F: "Gate too far"
     * r: "Gate to the Right" (go fowards)
     * R: "Gate to the Right" (go right)
     * l: "Gate to the left" (go fowards)
     * L: "Gate to the Left" (go left)
     * C: "Gate is centered"
     * S: "Reached front of gate. Stop vision"
     * X: "Error state" 
    */
    /*
    if (state == 0) // initiation character for controllers
        state = 'F';

    if (vcode == 2) {
        if (Output.range < 3.0) state = 'S';
        else state = 'C';
	m.translate(RIGHT);
    }
    else if (vcode == 3) state = 'S';
    else if (vcode == -1) state = 'X';
    else
    switch (state) {
        case 'F': // gate too far
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'l';
            }
            break;//     //float posX, posZ;

   //cos((angle.yaw*PI)/180)
            // if vcode == 0 then case == 'F'
        case 'C': // gate centered
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'l';
            };
            break;
            // if vcode == 2 then case == 'C'
        case 'r':
            if (vcode == 1) { 
                if (gateX > 0) state = 'R';
                else state = 'l';
            } 
            else if (vcode == 0) state = 'R';
            break;
        case 'l':
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'L';
            }
            else if (vcode == 0) state = 'L';
            break;
        case 'R':
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'l';
            }
            else if (vcode == 0) state = 'r';
            break;
         case 'L':
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'l';
            }
            else if (vcode == 0) state = 'l';
            break;
    }
    
    switch (state) {
        case 'F': case 'r': case 'l': return 'w';
        case 'L': return 'a';
        case 'R': return 'd';
        case 'C': 
            if (gateX > 30) return 'd';
            else if (gateX < -30) return 'a';
            else return 'w';
        case 'S':
            printf ("Gate Task Complete!\n");
            return '/';
        case 'X':
            printf ("Gate Error\n");
            return '/';
        default:
            return '\0';
    }
    */
}

