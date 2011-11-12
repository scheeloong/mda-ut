#include <stdio.h>
#include "cv_tasks.h"

/**######### HELPER FUNCTIONS ###########*/
int sqr_length (CvPoint** clusters, int i) {
// returns square length of index'th line
    int dx = clusters[i][0].x-clusters[i][1].x,  dy = clusters[i][0].y-clusters[i][1].y;
    return dx*dx + dy*dy;
}
float tangent_angle (CvPoint** clusters, int i) {
    return float(clusters[i][0].y-clusters[i][1].y) / (clusters[i][0].x-clusters[i][1].x);
}

/**######### MAIN GATE TASKK ############*/
char vision_GATE (IplImage* img, int &gateX, int &gateY, float &range, 
                  HSV_settings HSV, char* window[], char flags) {
// return state guide:
// -1 = bad, disregard this image
// 0 = no detection (not enough pixels to constitute image, or no lines detected)
// 1 = partial detection (1 segment detected) gateX, gateY correspond to segment center. Range valid
// 2 = full detection (2 segments detected). gateX, gateY = gate center. Range valid
// 3 = gate very close, (dist between segments > 3/4 image width). Otherwise same as 2
/** HS filter to extract gate object */
    IplImage* img_1;  
    HSV_Filter (img, img_1, HSV);
    
    IplConvKernel* kernel = cvCreateStructuringElementEx (3,3,1,1,CV_SHAPE_RECT);
    cvMorphologyEx (img_1, img_1, NULL, 
                    kernel, CV_MOP_OPEN);
    cvMorphologyEx (img_1, img_1, NULL, 
                    kernel, CV_MOP_CLOSE);
    cvReleaseStructuringElement (&kernel);
    
    if (flags & _INVERT) img_1->origin = 1;
    if (flags & _DISPLAY) cvShowImage(window[0], img_1);

    // check to see if there are enough pixels to do line finding
    int pix = cvCountNonZero (img_1); 
    //printf ("  vision_GATE: Pix Fraction: %f\n", float(pix)/img->width/img->height); 
    if (float(pix)/img_1->width/img_1->height < 0.005) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_GATE: Pixel Fraction Too Low. Exiting.\n");
        return 0;
    }

/** probabilistic Hough line finder. Determine the threshold using the number of high pixels */
    int thresh = (int)(sqrt(pix/GATE_SKINNYNESS) / 4.0); // guessed length of gate segment in pixels
    CvMemStorage* storage = cvCreateMemStorage(0); // create memstorage for line finidng, delete later
    CvSeq* lines = 0;
    
    int minlen=30, mindist=30;
    lines = cvHoughLines2(img_1, storage,
        CV_HOUGH_PROBABILISTIC,
        2, CV_PI/180.0,
        thresh, minlen, mindist);

/** check if lines were found, if not quit */
    int nlines=lines->total;
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_GATE: No Lines Detected. Exiting.\n");
        return 0;
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
            //cvLine (img_1, temp[0],temp[1], CV_RGB(100,200,100), 1);
    }
    //cvShowImage(window[1], img_1);
/** recheck that there are lines found */
    nlines=lines->total; // recalculate number of lines
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_GATE: No Lines Detected. Exiting.\n");
        return 0;
    }
    if (!(flags & _QUIET)) printf ("Lines: %d\n", nlines);

/** perform clustering */
    int nseeds=0;
    CvPoint** cseed=0;  // this memory needs to be freed later
    float min_valid = KMcluster_auto_K (cseed, nseeds, 1,2, lines, nlines, 2, _QUIET);
    
// display clustered lines
    if (flags & _DISPLAY) {
        for (int i = 0; i < nseeds; i++) 
            cvLine (img_1, cseed[i][0],cseed[i][1], CV_RGB(0,50,50), 2);
        cvShowImage(window[1], img_1);
    }
    
/** decide on how many segments detected, return results */
// perform some sort of decision using min_valid

    const float obj_real_width = 54, obj_real_height = 40;
    int ret = -1;
    
    if (nseeds == 1) { // if lines are very close (< guessed line length), only 1 line visible
        gateX = (cseed[0][0].x+cseed[0][1].x)/2 - img_1->width/2;
        gateY = (cseed[0][0].y+cseed[0][1].y)/2 - img_1->height/2;
        int obj_pix_height = fabs(cseed[0][0].y-cseed[0][1].y);
    
        // estimate range using vertical length
        range = obj_real_height * float(img_1->height) / obj_pix_height / TAN_FRONT_FOV;
        if (!(flags & _QUIET)) {
            printf ("  vision_GATE: One Segement Detected.\n");
            printf ("  vision_GATE: Range: %f\n", range);
        }
        
        ret = 1;
    }
    else {
        // average the endpoints to get center of gate
        gateX = (cseed[0][0].x+cseed[0][1].x+cseed[1][0].x+cseed[1][1].x)/4 - img_1->width/2;
        gateY = (cseed[0][0].y+cseed[0][1].y+cseed[1][0].y+cseed[1][1].y)/4 - img_1->height/2;
        int obj_pix_width = (fabs(cseed[0][0].x-cseed[1][0].x)+fabs(cseed[0][1].x-cseed[1][1].x))/2;
    
        // estimate range with horiz width
        range = obj_real_width * float(img_1->width) / obj_pix_width / TAN_FRONT_FOV;
        if (!(flags & _QUIET)) {
            printf ("  vision_GATE: Two Segements Detected.\n");
            printf ("  vision_GATE: Range: %f\n", range);
        }
        
        if (obj_pix_width > img_1->width*0.8) // line seperation > 0.8 of image width 
            ret = 3; // too close
        else ret = 2; 
    }
   
    cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage); // no leakingz
    for (int i = 0; i < nseeds; i++) 
        delete cseed[i];
    delete cseed;
    return ret;
}

/**######### SQUARE WINDOW TASKK ############*/
char vision_SQUARE (IplImage* img, int &gateX, int &gateY, float &range, 
                    HSV_settings HSV, char* window[], char flags) {
// return state guide:
// -1 = bad, disregard this image
// 0 = no detection (not enough pixels to constitute image, or no lines detected)
// 1 = one corner, two segments. gateX, gateY = direction to center of object. Range invalid
// 2 = two corners, three segments. gateX, gateY = direction to center of object. Range invalid
// 3 = four corners, four segments gateX, gateY = center of object. Range valid.
/** HS filter to extract gate object */
    IplImage* img_1;  
    HSV_Filter (img, img_1, HSV); // need to delete
    
    IplConvKernel* kernel = cvCreateStructuringElementEx (3,3,1,1,CV_SHAPE_RECT);
    cvMorphologyEx (img_1, img_1, NULL, 
                    kernel, CV_MOP_OPEN);
    cvMorphologyEx (img_1, img_1, NULL, 
                    kernel, CV_MOP_CLOSE);
    cvReleaseStructuringElement (&kernel);
    
    if (flags & _INVERT) img_1->origin = 1;
    if (flags & _DISPLAY) cvShowImage(window[0], img_1);

    // check to see if there are enough pixels to do line finding
    int pix = cvCountNonZero (img_1); 
    //printf ("  vision_GATE: Pix Fraction: %f\n", float(pix)/img->width/img->height); 
    if (float(pix)/img_1->width/img_1->height < 0.002) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_GATE: Pixel Fraction Too Low. Exiting.\n");
        return 0;
    }

/** probabilistic Hough line finder. Determine the threshold using the number of high pixels */
    int thresh = (int)(sqrt(pix/GATE_SKINNYNESS) / 4.0); // guessed length of gate segment in pixels
    CvMemStorage* storage = cvCreateMemStorage(0); // create memstorage for line finidng, delete later
    CvSeq* lines = 0;
    
    int minlen=30, mindist=30;
    lines = cvHoughLines2(img_1, storage,
        CV_HOUGH_PROBABILISTIC,
        2, CV_PI/180.0,
        thresh, minlen, mindist);

/** check if lines were found, if not quit */
    int nlines=lines->total;
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_GATE: No Lines Detected. Exiting.\n");
        return 0;
    }
    if (!(flags & _QUIET)) printf ("Lines: %d\n", nlines);
    
/** Organize lines so first endpoint of vertical line is the upper one, first endpoint
 of horizontal line is left one */
    CvPoint* temp; int swap;
    for (int i = nlines-1; i >= 0; i--) { // for each line
        temp = (CvPoint*)cvGetSeqElem(lines, i);  
 
        if (fabs(temp[1].y-temp[0].y) < fabs(temp[1].x-temp[0].x)) {  // horiz line 
            if (temp[0].x > temp[1].x) { // sort so lower X value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
        }
        else if (temp[0].y > temp[1].y) { // sort so lower Y value comes first
            swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
            swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
        }
    }

/** perform clustering, line intersect finding */
    int nseeds=0;
    CvPoint** cseed=0;  // this memory needs to be freed later
    KMcluster_auto_K (cseed, nseeds, 1,5, lines, nlines, 2, flags);
    CvPoint* intersects;
    int nIntersects = lineSegment_intersects (cseed, nseeds, cvGetSize(img_1), intersects);
    //printf ("%d\n", nIntersects);
    
// display clustered lines
    if (flags & _DISPLAY) {
        for (int i = 0; i < nIntersects; i++)
            cvCircle (img_1, intersects[i], 5, CV_RGB(0,100,100), -1);
        for (int i = 0; i < nseeds; i++) 
            cvLine (img_1, cseed[i][0],cseed[i][1], CV_RGB(0,50,50), 2);
        cvShowImage(window[1], img_1);
    }
    
/** decide on how many segments detected, return results */
    const float obj_real_width = 54, obj_real_height = 40;
    float obj_pix_width=-1, obj_pix_height=-1;
    int ret = -1;

    if (nseeds == 4) { // there are 4 lines visible
        if (nIntersects == 4) { // there are 4 intersects
            //if (!(flags & _QUIET)) printf ("  vision_SQUARE: Successful\n"); // if user did not specify QUIET flag, print msg
            gateX = (intersects[0].x+intersects[1].x+intersects[2].x+intersects[3].x)/4 - img_1->width/2; // gate center pos
            gateY = (intersects[0].y+intersects[1].y+intersects[2].y+intersects[3].y)/4 - img_1->height/2;
            
            obj_pix_width = (fabs(intersects[0].x-intersects[1].x) + fabs(intersects[2].x-intersects[3].x))/2;
            ret = 1; 
        }
        else ret = -1; // 4 lines visible but not 4 intersects -> some sort of error
    } 
    /** the decision structure continues here */
    // else if...
    
    /** end decision structure */
    
    if (obj_pix_width > 0) { // width
        range = obj_real_width * float(img_1->width) / obj_pix_width / TAN_FRONT_FOV;
        if (!(flags & _QUIET)) printf ("  vision_GATE: Range: %f\n", range);
    }
    else if (obj_pix_height > 0) { // height
        range = obj_real_height * float(img_1->height) / obj_pix_height / TAN_FRONT_FOV;
        if (!(flags & _QUIET)) printf ("  vision_GATE: Range: %f\n", range);
    }
    
    cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage); // no leakingz
    for (int i = 0; i < nseeds; i++) 
        delete cseed[i];
    delete cseed;
    delete [] intersects;
    return ret;
}

/**######### PATH FOLLOW TASKK ############*/
char vision_PATH (IplImage* img, int &pathX, int &pathY, float &tan_angle, float &length, 
                  HSV_settings HSV, char*window[], char flags) {
// state guide:
// 0 = no detection (obj not in view)
// 1 = partial detection (pipe found, but too much off to the side)
// 2 = full detection (pipe oriented almost directly below)
/** HS filter to extract object */
    IplImage* img_1;  
    HSV_Filter (img, img_1, HSV); // need to delete img_1
    IplConvKernel* kernel = cvCreateStructuringElementEx (3,3,1,1,CV_SHAPE_RECT);
    cvMorphologyEx (img_1, img_1, NULL, 
                    kernel, CV_MOP_OPEN);
    cvMorphologyEx (img_1, img_1, NULL, 
                    kernel, CV_MOP_CLOSE);
    cvReleaseStructuringElement (&kernel);
    
    if (flags & _INVERT) img_1->origin = 1;
    if (flags & _DISPLAY) cvShowImage(window[0], img_1);

    // check to see if there are enough pixels to do line finding
    int pix = cvCountNonZero (img_1); 
    //printf ("  vision_GATE: Pix Fraction: %f\n", float(pix)/img->width/img->height); 
    if (float(pix)/img_1->width/img_1->height < 0.001) { // if nothing in the view
        cvReleaseImage (&img_1);
        printf ("  vision_PATH: Pixel Fraction Too Low. Exiting.\n");
        return 0;
    }
    
/** check image centroid to see if path is approximately centered */
    CvPoint img_centroid = calcImgCentroid (img_1);
    int Mx = img_centroid.x, My = img_centroid.y;
    if (sqrt(Mx*Mx + My*My) > img_1->height / 4) { // not close enough to center
        cvReleaseImage (&img_1);
        printf ("  vision_PATH: Pipe Detected. Not Centered.\n");
        pathX = Mx; pathY = My; 
    
        return 1;
    }
/** take gradient of image */    
    cvGradient_Custom (img_1, img_1, 3, 3, 1);
    //if (flags & _DISPLAY) cvShowImage(window[1], img_1);
    
/** probabilistic Hough line finder. Determine the threshold using the number of high pixels */
    int thresh = 10;//(int)(sqrt(pix/PATH_SKINNYNESS)); // guessed length of pipe in pixels
    CvMemStorage* storage = cvCreateMemStorage(0); // create memstorage for line finidng, delete later
    CvSeq* lines = 0;
    
    int minlen=20, mindist=50;
    lines = cvHoughLines2(img_1, storage,
        CV_HOUGH_PROBABILISTIC,
        1, CV_PI/360.0,
        thresh, minlen, mindist);
/** check if lines were found, if not quit */
    int nlines=lines->total;
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_PATH: No Lines Detected. Exiting.\n");
        return 0;
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
            //cvLine (img_1, temp[0],temp[1], CV_RGB(100,200,100), 1);
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
    int ret=-1;
    if (nseeds >= 2) { // definitely reject img if less than 2 lines
        int long1=0, long2=0; // long1 = index of longest line. long2 = index of second longest
        for (int i = 1; i < nseeds; i++)
            if (sqr_length(cseed,i) > sqr_length(cseed,long1)) long1 = i;
        for (int i = 1; i < nseeds && i != long1; i++) 
            if (sqr_length(cseed,i) > sqr_length(cseed,long2)) long2 = i;
            
        // now check that these two lines have similar lengths and angles
        float len1 = sqr_length(cseed,long1), len2 = sqr_length(cseed,long2); // calculate lengths and tan of angles
        float ang1 = tangent_angle(cseed,long1), ang2 = tangent_angle(cseed, long2);
        float len_ratio = len1 / len2; float tan_ratio = ang1 / ang2; // ratios
      //  printf ("Ratios:  %f  %f\n", len_ratio, tan_ratio);
        if ((len_ratio < 1.20) && (len_ratio > 0.8333) && (tan_ratio < 1.25) && (tan_ratio > 0.8)) {
        // lines are OK, we can return good results
            pathX = (cseed[long1][0].x+cseed[long1][1].x+cseed[long2][0].x+cseed[long2][1].x)/4 - img_1->width/2;  
            pathY = (cseed[long1][0].y+cseed[long1][1].y+cseed[long2][0].y+cseed[long2][1].y)/4 - img_1->height/2;
            length = sqrt(len1 / (img_1->width*img_1->width + img_1->height*img_1->height)); // divide len1 by hypotenuse of image
            tan_angle = (ang1+ang2) / 2;
            
            if (!(flags & _QUIET)) printf ("  vision_PATH: Successful.\n  Length&Angle:  %f  %f\n",length,atan(tan_angle));
            if (flags & _DISPLAY) {
                CvPoint p1 = cvPoint (pathX+img_1->width/2 - length*400, pathY+img_1->height/2 - length*400*tan_angle);
                CvPoint p2 = cvPoint (pathX+img_1->width/2 + length*400, pathY+img_1->height/2 + length*400*tan_angle);
                cvLine (img_1, p1, p2, CV_RGB(255,100,100), 2);
                cvShowImage(window[1], img_1);
            }            
            ret = 2;
        }
        else ret = 1;
    }

    cvReleaseMemStorage(&storage);
    cvReleaseImage (&img_1); 
    delete cseed[0]; delete cseed;
    return ret;
}




char controller_GATE (IplImage* img, char &state, char* window[]) {
    if (state == 0) // initiation character for controllers
        state = 'F';
    char vcode;
    int gateX, gateY; float range;
    
    HSV_settings HSV(0, 50, 10, 255, 0, 255);
    printf ("aa: %d %d\n", HSV.H_MIN, HSV.H_MAX);
    vcode = vision_GATE (img, gateX,gateY,range, HSV, window, _INVERT);
        
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
    
    if (vcode == 2) {
        if (range < 3.0) state = 'S';
        else state = 'C';
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
            break;
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
}

char controller_PATH (IplImage* img, char &state, char* window[]) {
    char vcode;
    int pathX, pathY; float tan_angle, length;
    
    HSV_settings HSV(10, 30, 130, 255, 40, 255);
    vcode = vision_PATH (img, pathX,pathY,tan_angle,length, HSV, window);
        
    /** state machine
     * F: "Path too far (cant see it)"
     * P: "Partial Detection"  -  go towards the path
     * c: "Fully centered high depth"  -  sink
     * 1: sink 1
     * 2: ..
     * 3: ..
     * 4: ..
     * 5: ..
     * D: "Correct Depth"  -  Recenter
     * C: "Centered at correct depth"  -  Align
     * A: "Aligned"  - stop
     * S: "Reached front of gate. Stop vision"
     * X: "Error state" 
    */
    
    enum ESTATE {NOPATH,PARTIAL,CENTERED, SINK, DPARTIAL,DCENTERED,STOP,ERR};
    ESTATE lookup[8][3] = {
            {NOPATH,  PARTIAL,   CENTERED},  // returns 'w'
            {NOPATH,  PARTIAL,   PARTIAL},  // tries to center, math
            {ERR,     PARTIAL,   SINK},        // sinks
/*SINK*/    {ERR,     DPARTIAL,  DCENTERED},  // sinks, needs math
/*DPARTIAL*/{ERR,     DPARTIAL,  DCENTERED},  // tries to center
/*DCENTER*/ {ERR,     DPARTIAL,  DCENTERED},  // need math
            {STOP,    STOP,      STOP},
            {ERR,     ERR,       ERR}};
    
    ESTATE estate = (ESTATE) state;  
    // the only state that needs additional processing is DCENTERED
    if (estate == PARTIAL && sqrt(pathX*pathX + pathY*pathY) < 30)
        estate = CENTERED;    
    else if (estate == DCENTERED) {
        if (fabs(tan_angle) < 0.05) // 3ish degrees
            estate = STOP;
    }
    else if ((estate == SINK) && (length < 100)) {
        // continue to sink
    }
    else estate = lookup[estate][(int)vcode];
    
    state = estate;
    printf ("%d\n",state);
    
    /** figure out output */
    switch (estate) {
        case NOPATH: return 'w';
        case PARTIAL: case DPARTIAL:
            if (fabs(pathY/(pathX+0.01)) < 11.5) { // if centroid outside of +-5 degrees from vertical
                if (pathX > 0) return 'd'; // turn towards the centroid
                else return 'a';
            }
            else return 'w'; // move towards centroid
        case CENTERED: case SINK:
            return 'k';
        case DCENTERED:
            if (tan_angle > 0) // first quadrant
                return 'd';
            else return 'a'; // second quadrant
        case STOP:
            printf ("PATH Task Complete!\n");
            return '/';
        case ERR:
            printf ("PATH Error\n");
            return '/';
    }
}







