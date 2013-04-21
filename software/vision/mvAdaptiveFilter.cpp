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
    ds_scratch = cvCreateImage(cvSize(width/DS_FACTOR, height/DS_FACTOR), IPL_DEPTH_8U, 1);
    //ds_scratch = cvCreateImageHeader(cvSize(width/DS_FACTOR, height/DS_FACTOR), IPL_DEPTH_8U, 1);
    //ds_scratch->imageData = ds_scratch_3->imageData;

    // generate kernel point array
    KERNEL_AREA = KERNEL_SIZE * KERNEL_SIZE;
    KERNEL_RAD = (KERNEL_SIZE - 1)/2;

#ifdef KERNEL_SHAPE_RECT
    kernel_point_array = new int[KERNEL_AREA];
    
    unsigned array_index = 0;
    for (int j = -KERNEL_RAD; j <= KERNEL_RAD; j++)
        for (int i = -KERNEL_RAD; i <= KERNEL_RAD; i++)
            kernel_point_array[array_index++] = i*ds_scratch->widthStep + j;       
#else    
    int* temp_array = new int[KERNEL_AREA];
    unsigned array_index = 0;
    unsigned valid_count = 0;
    int R2 = KERNEL_RAD*KERNEL_RAD;//KERNEL_SIZE*KERNEL_SIZE/4;

    for (int j = -KERNEL_RAD; j <= KERNEL_RAD; j++) {
        for (int i = -KERNEL_RAD; i <= KERNEL_RAD; i++) {
            if (i*i + j*j <= R2) { // check if i,j are in the ellipse
                temp_array[array_index++] = i*ds_scratch->widthStep + j;
                valid_count++;   
            }
            else {
                temp_array[array_index++] = MV_UNDEFINED_VALUE;
            }                
        }
    }

    /// copy the valid points to kernel_point_array
    kernel_point_array = new int[valid_count];
    array_index = 0;
    for (int i = 0; i < KERNEL_AREA; i++) {
        if (temp_array[i] != MV_UNDEFINED_VALUE) {
            kernel_point_array[array_index++] = temp_array[i];
        }
    }
    delete[] temp_array;
    KERNEL_AREA = valid_count;
#endif
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

      meanshift_internal(src);

      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_HSV2BGR);
      upsample_to_3 (dst);
}

void mvMeanShift::filter(IplImage* src, IplImage* dst) {
      downsample_from (src);
      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

      colorFilter_internal_adaptive_hue();

      upsample_to (dst);
}

void mvMeanShift::combined_filter(IplImage* src, IplImage* dst) {
      downsample_from (src);
      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

      meanshift_internal(src);
      colorFilter_internal();

      upsample_to (dst);
}
 
bool mvMeanShift::add_pixel_if_within_range (unsigned char* pixel_to_add, unsigned char* ref_pixel,
                                unsigned &h_sum, unsigned &s_sum, unsigned &v_sum,
                                unsigned &num_pixels)
{
    int H = pixel_to_add[0];
    int S = pixel_to_add[1];
    int V = pixel_to_add[2];
    int Href = ref_pixel[0];
    int Sref = ref_pixel[1];
    int Vref = ref_pixel[2];

    if (abs(S-Sref) <= S_DIST && abs(V-Vref) <= V_DIST &&
        std::min(abs(H-Href),180-abs(H-Href)) <= H_DIST)
    {
        // Circular red case is hard, let's just use 0 if we're looking for red and see > 90,
        // and use 179 if we're looking for red >= 0
        if (H <= H_DIST && abs(H-Href) > 90)
            h_sum += 0;
        else if (H >= 180 - H_DIST && abs(H-Href) > 90)
            h_sum += 179;
        else
            h_sum += (unsigned)H;
                
        s_sum += (unsigned)S;
        v_sum += (unsigned)V;
        num_pixels++;

        return true;
    }
    return false;
}

void mvMeanShift::meanshift_internal(IplImage* src_scratch) {
// note this will treat the image as if it was in HSV format
// src_scratch is the src image passed in by the user, which will now be used as a scratch
#ifdef M_DEBUG
    cvNamedWindow("mvMeanShift debug", CV_WINDOW_AUTOSIZE);
#endif

    bin_MeanShift.start();
    
    cvZero (src_scratch);

    // we will be filtering ds_scratch_3 to src_scratch, then copying the data back to ds_scratch_3
    unsigned char* imgPtr, *resPtr;
    unsigned char* imgLast = (unsigned char*) (ds_scratch_3->imageData+ds_scratch_3->height*ds_scratch_3->widthStep);

    for (int r = 1; r < ds_scratch_3->height-1; r++) {                         
        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*KERNEL_RAD*ds_scratch_3->widthStep + 3*KERNEL_RAD); // imgPtr = first pixel of rth's row
        resPtr = (unsigned char*) (src_scratch->imageData + r*KERNEL_RAD*src_scratch->widthStep + 3*KERNEL_RAD);
        
        if (imgPtr >= imgLast)
            break;

        for (int c = 3*KERNEL_RAD; c < ds_scratch_3->width; c++) {
            // skip pixel if already visited
            if (resPtr[1] != 0) {
                imgPtr += 3;
                resPtr += 3;
                continue;
            }

            // check if the src pixel meats S,V min reqs
            if (imgPtr[1] >= S_MIN && imgPtr[2] >= V_MIN) {
                unsigned char* tempPtr;
                unsigned H2 = imgPtr[0], S2 = imgPtr[1], V2 = imgPtr[2];
                unsigned good_pixels = 1, total_pixels = 1;

                // go thru each pixel in the kernel
                for (int i = 0; i < KERNEL_AREA; i++) {
                    tempPtr = imgPtr + 3*kernel_point_array[i];

                    add_pixel_if_within_range (tempPtr, imgPtr, H2, S2, V2, good_pixels);

                    total_pixels++;
                }

                // if good enough, visit every pixel in the kernel and set value equal to average
                if (GOOD_PIXELS_FACTOR*good_pixels >= total_pixels) {
                    unsigned char Hnew = H2 / good_pixels;
                    unsigned char Snew = S2 / good_pixels;
                    unsigned char Vnew = V2 / good_pixels;

                    for (int i = 0; i < KERNEL_AREA; i++) {
                        tempPtr = resPtr + 3*kernel_point_array[i];
                        tempPtr[0] = Hnew;
                        tempPtr[1] = Snew;
                        tempPtr[2] = Vnew;
                    }
                }
            }
            
            imgPtr += 3;
            resPtr += 3;

#ifdef M_DEBUG
            cvShowImage("mvMeanShift debug", src_scratch);
            cvWaitKey(2);
#endif
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

                if (imgPtr[1] != 0 && (hue_box[i])->check_hsv(imgPtr[0], imgPtr[1], imgPtr[2])) {
                    *resPtr = (unsigned char) hue_box[i]->BOX_COLOR;
                }
            }

            imgPtr += 3;
            resPtr++;
        }
    }
}

void mvMeanShift::colorFilter_internal_adaptive_hue() {
// this function goes over the Hue_Box array and if a pixel falls inside box X, it marks that pixel
// with value X. Uses adaptive hue box so the range of hue can change
    unsigned char *imgPtr, *resPtr;
    for (int r = 0; r < ds_scratch->height; r++) {                        
        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*ds_scratch_3->widthStep);
        resPtr = (unsigned char*) (ds_scratch->imageData + r*ds_scratch->widthStep); 
   
        for (int c = 0; c < ds_scratch->width; c++) {
            *resPtr = 0;

            for (int i = 0; i < NUM_BOXES; i++) {
                if (!(hue_box[i])->is_enabled())
                    continue;

                if (imgPtr[1] != 0 && (hue_box[i])->check_hsv_adaptive_hue(imgPtr[0], imgPtr[1], imgPtr[2])) {
                    *resPtr = (unsigned char) hue_box[i]->BOX_COLOR;
                }
            }

            imgPtr += 3;
            resPtr++;
        }
    }

    for (int i = 0; i < NUM_BOXES; i++) {
        hue_box[i]->update_hue();
    }
}


void mvMeanShift::watershed(IplImage* src, IplImage* dst) {
    downsample_from (src);
    cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

    meanshift_internal(src);  

    IplImage* hue_img = cvCreateImage(cvGetSize(ds_scratch), IPL_DEPTH_8U, 1);
    IplImage* sat_img = cvCreateImage(cvGetSize(ds_scratch), IPL_DEPTH_8U, 1);
    unsigned char *imgPtr, *huePtr, *satPtr;
    for (int r = 0; r < ds_scratch_3->height; r++) {                         
        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*ds_scratch_3->widthStep);
        huePtr = (unsigned char*) (hue_img->imageData + r*hue_img->widthStep);
        satPtr = (unsigned char*) (sat_img->imageData + r*sat_img->widthStep);
        for (int c = 0; c < ds_scratch_3->width; c++) {
            // copy hue and sat over if S/V min fulfilled
            //if (imgPtr[1] >= S_MIN && imgPtr[2] >= V_MIN) {
                *huePtr = imgPtr[0];
                *satPtr = imgPtr[1];
            //}
            
            imgPtr += 3;
            huePtr++;
            satPtr++;
        }
    }

    IplImage* hue_derivative = cvCreateImage(cvGetSize(hue_img), IPL_DEPTH_16S, 1);
    IplImage* sat_derivative = cvCreateImage(cvGetSize(hue_img), IPL_DEPTH_16S, 1);
    cvZero(hue_derivative);

    cvSobel (hue_img, hue_derivative, 1,1, 3);
    cvAbs(hue_derivative,hue_derivative);
    cvSobel (sat_img, sat_derivative, 1,1, 3);
    cvAbs(sat_derivative, sat_derivative);
    cvAdd(hue_derivative,sat_derivative,hue_derivative);
    //cvCopy(sat_derivative,hue_derivative);

    CvScalar avg_s, stdev_s;
    cvAvgSdv(hue_derivative, &avg_s, &stdev_s);
    short avg = avg_s.val[0];
    short stdev = stdev_s.val[0];
    short thresh = avg+stdev;
    
    short* shortPtr;
    for (int r = 0; r < hue_derivative->height; r++) {                         
        shortPtr = (short*) (hue_derivative->imageData + r*hue_derivative->widthStep);
        satPtr = (unsigned char*) (sat_img->imageData + r*sat_img->widthStep);
        for (int c = 0; c < hue_derivative->width; c++) {
            if (*shortPtr > thresh)
                *satPtr = 255;
            else 
                *satPtr = 0; //(unsigned char) (*shortPtr *127 / thresh);
            shortPtr++;
            satPtr++;
        }
    }

    cvNamedWindow("mvMeanShift",CV_WINDOW_AUTOSIZE);
    cvShowImage("mvMeanShift", sat_img);
    
    cvReleaseImage(&hue_derivative);
    cvReleaseImage(&sat_derivative);
    cvReleaseImage(&hue_img);
    cvReleaseImage(&sat_img);
    upsample_to_3 (dst);
}

void mvMeanShift::flood_image(IplImage* src, IplImage* dst) {
    downsample_from (src);
    cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

    //meanshift_internal (src);
        
    cvZero (ds_scratch);
    unsigned box_num = 1;
    for (int r = 0; r < ds_scratch->height; r+=10) {                         
        for (int c = 0; c < ds_scratch->width; c+=10) {
            flood_from_pixel (r,c, box_num++);
        }
    }
    printf ("\n");

    unsigned char *imgPtr, *resPtr;
    for (int r = 0; r < ds_scratch->height; r++) {                         
        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*ds_scratch_3->widthStep);
        resPtr = (unsigned char*) (ds_scratch->imageData + r*ds_scratch->widthStep);
        for (int c = 0; c < ds_scratch->width; c++) {
            *imgPtr = *resPtr;
            imgPtr += 3;
            resPtr++;
        }
    }

    std::vector<Color_Box>::iterator iter_end = color_box_vector.end();
    for (std::vector<Color_Box>::iterator iter = color_box_vector.begin(); iter != iter_end; ++iter)
        printf ("color_box (%d): %d %d %d\n", iter->n_pixels, iter->hue, iter->sat, iter->val);
    printf ("\n");
    color_box_vector.clear();

    cvCvtColor(ds_scratch_3, ds_scratch_3, CV_HSV2BGR);
    upsample_to_3 (dst);
}

void mvMeanShift::flood_from_pixel(int R, int C, unsigned box_number) {
// assumes ds_scratch is zeroed as needed and does not use profile bin
//#define FLOOD_DEBUG
#ifdef FLOOD_DEBUG
     cvNamedWindow("mvMeanShift debug");
#endif

    unsigned char* imgPtr = (unsigned char*) (ds_scratch_3->imageData + R*ds_scratch_3->widthStep+C*3);
    unsigned char* resPtr = (unsigned char*) (ds_scratch->imageData + R*ds_scratch->widthStep+C);

    unsigned char* imgPtrOrig = imgPtr;

    if (*imgPtr == 0 || *resPtr != 0)
        return;

    // create a color model
    Color_Box color_box(imgPtr[0],imgPtr[1],imgPtr[2],box_number); 

    // use a queue, each time we visit a new pixel, check if its a "good" pixel and queue
    // visits to neighbours. Good pixels are marked with GOOD_PIXEL
    // loop until queue empty
    std::vector < std::pair<int,int> > Point_Array;
    Point_Array.push_back(std::make_pair(R,C));

    do {
        // dequeue the front pixel
        int r = Point_Array.back().first;
        int c = Point_Array.back().second;
        resPtr = (unsigned char*) (ds_scratch->imageData + r*ds_scratch->widthStep+c);
        Point_Array.pop_back();
        
        // check if we've visited this pixel before
        if (*resPtr != 0)
            continue;

        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*ds_scratch_3->widthStep+c*3);
        bool pixel_good = add_pixel_if_within_range (imgPtr, imgPtrOrig, color_box.hue, color_box.sat, color_box.val, color_box.n_pixels);
        if (pixel_good) {
            // mark pixel as visited
            *resPtr = GOOD_PIXEL;

            // queue neighbours
            if (c > 0 && resPtr[-1] == 0)
                Point_Array.push_back(std::make_pair(r,c-1));
            if (c < ds_scratch->width-1 && resPtr[1] == 0)
                Point_Array.push_back(std::make_pair(r,c+1));
            if (r > 0 && resPtr[-ds_scratch->widthStep] == 0)
                Point_Array.push_back(std::make_pair(r-1,c));
            if (r < ds_scratch->height-1 && resPtr[ds_scratch->widthStep] == 0)
                Point_Array.push_back(std::make_pair(r+1,c));
        }
    } while (!Point_Array.empty());

    if (color_box.n_pixels < (unsigned)ds_scratch->width*ds_scratch->height/200)
        return;

    color_box.calc_average();

    bool merge_success = false;
    std::vector<Color_Box>::iterator iter_end = color_box_vector.end();
    for (std::vector<Color_Box>::iterator iter = color_box_vector.begin(); iter != iter_end; ++iter) {
        if (abs((int)iter->hue-(int)color_box.hue) <= H_DIST && abs((int)iter->sat-(int)color_box.sat) <= S_DIST) {
            iter->merge(color_box);
            merge_success = true;
            break;
        }
    }

    if (!merge_success)
        color_box_vector.push_back(color_box);
    
    for (int r = 0; r < ds_scratch->height; r++) {                         
        resPtr = (unsigned char*) (ds_scratch->imageData + r*ds_scratch->widthStep);
        for (int c = 0; c < ds_scratch->width; c++) {
            if (*resPtr == GOOD_PIXEL)
                *resPtr = color_box.box_number;
            resPtr++;
        }
    }

#ifdef FLOOD_DEBUG
    cvShowImage("mvMeanShift debug", ds_scratch);
    cvWaitKey(100);
#endif
}

//#########################################################################
//#### Functions for Hue_Box.
//#########################################################################
Hue_Box::Hue_Box (const char* settings_file, int box_number) {
// read the HUE_MIN and HUE_MAX based on box number. So if box_number is 2, it reads
// HUE_MIN_2 and HUE_MAX_2
    BOX_NUMBER = box_number;
    std::string box_number_str;
    if (box_number == 1)
        box_number_str = "_1";
    else if (box_number == 2)
        box_number_str = "_2";
    else if (box_number == 3)
        box_number_str = "_3";
    else {
        printf ("Invalid box_number %d when constructing Hue_Box!\n", box_number);
        exit (1);
    }

    std::string enabled_str = std::string("ENABLE_BOX") + box_number_str;
    read_mv_setting (settings_file, enabled_str.c_str(), BOX_ENABLED);

    if (!BOX_ENABLED)
        return;

    // read the box color
    std::string box_color_str = std::string("COLOR_BOX") + box_number_str;
    std::string box_color;
    read_mv_setting (settings_file, box_color_str.c_str(), box_color);
    BOX_COLOR = color_str_to_int (box_color);

    std::string hue_min_str = std::string("HUE_MIN") + box_number_str;        
    std::string hue_max_str = std::string("HUE_MAX") + box_number_str;
    std::string sat_min_str = std::string("SAT_MIN") + box_number_str;        
    std::string val_min_str = std::string("VAL_MIN") + box_number_str;

    read_mv_setting (settings_file, hue_min_str.c_str(), HUE_MIN);
    read_mv_setting (settings_file, hue_max_str.c_str(), HUE_MAX);
    read_mv_setting (settings_file, sat_min_str.c_str(), SAT_MIN);
    read_mv_setting (settings_file, val_min_str.c_str(), VAL_MIN);

    HUE_MIN_OUT = (HUE_MIN < HUE_GUTTER_LEN) ? 180+HUE_MIN-HUE_GUTTER_LEN : HUE_MIN-HUE_GUTTER_LEN;
    HUE_MAX_OUT = (HUE_MAX + HUE_GUTTER_LEN >= 180) ? HUE_MAX+HUE_GUTTER_LEN-180 : HUE_MAX+HUE_GUTTER_LEN;
    HUE_MIN_ADP = HUE_MIN;
    HUE_MAX_ADP = HUE_MAX;
    inner_count = min_inside_count = max_inside_count = min_outside_count = max_outside_count = 0;

    if (DEBUG) {
        printf ("Hue_Box Number %d Constructed\n", box_number);
        printf ("\tBox Color %s. Greyscale Value = %d\n", box_color.c_str(), BOX_COLOR);
        printf ("\tHue MinMax = [%d,%d]\n", HUE_MIN, HUE_MAX);
    }
}

bool Hue_Box::check_hsv_adaptive_hue (unsigned char hue, unsigned char sat, unsigned char val) {
    if (sat >= SAT_MIN && val >= VAL_MIN) {
        /// only 1 of these 3 can be true
        bool wrap_around_min_out = (HUE_MIN_OUT > HUE_MIN_ADP);
        bool wrap_around_max_out = (HUE_MAX_OUT < HUE_MAX_ADP);
        bool wrap_around = (HUE_MAX_ADP < HUE_MIN_ADP);

        if (wrap_around_min_out) {          // if the 180deg wraparound is between min_out and min_adp
            if (hue >= HUE_MIN_ADP && hue <= HUE_MAX_ADP) {
                inner_count++;
                return true;
            }
            else if ((hue <= HUE_MIN_OUT && hue <= HUE_MIN_ADP) || (hue >= HUE_MIN_OUT && hue >= HUE_MIN_ADP)) {
                min_outside_count++;
                return false;
            }
            else if (hue >= HUE_MAX_ADP && hue <= HUE_MAX_OUT) {
                max_outside_count++;
                return false;
            }
        }
        else if (wrap_around) {
            if ((hue >= HUE_MIN_ADP && hue >= HUE_MAX_ADP) || (hue <= HUE_MIN_ADP && hue <= HUE_MAX_ADP)) {
                inner_count++;
                return true;
            }
            else if (hue >= HUE_MIN_OUT && hue <= HUE_MIN_ADP) {
                min_outside_count++;
                return false;
            }
            else if (hue >= HUE_MAX_ADP && hue <= HUE_MAX_OUT) {
                max_outside_count++;
                return false;
            }
        }
        else if (wrap_around_max_out) {
            if (hue >= HUE_MIN_ADP && hue <= HUE_MAX_ADP) {
                inner_count++;
                return true;
            }
            else if (hue >= HUE_MIN_OUT && hue <= HUE_MIN_ADP) {
                min_outside_count++;
                return false;
            }
            else if ((hue >= HUE_MAX_ADP && hue >= HUE_MAX_OUT) || (hue <= HUE_MAX_ADP && hue <= HUE_MAX_OUT)) {
                max_outside_count++;
                return false;
            }
        }
        else {
            if (hue >= HUE_MIN_ADP && hue <= HUE_MAX_ADP) {
                inner_count++;
                return true;
            }
            else if (hue >= HUE_MIN_OUT && hue <= HUE_MIN_ADP) {
                min_outside_count++;
                return false;
            }
            else if (hue >= HUE_MAX_ADP && hue <= HUE_MAX_OUT) {
                max_outside_count++;
                return false;
            }
        }
    }
    return false;
}

void Hue_Box::update_hue () {
    printf ("Counts: %d  %d  %d  %d  %d\n", min_outside_count,min_inside_count, inner_count, max_inside_count, max_outside_count);
    
    if (abs(HUE_MIN_ADP-HUE_MIN) <= 10 && 6*min_outside_count > inner_count) {
        HUE_MIN_ADP = (HUE_MIN_ADP < HUE_ADP_LEN) ? 180+HUE_MIN_ADP-HUE_ADP_LEN : HUE_MIN_ADP-HUE_ADP_LEN;
        HUE_MIN_OUT = (HUE_MIN_OUT < HUE_ADP_LEN) ? 180+HUE_MIN_OUT-HUE_ADP_LEN : HUE_MIN_OUT-HUE_ADP_LEN;
    }
    else if (HUE_MAX_ADP > HUE_MIN_ADP+HUE_ADP_LEN && 60*min_outside_count < inner_count) {
        HUE_MIN_ADP = (HUE_MIN_ADP + HUE_ADP_LEN >= 180) ? HUE_MIN_ADP+HUE_ADP_LEN-180 : HUE_MIN_ADP+HUE_ADP_LEN;
        HUE_MIN_OUT = (HUE_MIN_OUT + HUE_ADP_LEN >= 180) ? HUE_MIN_OUT+HUE_ADP_LEN-180 : HUE_MIN_OUT+HUE_ADP_LEN;
    }

    if (abs(HUE_MAX_ADP-HUE_MAX) <= 10 && 6*max_outside_count > inner_count) {
        HUE_MAX_ADP = (HUE_MAX_ADP + HUE_ADP_LEN >= 180) ? HUE_MAX_ADP+HUE_ADP_LEN-180 : HUE_MAX_ADP+HUE_ADP_LEN;
        HUE_MAX_OUT = (HUE_MAX_OUT + HUE_ADP_LEN >= 180) ? HUE_MAX_OUT+HUE_ADP_LEN-180 : HUE_MAX_OUT+HUE_ADP_LEN;
    }
    else if (HUE_MAX_ADP > HUE_MIN_ADP+HUE_ADP_LEN && 60*max_outside_count < inner_count) {
        HUE_MAX_ADP = (HUE_MAX_ADP < HUE_ADP_LEN) ? 180+HUE_MAX_ADP-HUE_ADP_LEN : HUE_MAX_ADP-HUE_ADP_LEN;
        HUE_MAX_OUT = (HUE_MAX_OUT < HUE_ADP_LEN) ? 180+HUE_MAX_OUT-HUE_ADP_LEN : HUE_MAX_OUT-HUE_ADP_LEN;
    }

    inner_count = min_inside_count = max_inside_count = min_outside_count = max_outside_count = 0;
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

