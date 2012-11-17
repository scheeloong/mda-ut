#include "mv.h"
#include <math.h>

#define FILTER_DEBUG
#ifdef FILTER_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

#define DISPLAY_HIST 1
#define HIST_PIXEL_SCALE 10

//####################################################################################
//####################################################################################
//####################################################################################

mvAdaptiveFilter3:: mvAdaptiveFilter3 (const char* settings_file) :
    bin_adaptive ("Adaptive3 - Logic"),
    bin_CvtColor ("Adaptive3 - CvtColor")
{
    read_mv_setting (settings_file, "HUE_MIN", hue_min);
    read_mv_setting (settings_file, "HUE_MAX", hue_max);
    read_mv_setting (settings_file, "SAT_MIN", sat_min);
    read_mv_setting (settings_file, "SAT_MAX", sat_max);
    read_mv_setting (settings_file, "VAL_MIN", val_min);
    read_mv_setting (settings_file, "VAL_MAX", val_max);

    src_HSV = mvGetScratchImage_Color();
    hue_img = mvCreateImage();
    sat_img = mvCreateImage();

    if (DISPLAY_HIST) {
        hist_img = cvCreateImage(
            cvSize(nbins_hue*HIST_PIXEL_SCALE, nbins_sat*HIST_PIXEL_SCALE),
            IPL_DEPTH_8U,
            1
        );
    }
    else
        hist_img = NULL;

    win = new mvWindow ("Adaptive Filter 3");

    int size[] = {nbins_hue, nbins_sat};
    float hue_range[] = {hue_range_min, hue_range_max};
    float sat_range[] = {sat_range_min, sat_range_max};
    float* ranges[] = {hue_range, sat_range};

    hist = cvCreateHist (
        2,              //dims
        size,           // num of bins per dims
        CV_HIST_ARRAY,  // dense matrix
        ranges,         // upper & lower bound for bins
        1               // uniform
    );
}

mvAdaptiveFilter3:: ~mvAdaptiveFilter3 () {
    delete win;
    cvReleaseHist (&hist);
    mvReleaseScratchImage_Color();
    cvReleaseImage(&hue_img);
    cvReleaseImage(&sat_img);
    cvReleaseImage(&hist_img);
}

void mvAdaptiveFilter3:: filter (const IplImage* src, IplImage* dst) {
    assert (src != NULL);
    assert (dst != NULL);
    assert (src->nChannels == 3);
    assert (dst->nChannels == 1);

    bin_CvtColor.start();

    /** extract hue plane. Mark all invalid pixels as Hue=255, which is outside histogram range */
    // convert to HSV       
    cvCvtColor (src, src_HSV, CV_BGR2HSV); // convert to Hue,Saturation,Value 

    bin_CvtColor.stop();
    bin_adaptive.start();

    unsigned char *imgPtr, *huePtr, *satPtr;
    for (int i = 0; i < src_HSV->height; i++) {
        imgPtr = (unsigned char*) src_HSV->imageData + i*src_HSV->widthStep;
        huePtr = (unsigned char*) hue_img->imageData + i*hue_img->widthStep;
        satPtr = (unsigned char*) sat_img->imageData + i*sat_img->widthStep;
        
        for (int j = 0; j < src_HSV->width; j++) {
            if ((*(imgPtr+2) >= val_min) && (*(imgPtr+2) <= val_max)) {
                *huePtr = *imgPtr;
                *satPtr = *(imgPtr+1);
            }
            else {
                *huePtr = 255;
                *satPtr = 1;
            }

            huePtr++;
            satPtr++; 
            imgPtr+=3;
        }
    }

    /** genereate hue-sat histogram */ 
    IplImage* planes[] = {hue_img, sat_img};
    cvCalcHist (
        planes, hist,
        0,      // accumulate
        NULL    // possible boolean mask image
    ); 

    cvNormalizeHist (hist, HISTOGRAM_NORM_FACTOR);
    
    #ifdef FILTER_DEBUG
        //show_histogram();
    #endif

    /// Attempt to find a local maximum within the user-defined hue-sat bounds
    int bin_min_index_hue = hue_min*nbins_hue / (hue_range_max - hue_range_min);
    int bin_max_index_hue = hue_max*nbins_hue / (hue_range_max - hue_range_min) - 1;
    int bin_min_index_sat = sat_min*nbins_sat / (sat_range_max - sat_range_min);
    int bin_max_index_sat = sat_max*nbins_sat / (sat_range_max - sat_range_min) - 1;

    DEBUG_PRINT ("Bins Searched: Hue (%d-%d)  Sat (%d-%d)\n", bin_min_index_hue, bin_max_index_hue, bin_min_index_sat, bin_max_index_sat);
    
    // now find the local maximum within bin_min_i and bin_max_i
    unsigned local_max_bin_index[2] = {0,0};
    unsigned local_max_bin_value = 0;
    for (int h = bin_min_index_hue; ; h++) {
        if (h >= nbins_hue) h = 0;  // allows looping of hue from bin16 to bin2, ect

        for (int s = bin_min_index_sat; ; s++) {
            if (s >= nbins_sat) s = 0;

            unsigned bin_value = cvQueryHistValue_2D (hist, h, s);
            if (bin_value > local_max_bin_value) {
                local_max_bin_value = bin_value;
                local_max_bin_index[0] = h;
                local_max_bin_index[1] = s;
            }

            if(s == bin_max_index_sat) break;
        }

        if(h == bin_max_index_hue) break;
    }

    if (local_max_bin_value == 0) { // no non-zero bins in range
        printf ("All bins in search range are zero!\n");
        bin_adaptive.stop();
        return;
    }

    DEBUG_PRINT ("Local Max at bin (%d, %d)\n", local_max_bin_index[0], local_max_bin_index[1]);
/*
    /// mark bins neighbouring the max until integral of marked bins >> those of nearby unmarked bins 
    // bin_min_i and bin_max_i now refer to the range of "marked" bins
    // a marked bin means the hue values inside is part of the target
    bin_min_i = bin_max_i = local_max_bin_index;
    unsigned marked = 1;
    bool success = false;
    for (; marked <= MAX_BINS_MARKED_1-1; marked++) {
        unsigned integral_marked = 0;
        unsigned integral_unmarked = 0;

        // calculate integral of marked bins 
        for (int i = bin_min_i; ; i++) {
            if (i >= nbins1) i = 0;
            integral_marked += cvQueryHistValue_1D (hist1, i);
             
            if (i == bin_max_i) break; // loop from bin_min_i to bin_max_i inclusive
        }
        integral_marked /= marked; // average integral of a marked bin.

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

        // if integral of marked bins >> integral of unmarked
        if ((integral_marked > integral_unmarked*5) && (integral_marked > integral_unmarked + HISTOGRAM_NORM_FACTOR/20)) {
            success = true;
            break;
        }

        // else mark one more bin
        if (min_1 > max_1)
            bin_min_i = (bin_min_i-1 >= 0) ? (bin_min_i-1) : 179;
        else
            bin_max_i = (bin_max_i+1 < 180) ? (bin_max_i+1) : 0;
    }

    if (success)
        DEBUG_PRINT ("Bins Succesfully Marked: %d-%d\n", bin_min_i, bin_max_i);
    else
        DEBUG_PRINT ("Unable to meet bin thresholds. Bins not marked\n");
    
    /// if successful, generate dst image
    unsigned hue_min_hist;
    unsigned hue_max_hist;
    if (success) {// successful
        hue_min_hist = bin_min_i * 179 / nbins1;
        hue_max_hist = (bin_max_i+1) * 179 / nbins1;
    
        for (int i = 0; i < hue_img->height; i++) {
            huePtr = (unsigned char*) hue_img->imageData + i*hue_img->widthStep;
            for (int j = 0; j < hue_img->width; j++) {
                if ((*huePtr != 255) && hue_in_range (*huePtr, hue_min_hist, hue_max_hist)) {
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
        cvZero (dst);
        bin_adaptive.stop();
        return;
    }

    DEBUG_PRINT ("Hue Range: %d-%d\n", hue_min_hist, hue_max_hist);
    */
    bin_adaptive.stop();
}

void mvAdaptiveFilter3:: show_histogram () {
    cvZero (hist_img);
    
    float max, binval;
    int intensity;
    cvGetMinMaxHistValue (hist, 0, &max, 0, 0);

    int hist_height = hist->mat.dim[0].size;
    int hist_width = hist->mat.dim[1].size;

    for (int i = 0; i < hist_height; i++) {
        for (int j = 0; j < hist_width; j++) {
            binval = cvQueryHistValue_2D(hist, i,j);
            intensity = cvRound (sqrt(binval/max) * 255);

            cvRectangle (
                hist_img,
                cvPoint (j*HIST_PIXEL_SCALE, i*HIST_PIXEL_SCALE), // x coord first
                cvPoint ((j+1)*HIST_PIXEL_SCALE, (i+1)*HIST_PIXEL_SCALE),
                CV_RGB(intensity,intensity,intensity),
                CV_FILLED
            );
        }
    }

    win->showImage(hist_img);
}