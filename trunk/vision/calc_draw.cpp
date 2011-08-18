#include "calc_draw.h"

// averages the 4 points of a rectangle to find center wrt image center
// rect is 4 elements, contains corner of the rect, offset is the output 
// positon of the center wrt the image center
CvPoint calcRectCenter (CvPoint2D32f* rect, IplImage* img, int origin_center) {
    int x = (int)(rect[0].x+rect[1].x+rect[2].x+rect[3].x) / 4;
    int y = (int)(rect[0].y+rect[1].y+rect[2].y+rect[3].y) / 4;
    
    if (origin_center == 0)
        return cvPoint (x,y);
    else
        return cvPoint (x-img->width/2, y - img->height/2);
}

// for the Lovers Lane thingy, the object will be an L shape. This calculates
// the "center" of the L shape given the 3 points that define it.
// All it does is find the 4th point and call calcRectCenter
CvPoint calcLCenter (CvPoint2D32f* L, IplImage* img, int origin_center) {
    int end0=0, end1=0; // end0 = upper endpoint, end1 = lower endpoint of the L
    int xAvg = 0;
    
    // the L can be an "L" or a "J" orientation. There is always a unique end0
    // but not an unique end1
    for (int i = 0; i < 3; i++) {
        if (L[i].y < L[end0].y) // assign to end0 index with smallest y value 
            end0 = i;
        xAvg += L[i].x;        // add all the x values
    }
    xAvg /= 3;                 // calc average
    for (int i = 0; i < 3; i++) {
        if (abs(L[i].x-xAvg) > abs(L[end1].x-xAvg))
            end1 = i; // assign to end1 index of greatest deviation from mean
    }
    
    if (end0 == end1) return cvPoint (-1,-1); // ERROR    
    int mid = 3 - end0 - end1; // mid = index that isnt end0 or end1

    L = (CvPoint2D32f*) realloc (L, 4*sizeof(CvPoint2D32f));
    L[3].x = L[end0].x - L[mid].x + L[end1].x;
    L[3].y = L[end0].y - L[mid].y + L[end1].y;
    return calcRectCenter (L, img, origin_center);
}

// given a line from cvHoughLines2, finds the perpendicular dist to it from img
// center. Assumes mat is CV_32FC2
// To do this, find the vector from point on line lp to img center. Then dot
// with line's unit direction vector
CvPoint calcLineDist (IplImage* img, float rad, float ang) {
    // x,y of lp 
    float lpx = rad*cos(ang), lpy = rad*sin(ang); // is it sin -ang
    // vector from lp to img center
    float Vx = img->width/2.0-lpx, Vy = img->height/2.0-lpy; 
    // dot product with unit direction vector
    float Dx = Vx*cos(90-ang), Dy = Vy*sin(90-ang);
    // now reference to img center
    return cvPoint (Dx-Vx, Dy-Vy);
}

// draw lines output by cvHoughLines2. Does so by calculating where line meets
// image edges. Must check to see if it meets x or y edge.
void drawHoughLines (IplImage* img, CvMat* lines, int display) {
    float rad, ang;
    float x[4], y[4];
    int mini, maxi, good1=-1, good2=-1;
    float* dataPtr; 
    
    for (int i=0; i < lines->rows; i++) {   
        // it has to be data.ptr, cast to float*. DO NOT USE data.fl
        dataPtr = (float*) (lines->data.ptr + i*lines->step); // beginning of ith row
        rad = *(dataPtr);  ang = *(dataPtr+1);
        //printf ("%f %f\n", rad, ang*180/CV_PI);
        
        if (fabs(ang - CV_PI/2.0) < 0.001) { x[0] = -9000;  x[2] = 9000; }
        else {
            x[0] = rad / cos(ang);  y[0] = 0; // intercept with y=0 line
            x[2] = (rad-img->height*sin(ang)) / cos(ang);  y[2] = img->height; // y = max(y) line
        }
        
        if (fabs(ang) < 0.001) { x[1] = -9000;  x[3] = 9000; }
        else {
            x[1] = 0;  y[1] = rad / sin(ang);  // x = 0 line
            x[3] = img->width;  y[3] = (rad-img->width*cos(ang)) / sin(ang); // x = max(x) line
        }

        // find the indices of the min and max x elements
        mini = maxi = 0;
        for (int i = 1; i < 4; i++) {
            if (x[mini] > x[i]) mini = i;
            else if (x[maxi] < x[i]) maxi = i;
        }
        // find the 2 non min/max elements
        for (int i = 0; i < 4; i++)
            if ((i != mini) && (i != maxi)) {
                if (good1 == -1)  good1 = i; 
                else good2 = i; 
            }
        
        // draw line using those 2 elements
        cvLine(img, cvPoint(int(x[good1]),int(y[good1])), cvPoint(int(x[good2]),int(y[good2])), cvScalar (100,200,200,0));
        //printf ("%d %d\n%d %d\n", int(x[2]),int(y[2]), int(x[3]),int(y[3]));      
    } 
    
    if (display) {
        cvNamedWindow ("drawHoughLines", CV_WINDOW_AUTOSIZE);
        cvShowImage ("drawHoughLines", img);
        cvWaitKey(0);
        cvDestroyWindow ("drawHoughLines");
    }
}
