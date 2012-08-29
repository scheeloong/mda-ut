#include "preproc_filters.h"
#include <stdio.h>

int main (int argc, char** argv) {
    IplImage *img = cvLoadImage (argv[1], 1);
    IplImage *dst;
    
    HSV_settings HSV (50, 90, 20, 250, 20, 250);
    
    //HSV_filter (img, dst, HSV);
    HSV_adjust_filter (img, dst, HSV, 18, 1);
    
    return 0;
}