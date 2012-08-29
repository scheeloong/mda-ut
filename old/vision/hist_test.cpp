#include "cv.h"
#include "highgui.h"

#include "calc_draw.h"
#include "histograms.h"

#define HUE_MAX 70
#define HUE_MIN 60
#define SAT_MIN 90

#define _DISPLAY 1 

void adjustHS (CvHistogram* hist, int h_guess, int s_min) {
    // first set the 3x3 square around max to zero
    
    int max_ind[2];  // indices of the histogram maximum
    cvGetMinMaxHistValue (hist, 0, 0, 0, max_ind);
    float *ptr = cvGetHistValue_2D(hist, max_ind[0],max_ind[1]); 
    //printf ("%d %d\n", max_ind[0], max_ind[1]);
    
    int h_rad = 3, s_rad = 6, sbound;
    float *temp, hbound;
    
    for (int h = -h_rad; h <= h_rad; h++) { // we step thru x values, use ellipse eqn to solve for y bounds
        temp = ptr + h*hist->mat.dim[0].step/4;
        hbound = float(h)/h_rad;
        sbound = cvRound(s_rad*sqrt(1.0 - hbound*hbound)); // eqn for ellipse
        printf("%f\n",hbound);
        for (int s = -sbound; s <= sbound; s++) // step thru all y values between bounds
            *(temp + s) = 0;
    }
    /*
    for (int s = -s_rad; s <= s_rad; s++) { // we step thru x values, use ellipse eqn to solve for y bounds
        temp = ptr + s;
        sbound = float(s)/s_rad;
        hbound = cvRound(h_rad*sqrt(1.0 - sbound*sbound)); // eqn for ellipse
        printf("%d\n", hbound);
        for (int h = -hbound; h <= hbound; h++) // step thru all y values between bounds
            *(temp + h*hist->mat.dim[0].step/4) = 0;
    }
    */
    /*
    
    for (int i = -1; i <= 1; i++)                       // step thru 3 bins of hue
        for (int j = 0; j < hist->mat.dim[1].size; j++) // all bins saturation
           *(ptr + i*hist->mat.dim[0].step/4 + j) = 0; // divide by 4 cuz 4 bytes per mat element(?)       
       
    // now find the bin coords of h_guess, s_guess
    int h_step = hist->thresh[0][1] / hist->mat.dim[0].size; // hist range / hist n_bins
    int s_step = hist->thresh[1][1] / hist->mat.dim[1].size;
    
    int h_binnum = h_guess / h_step, s_binnum = s_min / s_step;
    ptr = cvGetHistValue_2D(hist, h_binnum,s_binnum);   
        printf ("%d %d\n", h_binnum, s_binnum);
    *ptr = 0;
    */
}
    

int main( int argc, char** argv ) {
    IplImage* img = cvLoadImage( argv[1], CV_LOAD_IMAGE_ANYCOLOR); // load image with name argv[1], color
    
    CvHistogram* colorHist;
    
    histogram_HS (img, colorHist, 1);
    
    //cvNormalizeHist (colorHist, 100.0); // normalize to 1.0
    
// display histogram
   // adjustHS (colorHist, 70,88);
    displayHistogram2D (colorHist);
}