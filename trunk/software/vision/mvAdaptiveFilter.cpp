#include "mv.h"

#define FILTER_DEBUG
#ifdef FILTER_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

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
    target.c1 = 85; target.c2 = 75; target.c3 = 100;
    //target.c1 = 0; target.c2 = 90; target.c3 = 70;
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
    calc_limits (&target); 

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
            if (in_box (C1,C2,C3)) {
                accumulate_box (&next_target, C1,C2,C3);
                *dstPtr = 255;
            }
            else if (in_2box (C1,C2,C3)) {
                accumulate_box (&next_bg, C1,C2,C3);
            }

            srcPtr += 3;
            dstPtr ++;
        }
    }

    DEBUG_PRINT ("Target Box: %d %d %d. count=%d\n", target.c1,target.c2,target.c3,target.count);   
    DEBUG_PRINT ("BG Box: %d %d %d. count=%d\n", bg.c1,bg.c2,bg.c3,bg.count);
    //cvWaitKey(500);

    target.count = 0;
    bg.count = 0;
    copy_box (&next_target, &target);
    copy_box (&next_bg, &bg);

    bin_adaptive.stop();

}

//####################################################################################
//####################################################################################
//####################################################################################

mvAdaptiveFilter3:: mvAdaptiveFilter3 (const char* settings_file) :
    bin_adaptive ("Adaptive Color Filter 3")
{
    hue_target          = 0;
    sat_target          = 80;
    val_target          = 80;
    hue_target_delta    = 20;
    sat_target_delta    = 20;
    val_target_delta    = 30;

    src_HSV = mvCreateImage_Color();

    int size[] = {nbins1};
    float h_range[] = {0, 180};  float* ranges[] = {h_range};

    hist1 = cvCreateHist (
        1,              //dims
        size,           // num of bins per dims
        CV_HIST_ARRAY,  // dense matrix
        ranges,         // upper & lower bound for bins
        1               // uniform
    );
}

void mvAdaptiveFilter3:: filter (const IplImage* src, IplImage* dst) {
    assert (src != NULL);
    assert (dst != NULL);
    assert (src->nChannels == 3);
    assert (dst->nChannels == 1);

    bin_adaptive.start();

    hue_img = dst;

    /** extract hue plane. Mark all invalid pixels as Hue=255, which is outside histogram range */
    // convert to HSV       
    cvCvtColor (src, src_HSV, CV_BGR2HSV); // convert to Hue,Saturation,Value 
    
    int sat_max = sat_target + sat_target_delta;
    int sat_min = sat_target - sat_target_delta;
    int val_max = val_target + val_target_delta;
    int val_min = val_target - val_target_delta;

    unsigned char *imgPtr, *huePtr;
    for (int i = 0; i < src_HSV->height; i++) {
        imgPtr = (unsigned char*) src_HSV->imageData + i*src_HSV->widthStep;
        huePtr = (unsigned char*) hue_img->imageData + i*hue_img->widthStep;
        
        for (int j = 0; j < src_HSV->width; j++) {
            if ((*(imgPtr+1) >= sat_min) && (*(imgPtr+1) <= sat_max) &&
                (*(imgPtr+2) >= val_min) && (*(imgPtr+2) <= val_max)) 
                *huePtr = *imgPtr;
            else
                *huePtr = 255;

            huePtr++; 
            imgPtr+=3;
        }
    }

    /** genereate hue histogram */ 
    IplImage* planes[] = {hue_img};
    cvCalcHist (
        planes, hist1,
        0,      // accumulate
        NULL    // possible boolean mask image
    ); 

    cvNormalizeHist (hist1, 10000); // normalize so total count is 100
    for (int i = 0; i < nbins1; i++) {
        unsigned val = cvQueryHistValue_1D (hist1, i);
        DEBUG_PRINT ("Bin %2.1d (%3.1d-%3.1d): %d\n", i, 180/nbins1*i, 180/nbins1*(i+1)-1, val);    
    }

    /** find nearby local max, exit if fail to find */
    int srch_min = hue_target - hue_target_delta;
    int srch_max = hue_target + hue_target_delta;
    if (srch_min < 0) srch_min += 180;
    if (srch_max > 179) srch_max -= 180;
    // the min and max bins to search
    int bin_min_i = srch_min*nbins1 / 180;
    int bin_max_i = srch_max*nbins1 / 180; 
    
    // now find the local maximum within bin_min_i and bin_max_i
    unsigned local_max_bin_index = 0;
    unsigned local_max_bin_value = 0;
    unsigned temp;
    for (int i = bin_min_i; ; i++) {
        if (i >= nbins1) i = 0;     // this line allows looping from bin16 to bin2, for example
            temp = cvQueryHistValue_1D (hist1, i);
        if (temp > local_max_bin_value) {
            local_max_bin_value = temp;
            local_max_bin_index = i;
        }
        
        if (i == bin_max_i) break; // loop from bin_min_i to bin_max_i inclusive
    }

    /** mark bins neighbouring the max until integral of marked bins >> those of nearby unmarked bins */
    // bin_min_i and bin_max_i now refer to the range of "marked" bins
    // a marked bin means the hue values inside is part of the target
    bin_min_i = bin_max_i = local_max_bin_index;
    unsigned marked = 1;
    bool success = false;
    for (; marked <= nbins1/6-1; marked++) { // nbins1 / 6  =  30 / (180/nbins1)
        unsigned integral_marked = 0;
        unsigned integral_unmarked = 0;
printf ("hi1\n");
        // calculate integral of marked bins 
        for (int i = bin_min_i; ; i++) {
            if (i >= nbins1) i = 0;
            integral_marked += cvQueryHistValue_1D (hist1, i);
             
            if (i == bin_max_i) break; // loop from bin_min_i to bin_max_i inclusive
        }
        integral_marked /= marked; // average integral of a marked bin.
printf ("hi2\n");
        // caculate integral of neighbouring unmakred bins (those +- 1 from marked bins)
        int temp;
        unsigned min_1, max_1;
        temp = bin_min_i-1;
        if (temp < 0) temp += nbins1;
        min_1 = cvQueryHistValue_1D (hist1, temp);  integral_unmarked += min_1;
        temp = bin_max_i+1;
        if (temp >= nbins1) temp -= nbins1;
        max_1 = cvQueryHistValue_1D (hist1, temp);  integral_unmarked += max_1;
         
        integral_unmarked /= 2;
printf ("hi3\n");
        // if integral of marked bins >> integral of unmarked
        if ((integral_marked > integral_unmarked*5) && (integral_marked > integral_unmarked + 10)) {
            success = true;
            break;
        }
printf ("hi4\n");
        // else mark one more bin
        if (min_1 > max_1)
            bin_min_i = (bin_min_i-1 >= 0) ? (bin_min_i-1) : 179;
        else
            bin_max_i = (bin_max_i+1 < 180) ? (bin_max_i+1) : 0;
    }

    DEBUG_PRINT ("Bin Range: %d-%d\n", bin_min_i, bin_max_i);
    /** if successful, generate dst image */
    unsigned hue_min;
    unsigned hue_max;
    if (success) {// successful
        hue_min = bin_min_i * 180 / nbins1;
        hue_max = (bin_max_i+1) * 180 / nbins1;
    
        for (int i = 0; i < hue_img->height; i++) {
            huePtr = (unsigned char*) hue_img->imageData + i*hue_img->widthStep;
            for (int j = 0; j < hue_img->width; j++) {
                if ((*huePtr != 255) && hue_in_range (*huePtr, hue_min, hue_max)) {
                    *huePtr = 255;
                }
                else {                    
                    *huePtr = 0;
                }
                huePtr++; 
            }
        }
    }
    else {
        return;
    }

    DEBUG_PRINT ("Hue Range: %d-%d\n", hue_min, hue_max);
    bin_adaptive.stop();
}