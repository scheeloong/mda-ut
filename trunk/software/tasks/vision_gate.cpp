#include "mda_vision.h"

#define M_DEBUG
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

const char MDA_VISION_MODULE_GATE::MDA_VISION_GATE_SETTINGS[] = "vision_gate_settings.csv";

/// ########################################################################
/// MODULE_GATE methods
/// ########################################################################
MDA_VISION_MODULE_GATE:: MDA_VISION_MODULE_GATE () :
	window (mvWindow("Gate Vision Module")),
    window2 (mvWindow("Gate Vision Module 2")),
	HoughLines (mvHoughLines(MDA_VISION_GATE_SETTINGS)),
	lines (mvLines())
{
    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
}

MDA_VISION_MODULE_GATE:: ~MDA_VISION_MODULE_GATE () {
    mvReleaseScratchImage();
    mvReleaseScratchImage2();
}

void MDA_VISION_MODULE_GATE:: primary_filter (IplImage* src) {
    // shift the frames back by 1
    shift_frame_data (m_frame_data_vector, read_index, N_FRAMES_TO_KEEP);

    watershed_filter.watershed(src, gray_img);
    window.showImage (gray_img);

    COLOR_TRIPLE color;
    int H,S,V;
    MvRotatedBox rbox;
    MvRBoxVector rbox_vector;

    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        // check that the segment is roughly red
        tripletBGR2HSV (color.m1,color.m2,color.m3, H,S,V);
        /*if (S < 40 || V < 20 || !(H >= 150 || H < 80)) {
            //printf ("VISION_BUOY: rejected rectangle due to color: HSV=(%3d,%3d,%3d)\n", H,S,V);
            continue;
        }*/

        contour_filter.match_rectangle(gray_img_2, &rbox_vector, color, 9.0, 15.0);
        //window2.showImage (gray_img_2);
    }

    // debug only
    cvCopy (gray_img, gray_img_2);

    printf ("rbox_vector size = %d\n", static_cast<int>(rbox_vector.size()));
    if (rbox_vector.size() > 0) {
        MvRBoxVector::iterator iter = rbox_vector.begin();
        MvRBoxVector::iterator iter_end = rbox_vector.end();
        
        // this stores the rects with 2 best validity
        for (; iter != iter_end; ++iter) {
            m_frame_data_vector[read_index].assign_rbox_by_validity(*iter); 
        }
    }

    m_frame_data_vector[read_index].sort_rbox_by_x();

    if (m_frame_data_vector[read_index].is_valid()) {
        m_frame_data_vector[read_index].drawOntoImage(gray_img_2);
        window2.showImage (gray_img_2);
    }

    //print_frames();
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_GATE:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
    unsigned nboxes = (m_frame_data_vector[read_index].rboxes_valid[0]?1:0) + (m_frame_data_vector[read_index].rboxes_valid[1]?1:0);

    if (nboxes == 0) {
        printf ("Gate: No segments =(\n");
        return NO_TARGET;
    }
    else if (nboxes == 1) {
        /// single line, not much we can do but return its center and estimate range
        DEBUG_PRINT ("Gate: 1 segment =|\n");

        MvRotatedBox Box = m_frame_data_vector[read_index].m_frame_boxes[0];
        m_pixel_x = Box.center.x;
        m_pixel_y = Box.center.y;

        /// check that the line is at least vertical
        if (abs(Box.angle) > 20) {
            DEBUG_PRINT ("Gate Sanity Failure: Single line not vertical enough\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;   
        }
    
        /// calculate range if we pass sanity check
        m_range = (GATE_REAL_HEIGHT * gray_img->height) / (Box.length * TAN_FOV_Y);
        DEBUG_PRINT ("Gate Range: %d\n", m_range);

        retval = ONE_SEGMENT;       
        goto RETURN_CENTROID;
    }
    else {
        assert (nboxes == 2);
        DEBUG_PRINT ("Gate: 2 segments =)\n"); 
        
        MvRotatedBox Box1 = m_frame_data_vector[read_index].m_frame_boxes[0];
        MvRotatedBox Box2 = m_frame_data_vector[read_index].m_frame_boxes[1];
        m_pixel_x = (Box1.center.x + Box2.center.x) / 2;
        m_pixel_y = (Box1.center.y + Box2.center.y) / 2;        
        int gate_pixel_width = abs(Box2.center.x - Box1.center.x);
        int gate_pixel_height = (Box1.length + Box2.length) / 2;
        float gate_width_to_height_ratio = abs((float)gate_pixel_width / gate_pixel_height);

        /// sanity checks
        if (abs(Box1.angle) > 20 || abs(Box2.angle) > 20) {
            DEBUG_PRINT ("Gate Sanity Failure: One of the segments is not vertical enough\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;   
        } 
        if (Box1.length > 1.3*Box2.length || 1.3*Box1.length < Box2.length) {
            DEBUG_PRINT ("Gate Sanity Failure: Segments too dissimilar\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        } 
        if (gate_width_to_height_ratio > 1.3*GATE_WIDTH_TO_HEIGHT_RATIO || 1.3*gate_width_to_height_ratio < GATE_WIDTH_TO_HEIGHT_RATIO) {
            DEBUG_PRINT ("Gate Sanity Failure: Gate dimensions inconsistent with data\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }

        // calculate real distances
        m_range = (GATE_REAL_WIDTH * gray_img->width) / (gate_pixel_width * TAN_FOV_X);
        DEBUG_PRINT ("Gate Range: %d\n", m_range);
        
        retval = FULL_DETECT;
        goto RETURN_CENTROID;
    }

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
        m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
        DEBUG_PRINT ("Gate: (%d,%d) (%5.2f,%5.2f)\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y); 
        return retval;
}
