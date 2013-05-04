#include "mvColorFilter.h"
#include <math.h>

mvAdvancedColorFilter::mvAdvancedColorFilter (const char* settings_file) : 
    bin_Resize ("mvAdvanced - Resize"),
    bin_MeanShift ("mvAdvanced - MeanShift"),
    bin_Filter ("mvAdvanced - Filter")
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

mvAdvancedColorFilter::~mvAdvancedColorFilter () {
    delete[] kernel_point_array;

    for (int i = 0; i < NUM_BOXES; i++)
        delete hue_box[i];

    cvReleaseImage (&ds_scratch_3);
    cvReleaseImageHeader (&ds_scratch);
}

void mvAdvancedColorFilter::mean_shift(IplImage* src, IplImage* dst) {
      downsample_from (src);
      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

      meanshift_internal(src);

      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_HSV2BGR);
      upsample_to_3 (dst);
}

void mvAdvancedColorFilter::filter(IplImage* src, IplImage* dst) {
      downsample_from (src);
      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

      colorFilter_internal_adaptive_hue();

      upsample_to (dst);
}

void mvAdvancedColorFilter::combined_filter(IplImage* src, IplImage* dst) {
      downsample_from (src);
      cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

      meanshift_internal(src);
      colorFilter_internal();

      upsample_to (dst);
}
 
bool mvAdvancedColorFilter::add_pixel_if_within_range (unsigned char* pixel_to_add, unsigned char* ref_pixel,
                                unsigned &h_sum, unsigned &s_sum, unsigned &v_sum,
                                unsigned &num_pixels)
{
    int H = pixel_to_add[0];
    int S = pixel_to_add[1];
    int V = pixel_to_add[2];
    int Href = ref_pixel[0];
    int Sref = ref_pixel[1];
    int Vref = ref_pixel[2];

    int Hdelta = abs(H - Href);
    int Sdelta = abs(S - Sref);
    int Vdelta = abs(V - Vref);

    /*if (Sdelta <= S_DIST && 
        Vdelta <= V_DIST &&
        std::min(Hdelta,180-Hdelta) <= H_DIST)
    */
    if (std::min(Hdelta,180-Hdelta) + Sdelta + Vdelta < H_DIST)
    {
        // Circular red case is hard, let's just use 0 if we're looking for red and see > 90,
        // and use 179 if we're looking for red >= 0
        if (H <= H_DIST && Hdelta > 90)
            h_sum += 0;
        else if (H >= 180 - H_DIST && Hdelta > 90)
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

void mvAdvancedColorFilter::meanshift_internal(IplImage* src_scratch) {
// note this will treat the image as if it was in HSV format
// src_scratch is the src image passed in by the user, which will now be used as a scratch
#ifdef M_DEBUG
    cvNamedWindow("mvAdvancedColorFilter debug", CV_WINDOW_AUTOSIZE);
#endif

    bin_MeanShift.start();
    
    cvZero (src_scratch);

    // we will be filtering ds_scratch_3 to src_scratch, then copying the data back to ds_scratch_3
    unsigned char* imgPtr, *resPtr;
    unsigned char* imgLast = (unsigned char*) (ds_scratch_3->imageData+ds_scratch_3->height*ds_scratch_3->widthStep);

    for (int r = KERNEL_RAD; r < ds_scratch_3->height-KERNEL_RAD; r++) {                         
        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*ds_scratch_3->widthStep); // imgPtr = first pixel of rth's row
        resPtr = (unsigned char*) (src_scratch->imageData + r*src_scratch->widthStep);
        
        if (imgPtr >= imgLast)
            break;

        for (int c = 3*KERNEL_RAD; c < ds_scratch_3->width-3*KERNEL_RAD; c++) {
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
            cvShowImage("mvAdvancedColorFilter debug", src_scratch);
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

void mvAdvancedColorFilter::colorFilter_internal() {
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

void mvAdvancedColorFilter::colorFilter_internal_adaptive_hue() {
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


void mvAdvancedColorFilter::watershed(IplImage* src, IplImage* dst) {
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

    cvNamedWindow("mvAdvancedColorFilter",CV_WINDOW_AUTOSIZE);
    cvShowImage("mvAdvancedColorFilter", sat_img);
    
    cvReleaseImage(&hue_derivative);
    cvReleaseImage(&sat_derivative);
    cvReleaseImage(&hue_img);
    cvReleaseImage(&sat_img);
    upsample_to_3 (dst);
}

bool triple_has_more_pixels (Color_Triple t1, Color_Triple t2) {
    return (t1.n_pixels > t2.n_pixels);
}

#define FLOOD_IMAGE_COMPARE_WITH_ORIG

void mvAdvancedColorFilter::flood_image(IplImage* src, IplImage* dst) {
    #define M_DEBUG
    // this assert should be removed once scratch images in the last step is worked out
    assert (ds_scratch->width == dst->width && ds_scratch->height == dst->height);
    assert (src->nChannels == 3);
    assert (dst->nChannels == 1);

    bin_Resize.start();
    downsample_from (src);
    bin_Resize.stop();
    bin_Filter.start();
    cvCvtColor(ds_scratch_3, ds_scratch_3, CV_BGR2HSV);

    // we use ds_scratch as a mask image. If something is marked nonzero in ds_scratch the algorithm will usually
    // skip it.

    int widthStep = ds_scratch->widthStep;
    int widthStep3 = ds_scratch_3->widthStep;

    // mark all pixels below S/V threshold as bad
    //unsigned sat_min = hue_box[0]->SAT_MIN;
    //unsigned val_min = hue_box[0]->VAL_MIN;
    cvZero (ds_scratch);
    /*for (int r = 0; r < ds_scratch->height; r++) {                         
        unsigned char* scrPtr = (unsigned char*)(ds_scratch->imageData + r*widthStep);
        unsigned char* imgPtr = (unsigned char*)(ds_scratch_3->imageData + r*widthStep3);
        for (int c = 0; c < ds_scratch->width; c++) {
            if (imgPtr[1] < sat_min || imgPtr[2] < val_min)
                *scrPtr = BAD_PIXEL;
            imgPtr += 3;
            scrPtr ++;
        }
    }*/

#ifdef FLOOD_IMAGE_COMPARE_WITH_ORIG 
    //meanshift_internal (src);
#else
    //meanshift_internal (src);  
    // estimate the avg difference between a pixel and its neighbours in terms of H and S
    int hue_pixel_diff = 0;
    int sat_pixel_diff = 0;
    int val_pixel_diff = 0;
    int n_pixels_counted = 0;
    for (int r = 2; r < ds_scratch_3->height-2; r+=1) {                         
        unsigned char* scrPtr = (unsigned char*)(ds_scratch->imageData + r*widthStep);
        unsigned char* imgPtr = (unsigned char*)(ds_scratch_3->imageData + r*widthStep3);
        
        for (int c = 2; c < ds_scratch_3->width-2; c+=3) {
            if (*scrPtr == 0) {
                hue_pixel_diff += abs(imgPtr[0] - imgPtr[-3]);
                hue_pixel_diff += abs(imgPtr[0] - imgPtr[3]);
                hue_pixel_diff += abs(imgPtr[0] - imgPtr[-widthStep3]);
                hue_pixel_diff += abs(imgPtr[0] - imgPtr[widthStep3]);

                sat_pixel_diff += abs(imgPtr[1] - imgPtr[-3+1]);
                sat_pixel_diff += abs(imgPtr[1] - imgPtr[3+1]);
                sat_pixel_diff += abs(imgPtr[1] - imgPtr[-widthStep3+1]);
                sat_pixel_diff += abs(imgPtr[1] - imgPtr[widthStep3+1]);
                
                val_pixel_diff += abs(imgPtr[2] - imgPtr[-3+2]);
                val_pixel_diff += abs(imgPtr[2] - imgPtr[3+2]);
                val_pixel_diff += abs(imgPtr[2] - imgPtr[-widthStep3+2]);
                val_pixel_diff += abs(imgPtr[2] - imgPtr[widthStep3+2]);

                n_pixels_counted += 4;
            }
            imgPtr += 3;
            scrPtr ++;
        }
    }

    if (n_pixels_counted < 0)
        return;
    //H_DIST = hue_pixel_diff / n_pixels_counted + 20;
    //S_DIST = sat_pixel_diff / n_pixels_counted + 6;
    //V_DIST = val_pixel_diff / n_pixels_counted + 9;
    DEBUG_PRINT ("H_DIST=%d, S_DIST=%d, V_DIST=%d based on %d pixels\n", H_DIST, S_DIST, V_DIST, n_pixels_counted);
#endif

    // now perform image flooding to paint the pixels and extract color triplets    
    unsigned index_number = 100;
    for (int r = 3; r < ds_scratch->height - 3; r+=20) {                         
        for (int c = 3; c < ds_scratch->width - 3; c+=20) {
            if (flood_from_pixel (r,c, index_number))
                index_number++;
        }
    }

    std::sort (color_triple_vector.begin(), color_triple_vector.end(), triple_has_more_pixels);

    std::vector<Color_Triple>::iterator iter_end = color_triple_vector.end();
    for (std::vector<Color_Triple>::iterator iter = color_triple_vector.begin(); iter != iter_end; ++iter)
        DEBUG_PRINT ("color_triplet (%d): %d %d %d\n", iter->n_pixels, iter->hue, iter->sat, iter->val);
    DEBUG_PRINT ("\n");

    // go thru each active hue box and check if any of the models fit within the hue box
    // if so paint the pixels marked as those models to be the BOX number of that hue box
    int last = 0;
    
    cvZero(dst);
    for (int i = 0; i < NUM_BOXES; i++) {
        if (!hue_box[i]->BOX_ENABLED)
            continue;

        for (std::vector<Color_Triple>::iterator iter = color_triple_vector.begin(); iter != iter_end; ++iter) {
            if (hue_box[i]->check_hsv(iter->hue, iter->sat,iter->val)) {
                
                for (int k = 0; k < dst->height; k++) {
                    unsigned char* scrPtr = (unsigned char*)(ds_scratch->imageData + k*widthStep);
                    unsigned char* dstPtr = (unsigned char*)(dst->imageData + k*dst->widthStep);
                    for (int l = 0; l < dst->width; l++) {
                        /*if (*scrPtr != last) {
                            last = *scrPtr;
                            DEBUG_PRINT ("%d\n",*scrPtr);
                        }*/

                        if (*scrPtr == iter->index_number)
                            *dstPtr = 255;//hue_box[i]->BOX_NUMBER*50;
                        scrPtr++;
                        dstPtr++;
                    }
                }

            }
        }
    }

    color_triple_vector.clear();

    bin_Filter.stop();
}

bool mvAdvancedColorFilter::flood_from_pixel(int R, int C, unsigned index_number) {
// assumes ds_scratch is zeroed as needed and does not use profile bin
#define FLOOD_DEBUG
#ifdef FLOOD_DEBUG
     cvNamedWindow("mvAdvancedColorFilter debug");
#endif

    int widthStep = ds_scratch->widthStep;
    int widthStep3 = ds_scratch_3->widthStep;
    unsigned char* imgPtr = (unsigned char*) (ds_scratch_3->imageData + R*widthStep3+C*3);
    unsigned char* resPtr = (unsigned char*) (ds_scratch->imageData + R*widthStep+C);
    
    if (*imgPtr == 0 || *resPtr != 0)
        return false;

    // create a color model
    Color_Triple color_triple(imgPtr[0],imgPtr[1],imgPtr[2],index_number); 

    // use a queue, each time we visit a new pixel, check if its a "good" pixel and queue
    // visits to neighbours. Good pixels are marked with TEMP_PIXEL
    // loop until queue empty
    std::vector < std::pair<int,int> > Point_Array;
    Point_Array.push_back(std::make_pair(R,C));

#ifdef FLOOD_IMAGE_COMPARE_WITH_ORIG
    unsigned char* imgPtrOrig = imgPtr;
    do {
        // dequeue the front pixel
        int r = Point_Array.back().first;
        int c = Point_Array.back().second;
        Point_Array.pop_back();
        resPtr = (unsigned char*) (ds_scratch->imageData + r*widthStep+c);
        // check if we've visited this pixel before
        if (*resPtr != 0)
            continue;

        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*widthStep3+c*3);
        bool pixel_good = add_pixel_if_within_range (imgPtr, imgPtrOrig, color_triple.hue, color_triple.sat, color_triple.val, color_triple.n_pixels);
        if (pixel_good) {
            //DEBUG_PRINT ("passed %d,%d,%d vs %d,%d,%d\n",imgPtr[0],imgPtr[1],imgPtr[2],imgPtrOrig[0],imgPtrOrig[1],imgPtrOrig[2]);
            // mark pixel as visited
            *resPtr = TEMP_PIXEL;

            // queue neighbours
            if (c > 0 && resPtr[-1] == 0)
                Point_Array.push_back(std::make_pair(r,c-1));
            if (c < ds_scratch->width-1 && resPtr[1] == 0)
                Point_Array.push_back(std::make_pair(r,c+1));
            if (r > 0 && resPtr[-widthStep] == 0)
                Point_Array.push_back(std::make_pair(r-1,c));
            if (r < ds_scratch->height-1 && resPtr[widthStep] == 0)
                Point_Array.push_back(std::make_pair(r+1,c));
        }            
        else {
            DEBUG_PRINT ("failed %d,%d,%d vs %d,%d,%d\n",imgPtr[0],imgPtr[1],imgPtr[2],imgPtrOrig[0],imgPtrOrig[1],imgPtrOrig[2]);
        }
    } while (!Point_Array.empty());
#else
    do {
        // dequeue the front pixel
        int r = Point_Array.back().first;
        int c = Point_Array.back().second;
        Point_Array.pop_back();
        resPtr = (unsigned char*) (ds_scratch->imageData + r*widthStep + c);
        imgPtr = (unsigned char*) (ds_scratch_3->imageData + r*widthStep3 + c*3);

        // check each pixel around it
        if (c > 0 && resPtr[-1] == 0) { // left
            if (add_pixel_if_within_range (imgPtr-3, imgPtr, color_triple.hue, color_triple.sat, color_triple.val, color_triple.n_pixels)) {
                resPtr[-1] = TEMP_PIXEL;
                Point_Array.push_back(std::make_pair(r,c-1));
            }
        }
        if (c < ds_scratch->width-1 && resPtr[1] == 0) { // right
            if (add_pixel_if_within_range (imgPtr+3, imgPtr, color_triple.hue, color_triple.sat, color_triple.val, color_triple.n_pixels)) {
                resPtr[1] = TEMP_PIXEL;
                Point_Array.push_back(std::make_pair(r,c+1));
            }
        }
        if (r > 0 && resPtr[-widthStep] == 0) { // above
            if (add_pixel_if_within_range (imgPtr-widthStep3, imgPtr, color_triple.hue, color_triple.sat, color_triple.val, color_triple.n_pixels)) {
                resPtr[-widthStep] = TEMP_PIXEL;
                Point_Array.push_back(std::make_pair(r-1,c));
            }
        }
        if (r < ds_scratch->height-1 && resPtr[widthStep] == 0) { // below
            if (add_pixel_if_within_range (imgPtr+widthStep3, imgPtr, color_triple.hue, color_triple.sat, color_triple.val, color_triple.n_pixels)) {
                resPtr[widthStep] = TEMP_PIXEL;
                Point_Array.push_back(std::make_pair(r+1,c));
            }
        }
    } while (!Point_Array.empty());
#endif

    int final_index_number;
    
    // if the box doesnt contain enough pixels, throw it out
    if (color_triple.n_pixels < 100) {//(unsigned)ds_scratch->width*ds_scratch->height/300) {
        final_index_number = 0;
    }
    else {
        color_triple.calc_average();

        // attempt to merge the box with an existing box. This is if the boxes are very similar
        std::vector<Color_Triple>::iterator min_diff_iter;
        int min_diff = 9000;
        
        std::vector<Color_Triple>::iterator iter_end = color_triple_vector.end();
        for (std::vector<Color_Triple>::iterator iter = color_triple_vector.begin(); iter != iter_end; ++iter) {
            int diff = 2*abs((int)iter->hue-(int)color_triple.hue) + abs((int)iter->sat-(int)color_triple.sat) + abs((int)iter->val-(int)color_triple.val);
            if (diff < min_diff) {
                min_diff = diff;
                min_diff_iter = iter;
            }
        }
        // if could not merge, add the new box to the vector
        if (min_diff < 40) {
            min_diff_iter->merge(color_triple);
            final_index_number = min_diff_iter->index_number;
        }
        else {
            color_triple_vector.push_back(color_triple);
            final_index_number = color_triple.index_number;
        }
    }

    // paint the pixels with the appropriate index number
    for (int r = 0; r < ds_scratch->height; r++) {                         
        resPtr = (unsigned char*) (ds_scratch->imageData + r*widthStep);
        for (int c = 0; c < ds_scratch->width; c++) {
            if (*resPtr == TEMP_PIXEL)
                *resPtr = index_number;
            resPtr++;
        }
    }
  
#ifdef FLOOD_DEBUG
    cvShowImage("mvAdvancedColorFilter debug", ds_scratch);
    cvWaitKey(00);
#endif

    return (final_index_number != 0);
}

//#########################################################################
//#### Functions for Hue_Box.
//#########################################################################
Hue_Box::Hue_Box (const char* settings_file, int box_number) {
// read the HUE_MIN and HUE_MAX based on box number. So if box_number is 2, it reads
// HUE_MIN_2 and HUE_MAX_2
    #define M_DEBUG

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
    std::string sat_max_str = std::string("SAT_MAX") + box_number_str;        
    std::string val_min_str = std::string("VAL_MIN") + box_number_str;        
    std::string val_max_str = std::string("VAL_MAX") + box_number_str;

    read_mv_setting (settings_file, hue_min_str.c_str(), HUE_MIN);
    read_mv_setting (settings_file, hue_max_str.c_str(), HUE_MAX);
    read_mv_setting (settings_file, sat_min_str.c_str(), SAT_MIN);
    read_mv_setting (settings_file, sat_max_str.c_str(), SAT_MAX);
    read_mv_setting (settings_file, val_min_str.c_str(), VAL_MIN);
    read_mv_setting (settings_file, val_max_str.c_str(), VAL_MAX);

    HUE_MIN_OUT = (HUE_MIN < HUE_GUTTER_LEN) ? 180+HUE_MIN-HUE_GUTTER_LEN : HUE_MIN-HUE_GUTTER_LEN;
    HUE_MAX_OUT = (HUE_MAX + HUE_GUTTER_LEN >= 180) ? HUE_MAX+HUE_GUTTER_LEN-180 : HUE_MAX+HUE_GUTTER_LEN;
    HUE_MIN_ADP = HUE_MIN;
    HUE_MAX_ADP = HUE_MAX;
    inner_count = min_inside_count = max_inside_count = min_outside_count = max_outside_count = 0;

    DEBUG_PRINT ("Hue_Box Number %d Constructed\n", box_number);
    DEBUG_PRINT ("\tBox Color %s. Greyscale Value = %d\n", box_color.c_str(), BOX_COLOR);
    DEBUG_PRINT ("\tHue MinMax = [%d,%d]\n", HUE_MIN, HUE_MAX);
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
    DEBUG_PRINT ("Counts: %d  %d  %d  %d  %d\n", min_outside_count,min_inside_count, inner_count, max_inside_count, max_outside_count);
    
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
