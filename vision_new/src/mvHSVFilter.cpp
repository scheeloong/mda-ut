#include "mv.h"
#include "mgui.h"
#include <cv.h>   
    
mvHSVFilter:: mvHSVFilter (const char* settings_file) {
    read_common_mv_setting ("IMG_WIDTH_COMMON", IMG_WIDTH);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", IMG_HEIGHT);
    read_mv_setting (settings_file, "HUE_MIN", HMIN);
    read_mv_setting (settings_file, "HUE_MAX", HMAX);
    read_mv_setting (settings_file, "SAT_MIN", SMIN);
    read_mv_setting (settings_file, "SAT_MAX", SMAX);
    read_mv_setting (settings_file, "VAL_MIN", VMIN);
    read_mv_setting (settings_file, "VAL_MAX", VMAX);
    
    HMIN = (HMIN>=0) ? HMIN : HMIN+180; 
    HMAX = (HMAX<180) ? HMAX : HMAX-180; 
    
    HSVImg = cvCreateImage ( 
        cvSize(IMG_WIDTH,IMG_HEIGHT),
        IPL_DEPTH_8U,     // depth
        3                 // nChannels
    );
}

mvHSVFilter:: ~mvHSVFilter () {
    cvReleaseImage (&HSVImg);
}

void mvHSVFilter:: setHSV (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax) {
    if (hmin != _SAME_) HMIN = (hmin>=0) ? hmin : hmin+180; 
    if (hmax != _SAME_) HMAX = (hmax<180) ? hmax : hmax-180; 
    if (smin != _SAME_) SMIN = smin;
    if (smax != _SAME_) SMAX = smax;
    if (vmin != _SAME_) VMIN = vmin;
    if (vmax != _SAME_) VMAX = vmax;
}

int mvHSVFilter:: hueInRange (unsigned char hue) { // helper function for the filter
 if (HMAX >= HMIN) 
        return (hue >= HMIN && hue <= HMAX);
    else
        return ((hue <= HMIN && hue <= HMAX) || (hue >= HMIN && hue >= HMAX)); 
}
        
void mvHSVFilter:: filter (const IplImage* img, IplImage* result) {
    assert (img != NULL);
    assert (img->nChannels == 3);
    assert (result != NULL);
    assert (result->nChannels == 1);

    cvCvtColor (img, HSVImg, CV_BGR2HSV); // convert to HSV 

    /* go through each pixel, set the result image's pixel to 0 or 255 based on whether the
     * origin img's HSV values are withing bounds
     */
    unsigned char *imgPtr, *resPtr;
    
    for (int r = 0; r < result->height; r++) {                         
        imgPtr = (unsigned char*) (HSVImg->imageData + r*HSVImg->widthStep); // imgPtr = first pixel of rth's row
        resPtr = (unsigned char*) (result->imageData + r*result->widthStep);
        
        for (int c = 0; c < result->width; c++) {
            if (hueInRange (*imgPtr) && 
                (*(imgPtr+1) >= SMIN) && (*(imgPtr+1) <= SMAX) &&
                (*(imgPtr+2) >= VMIN) && (*(imgPtr+2) <= VMAX))
            {
                *resPtr = 255;
            }
            else 
                *resPtr = 0;
            
            imgPtr+=3; resPtr++;
        }
    }
}
