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

mvAdaptiveFilter::mvAdaptiveFilter (const char* settings_file) :
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
    hue_img = mvCreateImageHeader();
    sat_img = mvCreateImageHeader();

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

mvAdaptiveFilter::~mvAdaptiveFilter () {
    delete win;
    cvReleaseHist (&hist);
    mvReleaseScratchImage_Color();
    cvReleaseImageHeader(&hue_img);
    cvReleaseImageHeader(&sat_img);
    if (DISPLAY_HIST)
        cvReleaseImage(&hist_img);
}

void mvAdaptiveFilter:: setQuad (Quad &Q, int h0, int s0, int h1, int s1) {
    Q.h0 = h0; Q.s0 = s0;
    Q.h1 = h1; Q.s1 = s1;
}

int mvAdaptiveFilter::getQuadValue (Quad Q) {
    if (Q.s0 == -1)
        return -1;

    int count = 0;
    int num_bins = 0;
    for (int h = Q.h0; ; h++) {
        if (h >= nbins_hue) h = 0;  // allows looping of hue from bin16 to bin2, ect
        for (int s = Q.s0; s <= Q.s1; s++) {
            count += cvQueryHistValue_2D (hist, h, s);
            num_bins++;   
        }
        if(h == Q.h1) break;
    }
    return count/num_bins;
}

void mvAdaptiveFilter::filter (IplImage* src, IplImage* dst) {
    assert (src != NULL);
    assert (dst != NULL);
    assert (src->nChannels == 3);
    assert (dst->nChannels == 1);
    assert (src->width == src_HSV->width);
    assert (src->height == src_HSV->height);

    /// convert imageto HSV  
    bin_CvtColor.start();
    cvCvtColor (src, src_HSV, CV_BGR2HSV); // convert to Hue,Saturation,Value 
    bin_CvtColor.stop();

    /// Mark all invalid (Value not within limits) pixels as Hue=255 and Sat = 0 which is outside histogram range
    bin_adaptive.start();
    unsigned char *imgPtr;
    for (int i = 0; i < src_HSV->height; i++) {
        imgPtr = (unsigned char*) src_HSV->imageData + i*src_HSV->widthStep;
        for (int j = 0; j < src_HSV->width; j++) {
            if ((*(imgPtr+2) < val_min) || (*(imgPtr+2) > val_max)) {
                *imgPtr = 255;
                *(imgPtr+1) = 0;
            }
            imgPtr+=3;
        }
    }

    /// in place split of the image
    mvSplitImage (src_HSV, &hue_img, &sat_img);

    /// genereate the hue-sat histogram and normalize it 
    IplImage* planes[] = {hue_img, sat_img};
    cvCalcHist (
        planes, hist,
        0,      // accumulate
        NULL    // possible boolean mask image
    ); 

    cvNormalizeHist (hist, HISTOGRAM_NORM_FACTOR);
    
    #ifdef FILTER_DEBUG
        print_histogram();
        show_histogram();
    #endif

    DEBUG_PRINT ("\nBeginning Histogram Filter Algorithm\n");

    /// Attempt to find a local maximum within the user-defined hue-sat bounds
    int bin_min_index_hue = hue_min*nbins_hue / (hue_range_max - hue_range_min);
    int bin_max_index_hue = hue_max*nbins_hue / (hue_range_max - hue_range_min) - 1;
    int bin_min_index_sat = sat_min*nbins_sat / (sat_range_max - sat_range_min);
    int bin_max_index_sat = sat_max*nbins_sat / (sat_range_max - sat_range_min) - 1;
    DEBUG_PRINT ("Bins Searched: Hue (%d-%d)  Sat (%d-%d)\n", bin_min_index_hue, bin_max_index_hue, bin_min_index_sat, bin_max_index_sat);
    
    unsigned local_max_bin_index[2] = {0,0};
    unsigned local_max_bin_value = 0;
    for (int h = bin_min_index_hue; ; h++) {
        if (h >= nbins_hue) h = 0;  // allows looping of hue from bin16 to bin2, ect

        for (int s = bin_min_index_sat; s <= bin_max_index_sat; s++) {
            unsigned bin_value = cvQueryHistValue_2D (hist, h, s);
            if (bin_value > local_max_bin_value) {
                local_max_bin_value = bin_value;
                local_max_bin_index[0] = h;
                local_max_bin_index[1] = s;
            }
        }

        if(h == bin_max_index_hue) break;
    }

    if (local_max_bin_value == 0) { // no non-zero bins in range
        printf ("All bins in search range are zero!\n");
        bin_adaptive.stop();
        return;
    }

    DEBUG_PRINT ("Local Max at bin (%d, %d)\n", local_max_bin_index[0], local_max_bin_index[1]);

    /// now define rect, which is the rectangle in BinIndex space representing the accepted H-S window
    Quad rect;
    setQuad (rect, local_max_bin_index[0], local_max_bin_index[1], local_max_bin_index[0], local_max_bin_index[1]);
    Quad sides[NUM_SIDES_RECTANGLE];

    DEBUG_PRINT ("Accumulating Bins\n");
    bool successful = false;

    for(int i = 0; i < 4; i++){
        DEBUG_PRINT ("  Iteration %d\n", i);
        
        /// Obtain the BinIndex space representation of the 4 sides of the rect
        getRectangleNeighbours (rect, sides);
        int rect_val = getQuadValue(rect);

        /// find the avg value of the squares on the sides.
        int side_val[NUM_SIDES_RECTANGLE];
        int side_val_avg = 0;
        int num_valid_sides = 0;
        for (int j = 0; j < NUM_SIDES_RECTANGLE; j++) {
            side_val[j] = getQuadValue(sides[j]);
            if (side_val[j] >= 0) {
                num_valid_sides++;
                side_val_avg += side_val[j];
            }
        }

        side_val_avg /= num_valid_sides;
   
        DEBUG_PRINT ("    rect: (%d,%d,%d,%d) - %d\n", rect.h0, rect.s0, rect.h1, rect.s1, rect_val);
        for (int j = 0; j < NUM_SIDES_RECTANGLE; j++){
            DEBUG_PRINT("    side: (%d,%d,%d,%d) - %d\n", sides[j].h0, sides[j].s0, sides[j].h1, sides[j].s1, side_val[j]);
        }        
        DEBUG_PRINT ("    side_avg: %d\n", side_val_avg);

        /// If the avg value of the sides >> value inside the rect, thenthe algorithm is done
        if (rect_val > 8*side_val_avg) {
            DEBUG_PRINT ("  Break on iteration %d\n", i);
            successful = true;
            break;
        }

        /// Otherwise add the best side to the rectangle
        int best_side = -1;
        int best_side_val = -1;
        for (int j = 0; j < NUM_SIDES_RECTANGLE; j++) {
            if (side_val[j] >= 0 && side_val[j] > best_side_val) {
                best_side = j;
                best_side_val = side_val[j];
            }
        }
        if (best_side == 0 || best_side == 3) {
            rect.h0 = sides[best_side].h0; 
            rect.s0 = sides[best_side].s0;
        }
        else {
            rect.h1 = sides[best_side].h1;
            rect.s1 = sides[best_side].s1;
        }
    }

    /// Calculate the H-S space coordinates of the allowed window using the BinIndex values
    unsigned hue_min_hist = (unsigned)rect.h0 * hue_range_max / nbins_hue;
    unsigned hue_max_hist = (unsigned)(rect.h1+1) * hue_range_max / nbins_hue;
    unsigned sat_min_hist = (unsigned)rect.s0 * sat_range_max / nbins_sat;
    unsigned sat_max_hist = (unsigned)(rect.s1+1) * sat_range_max / nbins_sat;
    DEBUG_PRINT ("Hue Range: %d-%d\n", hue_min_hist, hue_max_hist);
    DEBUG_PRINT ("Sat Range: %d-%d\n", sat_min_hist, sat_max_hist);

    /// if successful, generate dst image
    unsigned count = 0;
    if (successful) {
        unsigned char *dstPtr, *huePtr, *satPtr;
        for (int i = 0; i < dst->height; i++) {
            dstPtr = (unsigned char*) dst->imageData + i*dst->widthStep;
            huePtr = (unsigned char*) hue_img->imageData + i*hue_img->widthStep;
            satPtr = (unsigned char*) sat_img->imageData + i*sat_img->widthStep;

            for (int j = 0; j < dst->width; j++) {
                if ((*huePtr != 255) && hue_in_range (*huePtr, hue_min_hist, hue_max_hist) && 
                    (*satPtr >= sat_min_hist && *satPtr <= sat_max_hist))
                {
                    *dstPtr = 255;
                    count++;
                }
                else {                    
                    *dstPtr = 0;
                }
                
                dstPtr++; 
                huePtr++;
                satPtr++;
            }
        }
    }
    else {
        cvZero (dst);
    }

    DEBUG_PRINT ("Allowed %d pixels\n", count);
    bin_adaptive.stop();
}

void mvAdaptiveFilter::print_histogram () {
    int hist_height = hist->mat.dim[0].size;
    int hist_width = hist->mat.dim[1].size;
    printf ("\nprint_histogram():\n");

    for (int i = 0; i < hist_height; i++) {
        for (int j = 0; j < hist_width; j++) {
            int binval = cvQueryHistValue_2D(hist, i,j);
            unsigned hue_min = (unsigned)i * hue_range_max / nbins_hue;
            unsigned hue_max = (unsigned)(i+1) * hue_range_max / nbins_hue;
            unsigned sat_min = (unsigned)j * sat_range_max / nbins_sat;
            unsigned sat_max = (unsigned)(j+1) * sat_range_max / nbins_sat;
        
            if (binval > 0)
                printf ("Bin(%2d,%2d) HS(%d-%d,%d-%d) count=%d\n",i,j, hue_min,hue_max,sat_min,sat_max, binval);
        }
    }
}

void mvAdaptiveFilter::show_histogram () {
    cvZero (hist_img);
    
    float max;
    cvGetMinMaxHistValue (hist, 0, &max, 0, 0);

    int hist_height = hist->mat.dim[0].size;
    int hist_width = hist->mat.dim[1].size;

    for (int i = 0; i < hist_height; i++) {
        for (int j = 0; j < hist_width; j++) {
            float binval = cvQueryHistValue_2D(hist, i,j);
            int intensity = cvRound (sqrt(binval/max) * 255);

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

void mvAdaptiveFilter::getRectangleNeighbours(Quad rect, Quad sides[]){
    int h0 = rect.h0;
    int s0 = rect.s0;
    int h1 = rect.h1;
    int s1 = rect.s1;

    setQuad(sides[0], (h0>0) ? h0-1: nbins_hue-1, s0, (h0>0) ? h0-1: nbins_hue-1, s1);
    setQuad(sides[1], h0, (s1<nbins_sat-1) ? s1+1 : -1, h1, (s1<nbins_sat-1) ? s1+1: -1);
    setQuad(sides[2], (h1<nbins_hue-1) ? h1+1: 0, s0, (h1<nbins_hue-1) ? h1+1: 0, s1);
    setQuad(sides[3], h0, (s0>0)? s0-1: -1, h1, (s0>0)? s0-1: -1);
}

//####################################################################################
//####################################################################################
//####################################################################################

mvMeanShift::mvMeanShift (const char* settings_file) : 
    bin_Resize ("mvMeanShift - Resize"),
    bin_MeanShift ("mvMeanShift - MeanShift"),
    bin_Filter ("mvMeanShift - Filter")
{
    assert (KERNEL_SIZE % 2 == 1);
    
    // read constants from file
    read_mv_setting (settings_file, "HUE_DIST", H_DIST);
    read_mv_setting (settings_file, "SAT_DIST", S_DIST);
    read_mv_setting (settings_file, "VAL_DIST", V_DIST);

    int width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);

    // create Hue_Box
    for (int i = 0; i < NUM_BOXES; i++) {
        hue_box[i] = new Hue_Box(settings_file, i+1);
    }

    // create downsampled scratch images. The 1 channel image shares data with the 3 channel
    ds_scratch_3 = cvCreateImage(cvSize(width/DS_FACTOR, height/DS_FACTOR), IPL_DEPTH_8U, 3);
    ds_scratch = cvCreateImageHeader(cvSize(width/DS_FACTOR, height/DS_FACTOR), IPL_DEPTH_8U, 1);
    ds_scratch->imageData = ds_scratch_3->imageData;

    // generate kernel point array
    int kernel_area = KERNEL_SIZE * KERNEL_SIZE;
    int kernel_rad = (KERNEL_SIZE - 1)/2;
    kernel_point_array = new int[kernel_area];
    
    unsigned array_index = 0;
    for (int j = -kernel_rad; j <= kernel_rad; j++)
        for (int i = -kernel_rad; i <= kernel_rad; i++)
            kernel_point_array[array_index++] = i*ds_scratch->widthStep + j;
}

mvMeanShift::~mvMeanShift () {
    delete[] kernel_point_array;

    for (int i = 0; i < NUM_BOXES; i++)
        delete hue_box[i];

    cvReleaseImage (&ds_scratch_3);
    cvReleaseImageHeader (&ds_scratch);
}

void mvMeanShift::mean_shift(IplImage* src, IplImage* dst) {
      downsample_from (src);
      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

      mvMeanShift_internal(src);

      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_HSV2BGR);
      upsample_to_3 (dst);
}

void mvMeanShift::filter(IplImage* src, IplImage* dst) {
      downsample_from (src);
      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

      colorFilter_internal();

      upsample_to (dst);
}

void mvMeanShift::combined_filter(IplImage* src, IplImage* dst) {
      downsample_from (src);
      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

      mvMeanShift_internal(src);
      colorFilter_internal();

      upsample_to (dst);
}

void mvMeanShift::mvMeanShift_internal(IplImage* src_scratch) {
// note this will treat the image as if it was in HSV format
// src_scratch is the src image passed in by the user, which will now be used as a scratch
    bin_MeanShift.start();

    int kernel_area = KERNEL_SIZE * KERNEL_SIZE;
    int kernel_rad = (KERNEL_SIZE - 1) / 2;

    cvZero (src_scratch);

    // we will be filtering ds_scratch_3 to src_scratch, then copying the data back to ds_scratch_3
    unsigned char* imgPtr, *resPtr;
    for (int r = kernel_rad; r < ds_scratch_3->height-kernel_rad; r++) {                         
        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*ds_scratch_3->widthStep + 3*kernel_rad); // imgPtr = first pixel of rth's row
        resPtr = (unsigned char*) (src_scratch->imageData + r*src_scratch->widthStep + 3*kernel_rad);
        
        for (int c = kernel_rad; c < ds_scratch_3->width-kernel_rad; c++) {
            unsigned char H = *imgPtr;
            unsigned char S = *(imgPtr+1);
            unsigned char V = *(imgPtr+2);
            
            // check if the src pixel meats S,V min reqs
            if (S >= S_MIN && V >= V_MIN) {
                unsigned char* tempPtr;
                unsigned H2 = H, S2 = S, V2 = V;
                unsigned char good_pixels = 1, total_pixels = 1;

                // go thru each pixel in the kernel
                for (int i = 0; i < kernel_area; i++) {
                    tempPtr = imgPtr + 3*kernel_point_array[i];

                    if (abs(S-tempPtr[1]) <= S_DIST && 
                        abs(V-tempPtr[2]) <= V_DIST &&
                        std::min(abs(H-tempPtr[0]),180-abs(H-tempPtr[0])) <= H_DIST
                        ) 
                    {
                        // Circular red case is hard, let's just use 0 if we're looking for red and see > 90,
                        // and use 179 if we're looking for red >= 0
                        // RZ: I'm still confused
                        if (H <= H_DIST && abs(H-tempPtr[0]) > 90) {
                            H2 += 0;
                        } else if (H >= 180 - H_DIST && abs(H-tempPtr[0]) > 90) {
                            H2 += 179;
                        } else {
                            H2 += (unsigned)tempPtr[0];
                        }
                        S2 += (unsigned)tempPtr[1];
                        V2 += (unsigned)tempPtr[2];
                        good_pixels++;
                    }

                    total_pixels++;
                }

                if (GOOD_PIXELS_FACTOR*good_pixels >= total_pixels) {
                    resPtr[0] = (unsigned char)(H2 / good_pixels);
                    resPtr[1] = (unsigned char)(S2 / good_pixels);
                    resPtr[2] = (unsigned char)(V2 / good_pixels);
                }
            }
            
            imgPtr += 3;
            resPtr += 3;
        }
    }

    // copy src_scratch's useful data to ds_scratch_3
    for (int r = 0; r < ds_scratch_3->height; r++) {            
        imgPtr = (unsigned char*) (src_scratch->imageData + r*src_scratch->widthStep);             
        resPtr = (unsigned char*) (ds_scratch_3->imageData + r*ds_scratch_3->widthStep);
        memcpy (resPtr, imgPtr, ds_scratch_3->widthStep);
        /*   
        for (int c = 0; c < ds_scratch_3->width*3; c++) {
            *resPtr = *imgPtr;
            imgPtr++;
            resPtr++;
        }*/
    }

#ifdef M_DEBUG
    cvNamedWindow("mvMeanShift debug", CV_WINDOW_AUTOSIZE);
    cvShowImage("mvMeanShift debug", ds_scratch_3);
#endif

    bin_MeanShift.stop();
}

void mvMeanShift::colorFilter_internal() {
// this function goes over the Hue_Box array and if a pixel falls inside box X, it marks that pixel
// with value X
    unsigned char *imgPtr, *resPtr;
    for (int r = 0; r < ds_scratch->height; r++) {                        
        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*ds_scratch_3->widthStep);
        resPtr = (unsigned char*) (ds_scratch->imageData + r*ds_scratch->widthStep); 
   
        for (int c = 0; c < ds_scratch->width; c++) {
            *resPtr = 0;

            for (int i = 0; i < NUM_BOXES; i++) {
                if (!(hue_box[i])->is_enabled())
                    continue;

                if (imgPtr[1] != 0 && (hue_box[i])->check_hue(imgPtr[0], imgPtr[1], imgPtr[2])) {
                    *resPtr = (unsigned char) hue_box[i]->BOX_COLOR;
                }
            }

            imgPtr += 3;
            resPtr++;
        }
    }
}

//####################################################################################
//####################################################################################
//####################################################################################

struct mvTarget {
    unsigned char h, s, v;
};

char mDistance(int a, int b, int c, int x, int y, int z){
    return((4*std::min(abs(x-a),180-abs(x-a)) + abs(y-b) + abs(z-c))/5);
}

void ManhattanDistanceFilter(IplImage* src, IplImage* dst){
    mvTarget targets[] = {{50,130,60},{100,80,40}};

    unsigned char minDist, tempDist;
    unsigned char* imgPtr, *resPtr;

    IplImage * HSVImg = mvGetScratchImage_Color();
    cvCvtColor (src, HSVImg, CV_BGR2HSV);


    for (int r = 0; r < HSVImg->height; r++) {                         
        imgPtr = (unsigned char*) (HSVImg->imageData + r*HSVImg->widthStep); // imgPtr = first pixel of rth's row
        resPtr = (unsigned char*) (dst->imageData + r*dst->widthStep);
        
        for (int c = 0; c < dst->width; c++) {
            minDist = 255;
            for(int i =0; i<2; i++){
                tempDist = mDistance(*imgPtr, *(imgPtr+1), *(imgPtr+2), targets[i].h, targets[i].s, targets[i].v);
                if(tempDist < minDist) minDist = tempDist;
            }
            *resPtr = minDist;
            imgPtr+=3; resPtr++;
        }
    }

    mvReleaseScratchImage_Color();
}

