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
    N_FRAMES_TO_KEEP = 12;
    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
}

MDA_VISION_MODULE_GATE:: ~MDA_VISION_MODULE_GATE () {
    mvReleaseScratchImage();
    mvReleaseScratchImage2();
}

void MDA_VISION_MODULE_GATE::add_frame (IplImage* src) {
    // shift the frames back by 1
    shift_frame_data (m_frame_data_vector, read_index, N_FRAMES_TO_KEEP);

    watershed_filter.watershed(src, gray_img, 1);
    window.showImage (src);

    COLOR_TRIPLE color;
    int H,S,V;
    MvRotatedBox rbox;
    MvRBoxVector rbox_vector;

    //float length_to_width = 1.d/GATE_REAL_SLENDERNESS;

    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        // check that the segment is roughly red
        tripletBGR2HSV (color.m1,color.m2,color.m3, H,S,V);
        /*if (S < 40 || V < 20 || !(H >= 150 || H < 80)) {
            //printf ("VISION_BUOY: rejected rectangle due to color: HSV=(%3d,%3d,%3d)\n", H,S,V);
            continue;
        }*/

        contour_filter.match_rectangle(gray_img_2, &rbox_vector, color, 7.0, 18.0, 1);
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
            if (abs(iter->angle) < 20) {
                m_frame_data_vector[read_index].assign_rbox_by_validity(*iter);
            }
        }
    }

    m_frame_data_vector[read_index].sort_rbox_by_x();

    if (m_frame_data_vector[read_index].is_valid()) {
        m_frame_data_vector[read_index].drawOntoImage(gray_img_2);
        window2.showImage (gray_img_2);
    }

    print_frames();
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_GATE::frame_calc () {
    // loop thru each frame
    // if some number of double segment, calc and return FULL_DETECT
    // else if some number of one segment, calc and return ONE_SEGMENT
    // else return NO_TARGET
    // always clear frames
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;

    int ANGLE_LIMIT = 30;
    int num_one_seg = 0;
    int num_two_seg = 0;
    for (int i = 0; i < N_FRAMES_TO_KEEP; i++) {
        //if (!m_frame_data_vector[i].has_data())
        //    return NO_TARGET;

        bool valid0 = m_frame_data_vector[i].rboxes_valid[0];
        bool valid1 = m_frame_data_vector[i].rboxes_valid[1];
        if (valid0 && valid1)
            num_two_seg++;
        else if (valid0)
            num_one_seg++;
    }

    if (num_two_seg >= 2) {
        DEBUG_PRINT ("FGate: 2 segments =)\n"); 
        
        // calculate average values for x,y,height, width
        m_pixel_x = m_pixel_y = 0;
        int gate_pixel_height = 0;
        int gate_pixel_width = 0;
        int nboxes = 0;
        for (int i = 0; i < N_FRAMES_TO_KEEP; i++) {
            // if this is a two segment frame
            if (m_frame_data_vector[i].rboxes_valid[0] && m_frame_data_vector[i].rboxes_valid[1]) {        
                MvRotatedBox Box1 = m_frame_data_vector[read_index].m_frame_boxes[0];
                MvRotatedBox Box2 = m_frame_data_vector[read_index].m_frame_boxes[1];
                
                // if segment is vertical
                if (abs(Box1.angle) > ANGLE_LIMIT || abs(Box2.angle) > ANGLE_LIMIT) {
                    continue;
                }
                // similarity of the segments
                if (Box1.length > 1.3*Box2.length || 1.3*Box1.length < Box2.length) {
                    continue;
                }
                
                m_pixel_x += (Box1.center.x + Box2.center.x) / 2;
                m_pixel_y += (Box1.center.y + Box2.center.y) / 2;
                gate_pixel_width += abs(Box2.center.x - Box1.center.x);
                gate_pixel_height += (Box1.length + Box2.length) / 2;
                nboxes++;
            }
        }
        if (nboxes == 0) {
        }
        else {
            m_pixel_x /= nboxes;
            m_pixel_y /= nboxes;
            gate_pixel_height /= nboxes;
            gate_pixel_width /= nboxes;
            
            float gate_width_to_height_ratio = abs((float)gate_pixel_width / gate_pixel_height);
            if (gate_width_to_height_ratio > 1.3*GATE_WIDTH_TO_HEIGHT_RATIO || 1.3*gate_width_to_height_ratio < GATE_WIDTH_TO_HEIGHT_RATIO) {
                goto CLEAR_FRAMES_AND_RETURN_NO_TARGET;
            }

            // calculate real distances
            m_range = (GATE_REAL_WIDTH * gray_img->width) / (gate_pixel_width * TAN_FOV_X);
            DEBUG_PRINT ("Gate Range: %d\n", m_range);
            
            retval = FULL_DETECT;
            goto CLEAR_FRAMES_AND_RETURN_TARGET;
        }
    }

    if (num_one_seg >= 4) {
        DEBUG_PRINT ("FGate: 1 segment =|\n");

        // calculate average values for x,y,height
        m_pixel_x = m_pixel_y = 0;
        int gate_pixel_height = 0;
        int nboxes = 0;
        for (int i = 0; i < N_FRAMES_TO_KEEP; i++) {
            // if this is a one segment frame
            if (m_frame_data_vector[i].rboxes_valid[0] && !m_frame_data_vector[i].rboxes_valid[1]) {
                MvRotatedBox Box = m_frame_data_vector[read_index].m_frame_boxes[0];
                    
                // if segment is vertical
                if (abs(Box.angle) <= ANGLE_LIMIT) {
                    m_pixel_x += Box.center.x;
                    m_pixel_y += Box.center.y;
                    gate_pixel_height += Box.length;
                    nboxes++;
                }
            }
        }
        if (nboxes == 0) {
            goto CLEAR_FRAMES_AND_RETURN_NO_TARGET;
        }
        m_pixel_x /= nboxes;
        m_pixel_y /= nboxes;
        gate_pixel_height /= nboxes;
    
        /// calculate range
        m_range = (GATE_REAL_HEIGHT * gray_img->height) / (gate_pixel_height * TAN_FOV_Y);
        DEBUG_PRINT ("FGate Range: %d\n", m_range);

        retval = ONE_SEGMENT;       
        goto CLEAR_FRAMES_AND_RETURN_TARGET;
    }

CLEAR_FRAMES_AND_RETURN_NO_TARGET:
    return NO_TARGET;
CLEAR_FRAMES_AND_RETURN_TARGET:
    m_pixel_x -= gray_img->width/2;
    m_pixel_y -= gray_img->height/2;
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
    DEBUG_PRINT ("FGate: (%d,%d) (%5.2f,%5.2f)\n", m_pixel_x, m_pixel_y, m_angular_x, m_angular_y); 
    return retval;
}
