#include <stdio.h>
#include "cv_tasks.h"

// this is for the gate task

char vision_GATE (IplImage* img, int &gateX, int &gateY, float &range, char* window[]) {
// return state guide:
// -1 = bad, disregard this image
// 0 = no detection (not enough pixels to constitute image, or no lines detected)
// 1 = partial detection (1 segment detected) XY segment center
// 2 = full detection (2 segments detected). XY gate center.
// 3 = gate very close, (dist between segments > 3/4 image width). XY gate center.
/** HS filter to extract gate object */
    IplImage* img_1;  
    HueSat_Filter1 (img, img_1, GATE_HMIN, GATE_HMAX, GATE_SMIN, GATE_SMAX); // need to delete
    img_1->origin = 1;
    cvShowImage(window[0], img_1);

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
            /*if (temp[0].x > temp[1].x) { // sort so lower X value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }*/
        }
        else {
            if (temp[0].y > temp[1].y) { // sort so lower Y value comes first
                swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
                swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
            }
            //cvLine (img_1, temp[0],temp[1], CV_RGB(100,200,100), 1);
        }
    }
    //cvShowImage(window[1], img_1);
/** recheck that there are lines found */
    nlines=lines->total; // recalculate number of lines
    if (nlines == 0) { 
        cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage);
        printf ("  vision_GATE: No Lines Detected. Exiting.\n");
        return 0;
    }
    printf ("Lines: %d\n", nlines);

/** perform clustering */
    int nseeds=0;
    CvPoint** cseed=0;  // this memory needs to be freed later
    KMcluster_auto_K (cseed, nseeds, 1,2, lines, nlines, 1);
    
// display clustered lines
    for (int i = 0; i < nseeds; i++) 
        cvLine (img_1, cseed[i][0],cseed[i][1], CV_RGB(0,50,50), 2);
    cvShowImage(window[1], img_1);

/** decide on how many segments detected, return results */
    int line_seperation = (fabs(cseed[0][0].x-cseed[1][0].x)+fabs(cseed[0][1].x-cseed[1][1].x))/2;
    int ret = -1;
    
    if (nseeds == 1) { // if lines are very close (< guessed line length), only 1 line visible
        printf ("  vision_GATE: One Segment Detected.\n");
        gateX = (cseed[0][0].x+cseed[0][1].x+cseed[1][0].x+cseed[1][1].x)/4 - img_1->width/2;
        gateY = (cseed[0][0].y+cseed[0][1].y+cseed[1][0].y+cseed[1][1].y)/4 - img_1->height/2;
        // estimate range using vertical length? bad?
        int h = ((cseed[0][1].y+cseed[1][1].y) - (cseed[0][0].y+cseed[1][0].y))/2;
        float tan_subtended_halfangle = float(h)/img_1->height * tan(FRONT_FOV*CV_PI/180/2);
        range = GATE_HEIGHT / 2.0 / tan_subtended_halfangle;
        printf ("  vision_GATE: Range: %f\n",range);
        ret = 1;
    }
    else {
        printf ("  vision_GATE: Successful.\n");
        // average the endpoints to get center of gate
        gateX = (cseed[0][0].x+cseed[0][1].x+cseed[1][0].x+cseed[1][1].x)/4 - img_1->width/2;
        gateY = (cseed[0][0].y+cseed[0][1].y+cseed[1][0].y+cseed[1][1].y)/4 - img_1->height/2;
        // estimate range
        float tan_subtended_halfangle = float(line_seperation)/img_1->width * tan(FRONT_FOV*CV_PI/180/2);
        range = GATE_WIDTH / 2.0 / tan_subtended_halfangle;
        printf ("  vision_GATE: Range: %f\n",range);
        
        if (line_seperation > img_1->width*0.8) // line seperation > 0.8 of image width 
            ret = 3;
        else ret = 2;
    }
    
    cvReleaseImage (&img_1);  cvReleaseMemStorage (&storage); // no leakingz
    for (int i = 0; i < nseeds; i++) 
        delete cseed[i];
    delete cseed;
    return ret;
}

char vision_PATH (IplImage* img, int &pathX, int &pathY, float &tan_angle, float &length, 
                  char*window[]) {
// state guide:
// 0 = no detection (obj not in view)
// 1 = partial detection (pipe found, but too much off to the side)
// 2 = full detection (pipe oriented almost directly below)
/** HS filter to extract object */
    IplImage* img_1;  
    HueSat_Filter1 (img, img_1, PATH_HMIN, PATH_HMAX, PATH_SMIN, PATH_SMAX); // need to delete
    img_1->origin = 1;
    cvShowImage(window[0], img_1);

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
    cvShowImage(window[1], img_1);
    
/** probabilistic Hough line finder. Determine the threshold using the number of high pixels */
    int thresh = (int)(sqrt(pix/PATH_SKINNYNESS)); // guessed length of pipe in pixels
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
/** arrange lines by Y value. Will bug if horizontal lines encountered. Assume no horiz lines. */
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
    KMcluster_auto_K (cseed, nseeds, 1,4, lines, nlines, 2);    // 2 iterations needed I think
// display clustered lines
    for (int i = 0; i < nseeds; i++) 
        cvLine (img_1, cseed[i][0],cseed[i][1], CV_RGB(0,50,50), 2);
    cvShowImage(window[1], img_1);
    
/** calculations and cleanup */
    if (nseeds == 2) {
        pathX = (cseed[0][0].x+cseed[0][1].x+cseed[1][0].x+cseed[1][1].x)/4 - img_1->width/2;  
        pathY = (cseed[0][0].y+cseed[0][1].y+cseed[1][0].y+cseed[1][1].y)/4 - img_1->height/2;
        int dy = ((cseed[0][1].y+cseed[1][1].y)-(cseed[0][0].y+cseed[1][0].y))/2;
        int dx = ((cseed[0][1].x+cseed[1][1].x)-(cseed[0][0].x+cseed[1][0].x))/2; 
        tan_angle = float(dx) / dy; // angle with vertical
        length = sqrt(dx*dx + dy*dy);
        printf ("  vision_PATH: Successful.\n  Length: %f\n",length);
    }
    
    cvReleaseMemStorage(&storage);
    cvReleaseImage (&img_1); 
    delete cseed[0]; delete cseed;
    return 2;
}


char controller_GATE (IplImage* img, char &state, char* window[]) {
    if (state == 0) // initiation character for controllers
        state = 'F';
    char vcode;
    int gateX, gateY; float range;
    
    vcode = vision_GATE (img, gateX,gateY,range, window);
        
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
    
    vcode = vision_PATH (img, pathX,pathY,tan_angle,length, window);
        
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




