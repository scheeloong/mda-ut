#include "mvColorFilter.h"
#include <math.h>

// Contains functions for mvAdvancedColorFilter that pertain to the watershed algorithm

#define M_DEBUG
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

// used in sorting color difference color triples
bool m3_less_than (COLOR_TRIPLE T1, COLOR_TRIPLE T2) {
    return T1.m3 < T2.m3;
}

void mvAdvancedColorFilter::watershed(IplImage* src, IplImage* dst) {
// attemps to use cvWaterShed to segment the image
    assert (src->nChannels == 3);
    assert (dst->nChannels == 1);

    bin_Resize.start();
    downsample_from (src);
    bin_Resize.stop();

    bin_Seed.start();
    watershed_markers_internal(src);
    bin_Seed.stop();

    bin_Filter.start();
    watershed_filter_internal(src,dst);
    bin_Filter.stop();

    // return curr_segment_iter to beginning
    curr_segment_iter = segment_color_hash.begin();
    curr_segment_index = 0;
    
    bin_Resize.start();
    upsample_to (dst);
    bin_Resize.stop();
}

void mvAdvancedColorFilter::watershed_markers_internal (IplImage* src) {
    // massively downsample - this smoothes the image
    cvResize (ds_scratch_3, ds_image, CV_INTER_LINEAR);
    
    // Sample the image at certain intervals - add sample to an array
    typedef std::pair<COLOR_TRIPLE, CvPoint> COLOR_POINT;
    typedef std::vector<COLOR_POINT> COLOR_POINT_VECTOR;
    COLOR_POINT_VECTOR color_point_vector;

    const int STEP_SIZE = 7;
    unsigned char * ptr;
    for (int r = STEP_SIZE/2; r < ds_image->height - STEP_SIZE/2; r+=STEP_SIZE) {                         
        ptr = (unsigned char*) (ds_image->imageData + r*ds_image->widthStep);
        for (int c = STEP_SIZE/2; c < ds_image->width - STEP_SIZE/2; c+=STEP_SIZE) {
            unsigned char* cptr = ptr + 3*c;
            COLOR_TRIPLE ct (cptr[0],cptr[1],cptr[2],0);

            color_point_vector.push_back(make_pair(ct, cvPoint(c,r)));
        }
    }

    int num_pixels = color_point_vector.size();

    // go thru each pair of pixels and calculate their color difference and add it to a vector
    // the pixels are represented by their indices in the color_point_vector
    // we'll use a vector of triples - m1 = index1, m2 = index2, m3 = diff
    COLOR_TRIPLE_VECTOR pair_difference_vector;
    for (int i = 0; i < num_pixels; i++) {
        for (int j = i+1; j < num_pixels; j++) {
            int diff = color_point_vector[i].first.diff(color_point_vector[j].first);
            COLOR_TRIPLE ct (i, j, diff, 0);

            pair_difference_vector.push_back(ct);
        }
    }

    // now sort the pair_difference_vector
    std::sort(pair_difference_vector.begin(), pair_difference_vector.end(), m3_less_than);

    // assign index numbers
    max_index_number = 10;
    int limit = pair_difference_vector.size()/2;
    for (int i = 0; i < limit; i++) {
        const int index1 = pair_difference_vector[i].m1;
        const int index2 = pair_difference_vector[i].m2;
        const int diff = pair_difference_vector[i].m3;

        if (diff > 80 || max_index_number > MAX_INDEX_NUMBER)
            break;

        const bool pixel1_unassigned = (color_point_vector[index1].first.index_number == 0);
        const bool pixel2_unassigned = (color_point_vector[index2].first.index_number == 0);

        // if neither pixel has been assigned a number
        if (pixel1_unassigned && pixel2_unassigned) {
            // assign both pixels the next number
            color_point_vector[index1].first.index_number = max_index_number;
            color_point_vector[index2].first.index_number = max_index_number;
            max_index_number += 10;
        }
        // if first has been assigned a number
        else if (!pixel1_unassigned && pixel2_unassigned) {
            // assign second the same number
            color_point_vector[index2].first.index_number = color_point_vector[index1].first.index_number;
        }
        else if (pixel1_unassigned && !pixel2_unassigned) {
            color_point_vector[index1].first.index_number = color_point_vector[index2].first.index_number;
        }
        // both have numbers
        else {
            // go thru entire color_point_vector. Every pixel with the second index gets the first index instead
            const unsigned index_to_find = color_point_vector[index2].first.index_number;
            const unsigned index_to_set = color_point_vector[index1].first.index_number;
            for (int j = 0; j < num_pixels; j++) 
                if (color_point_vector[j].first.index_number == index_to_find)
                    color_point_vector[j].first.index_number = index_to_set;
        }
    }

    // clear segment_color_hash, then populate the hash with the needed triples
    segment_color_hash.clear();
    for (int i = 0; i < num_pixels; i++) {
        int index_number = color_point_vector[i].first.index_number;
        
        if (index_number != 0) {
            unsigned char index_char = static_cast<unsigned char>(index_number);
            COLOR_TRIPLE T (0,0,0,index_number);
            segment_color_hash.insert(make_pair(index_char,T));
        }
    }

    // zero marker image and draw markers onto it
    // also draw marker positions onto src so we can see where the markers are
    cvZero (marker_img_32s);
    //printf ("Markers:\n");
    for (int i = 0; i < num_pixels; i++) {
        COLOR_TRIPLE ct = color_point_vector[i].first;
        CvPoint C = color_point_vector[i].second;
        int x = C.x*WATERSHED_DS_FACTOR;
        int y = C.y*WATERSHED_DS_FACTOR;

        if (ct.index_number != 0) {
            int *ptr = &CV_IMAGE_ELEM(marker_img_32s, int, y, x);
            unsigned char* srcPtr = &CV_IMAGE_ELEM(src, unsigned char, y, 3*x);
            
            *ptr = static_cast<int>(ct.index_number);
            srcPtr[0] = srcPtr[1] = srcPtr[2] = 255;
            srcPtr[-1]= srcPtr[-2]= srcPtr[-3]= 0;
            srcPtr[3] = srcPtr[4] = srcPtr[5] = 0;
        }
        //debug
        //printf ("\tmarker: location <%3d,%3d>: color (%3d,%3d,%3d) - %2d\n", x, y, ct.m1, ct.m2, ct.m3, ct.index_number);
    }
}

void mvAdvancedColorFilter::watershed_filter_internal (IplImage* src, IplImage* dst) {
    cvWatershed(ds_scratch_3, marker_img_32s);

    // go thru each pixel in the marker img and do two things
    // 1. fill in ds_scratch so we can show it and stuff later
    // 2. add each pixel from a segment to its entry in segment_color_hash
    for (int i = 0; i < marker_img_32s->height; i++) {
        for (int j = 0; j < marker_img_32s->width; j++) {
            int index_number = CV_IMAGE_ELEM(marker_img_32s, int, i, j);
            unsigned char* dstPtr = &CV_IMAGE_ELEM(ds_scratch, unsigned char, i, j);
            unsigned char* colorPtr = &CV_IMAGE_ELEM(ds_scratch_3, unsigned char, i, j*3);

            if (index_number == -1) {
                *dstPtr = 0;
            }
            else {
                unsigned char index_char = static_cast<unsigned char>(index_number);
                // 1.
                *dstPtr = index_char * max_index_number / MAX_INDEX_NUMBER;
                // 2.
                segment_color_hash[index_char].add_pixel(colorPtr[0],colorPtr[1],colorPtr[2]);
            }
        }
    }

    printf ("Watershed Segments:\n");
    // calculate the mean color profile of each segment
    std::map<unsigned char,COLOR_TRIPLE>::iterator seg_iter = segment_color_hash.begin();
    std::map<unsigned char,COLOR_TRIPLE>::iterator seg_iter_end = segment_color_hash.end();
    for (; seg_iter != seg_iter_end; ++seg_iter) {
        COLOR_TRIPLE* ct_ptr = &(seg_iter->second);
        if (ct_ptr->n_pixels > 0) {
            ct_ptr->calc_average();
            printf ("\tSegment: index %d (%d pixels): (%3d,%3d,%3d)\n", ct_ptr->index_number, ct_ptr->n_pixels, ct_ptr->m1, ct_ptr->m2, ct_ptr->m3);
        }
    }
}

bool mvAdvancedColorFilter::get_next_watershed_segment(IplImage* binary_img, COLOR_TRIPLE &T) {
    assert (binary_img->width == marker_img_32s->width);
    assert (binary_img->height == marker_img_32s->height);
    
    // check if we are out of segments
    //if (curr_segment_iter == segment_color_hash.end()) {
    if (curr_segment_index >= segment_color_hash.size()) {
        return false;   
    }

    // get the index number of the current segment, then obtain a binary image which is 1 for each pixel
    // on the watershed result that matches the index number, and 0 otherwise
    int index_number = static_cast<int>(curr_segment_iter->second.index_number);
    T = curr_segment_iter->second;
    cvCmpS (marker_img_32s, index_number, binary_img, CV_CMP_EQ);

    ++curr_segment_iter;
    ++curr_segment_index;
    return true;
}
