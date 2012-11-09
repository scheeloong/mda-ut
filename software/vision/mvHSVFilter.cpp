#include "mv.h"
#include "mgui.h"
#include <cv.h>   
    
#define FILTER_DEBUG
#ifdef FILTER_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

mvHSVFilter:: mvHSVFilter (const char* settings_file) :
    bin_WorkingLoop ("HSV - Working Loop"),
    bin_CvtColor ("HSV - CvtColor")
 {
    read_mv_setting (settings_file, "HUE_MIN", HMIN);
    read_mv_setting (settings_file, "HUE_MAX", HMAX);
    read_mv_setting (settings_file, "SAT_MIN", SMIN);
    read_mv_setting (settings_file, "SAT_MAX", SMAX);
    read_mv_setting (settings_file, "VAL_MIN", VMIN);
    read_mv_setting (settings_file, "VAL_MAX", VMAX);
    
    HMIN = (HMIN>=0) ? HMIN : HMIN+180; 
    HMAX = (HMAX<180) ? HMAX : HMAX-180; 
    
    HSVImg = mvCreateImage_Color (); // common size, 3 channel 
}

mvHSVFilter:: ~mvHSVFilter () {
    cvReleaseImage (&HSVImg);
}

inline
void mvHSVFilter:: setHSV (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax) {
    if (hmin != (int)UNCHANGED) HMIN = (hmin>=0) ? hmin : hmin+180; 
    if (hmax != (int)UNCHANGED) HMAX = (hmax<180) ? hmax : hmax-180; 
    if (smin != UNCHANGED) SMIN = smin;
    if (smax != UNCHANGED) SMAX = smax;
    if (vmin != UNCHANGED) VMIN = vmin;
    if (vmax != UNCHANGED) VMAX = vmax;
}

inline
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

      bin_CvtColor.start();
    cvCvtColor (img, HSVImg, CV_BGR2HSV); // convert to HSV 
      bin_CvtColor.stop();

    /* go through each pixel, set the result image's pixel to 0 or 255 based on whether the
     * origin img's HSV values are withing bounds
     */
    unsigned char *imgPtr, *resPtr;
    
      bin_WorkingLoop.start();
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
      bin_WorkingLoop.stop();
}

//####################################################################################
//####################################################################################
//####################################################################################

#define ABS(X) (((X)>=0) ? (X) : (-(X)))

mvAdaptiveFilter:: mvAdaptiveFilter (const char* settings_file) :
    bin_adaptive ("Adaptive Color Filter")
{
    for (unsigned i = 0; i < nBoxes; i++) {
        box_array[i].B = 0;
        box_array[i].G = 0;
        box_array[i].R = 0;
        box_array[i].count = 0;
        // possible location thingies?
    }

    src_HSV = mvCreateImage_Color();

    guess[0] = 110;
    guess[1] = 120;
    guess[2] = 100;
}

void mvAdaptiveFilter:: filter (const IplImage* src, IplImage* dst) {
    cvCvtColor (src, src_HSV, CV_BGR2HSV);

    bin_adaptive.start();
    unsigned next_empty_box = 0;

    unsigned char *srcPtr;  // pointer that will point to the beginning of each row     
    for (int r = 0; r < src_HSV->height; r++) { // for each row                        
        srcPtr = (unsigned char*) (src_HSV->imageData + r*src_HSV->widthStep);  // this points srcPtr to the beginning of the r'th row!
     
        for (int c = 0; c < src_HSV->width; c++) { // for each pixel
            int BB, GG, RR;
            BB = (int)(*srcPtr);
            GG = (int)(*(srcPtr+1));
            RR = (int)(*(srcPtr+2));

            int min_color_diff = 1E9;
            int min_color_diff_box = -1;
            int cur_color_diff;

            /// search for box of least color-space diff
            for (unsigned i = 0; i < next_empty_box; i++) {
                int E1 = (BB - box_array[i].B);
                int E2 = (GG - box_array[i].G);
                int E3 = (RR - box_array[i].R);

                cur_color_diff = 3*E1*E1 + E2*E2 + E3*E3;
                if (cur_color_diff < min_color_diff) {
                    min_color_diff = cur_color_diff;
                    min_color_diff_box = i;
                }
            }

            /// if that box is close enough, add pixel to its avg
            int m = min_color_diff_box;
            int E1 = (BB - box_array[m].B);
            int E2 = (GG - box_array[m].G);
            int E3 = (RR - box_array[m].R);
            if (m >= 0 && abs(E1)<15 && abs(E2)<50 && abs(E3)<50) {
                box_array[m].B = ((box_array[m].B)*box_array[m].count + BB) / (box_array[m].count+1);
                box_array[m].G = ((box_array[m].G)*box_array[m].count + GG) / (box_array[m].count+1);
                box_array[m].R = ((box_array[m].R)*box_array[m].count + RR) / (box_array[m].count+1);
                box_array[m].count++;
            }
            else if (next_empty_box < nBoxes) { /// else create a new box
/*
            DEBUG_PRINT ("Pixel %d (%d,%d,%d). Best Bin %d (%d,%d,%d). Diff %d,%d,%d\n", 
                r*src->width + c,
                BB,GG,RR, 
                min_color_diff_box, 
                box_array[min_color_diff_box].B, box_array[min_color_diff_box].G, box_array[min_color_diff_box].R,
                E1, E2, E3);
            cvWaitKey(600);
*/
                //printf ("pix: %d,%d,%d - box: %d,%d,%d\n", BB,GG,RR, box_array[m].B,box_array[m].G,box_array[m].R);
                box_array[next_empty_box].B = BB;
                box_array[next_empty_box].G = GG;
                box_array[next_empty_box].R = RR;
                box_array[next_empty_box].count = 1;
                next_empty_box++;
            }
            //else printf ("no more boxes wtf\n");

        srcPtr += 3;        // advance to the next pixel
        }
    }

    printf ("\nBox Count - %d\n", next_empty_box);
    for (unsigned i = 0; i < next_empty_box; i++) {
        printf ("  Box %d: (%d, %d, %d) - count %d\n", i+1, box_array[i].B, box_array[i].G, box_array[i].R, box_array[i].count);
    }
    cvWaitKey (1000);

    bin_adaptive.stop();
}

//####################################################################################
//####################################################################################
//####################################################################################

mvAdaptiveFilter2:: mvAdaptiveFilter2 (const char* settings_file) :
    bin_adaptive ("Adaptive Color Filter 2")
{
    //target.c1 = 85; target.c2 = 75; target.c3 = 100;
    target.c1 = 0; target.c2 = 90; target.c3 = 70;
    target.d1 = 10; target.d2 = 10; target.d3 = 10;
    target.count = 0;

    bg.c1 = 70; bg.c2 = 5; bg.c3 = 170;
    bg.d1 = 10; bg.d2 = 10; bg.d3 = 10;
    bg.count = 0;

    copy_box (&target, &next_target);
    copy_box (&bg, &next_bg);

    src_HSV = mvCreateImage_Color();
}

void mvAdaptiveFilter2:: filter (const IplImage* src, IplImage* dst) {
    assert (src != NULL);
    assert (dst != NULL);
    assert (src->nChannels == 3);
    assert (dst->nChannels == 1);

    cvCvtColor (src, src_HSV, CV_BGR2HSV);

    bin_adaptive.start();

    /// go thru each pixel. Allow those within 1d of target thru. Remove those within 1d of bg
    /// those with 2d of each are used to calculate the next frame's target and bg
    unsigned char *srcPtr, *dstPtr;
    for (int r = 0; r < src_HSV->height; r++) { // for each row                        
        srcPtr = (unsigned char*) (src_HSV->imageData + r*src_HSV->widthStep);
        dstPtr = (unsigned char*) (dst->imageData + r*dst->widthStep);
     
        for (int c = 0; c < src_HSV->width; c++) { // for each pixel
            int C1, C2, C3;
            C1 = (int)(*srcPtr);
            C2 = (int)(*(srcPtr+1));
            C3 = (int)(*(srcPtr+2));

            *dstPtr = 0;

            /// if within 1d of target
            if (in_box (&target, C1,C2,C3)) {
                accumulate_box (&next_target, C1,C2,C3);
                *dstPtr = 255;
            }
            else if (in_box(&bg, C1,C2,C3)) {
                accumulate_box (&next_bg, C1,C2,C3);
            }
            else if (in_2box (&target, C1,C2,C3)) {
                accumulate_box (&next_target, C1,C2,C3);
            }
            else if (in_2box (&bg, C1,C2,C3)) {
                accumulate_box (&next_bg, C1,C2,C3);
            }

            srcPtr += 3;
            dstPtr ++;
        }
    }

    DEBUG_PRINT ("Target Box: %d %d %d. count=%d\n", target.c1,target.c2,target.c3,target.count);   
    DEBUG_PRINT ("BG Box: %d %d %d. count=%d\n", bg.c1,bg.c2,bg.c3,bg.count);
    cvWaitKey(500);

    target.count = 0;
    bg.count = 0;
    copy_box (&next_target, &target);
    copy_box (&next_bg, &bg);

    bin_adaptive.stop();

}

