#include "mda_vision.h"

#define M_DEBUG
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

float absf(float f){
    if(f < 0) return f*-1;
    else return f;
}

const char MDA_VISION_MODULE_PATH::MDA_VISION_PATH_SETTINGS[] = "vision_path_settings.csv";

/// ########################################################################
/// MODULE_PATH methods
/// ########################################################################
MDA_VISION_MODULE_PATH:: MDA_VISION_MODULE_PATH () :
	window (mvWindow("Path Vision 1")),
    window2 (mvWindow("Path Vision 2")),
    //HSVFilter (mvHSVFilter(MDA_VISION_PATH_SETTINGS)),
    Morphology (mvBinaryMorphology(19, 19, MV_KERN_RECT)),
    Morphology2 (mvBinaryMorphology(7, 7, MV_KERN_RECT))//,
    //HoughLines (mvHoughLines(MDA_VISION_PATH_SETTINGS))
{
    read_mv_setting (MDA_VISION_PATH_SETTINGS, "TARGET_BLUE", TARGET_BLUE);
    read_mv_setting (MDA_VISION_PATH_SETTINGS, "TARGET_GREEN", TARGET_GREEN);
    read_mv_setting (MDA_VISION_PATH_SETTINGS, "TARGET_RED", TARGET_RED);
    //read_mv_setting (MDA_VISION_PATH_SETTINGS, "DIFF_THRESHOLD", DIFF_THRESHOLD_SETTING);

    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
}

MDA_VISION_MODULE_PATH:: ~MDA_VISION_MODULE_PATH () {
    mvReleaseScratchImage();
    mvReleaseScratchImage2();
}

void MDA_VISION_MODULE_PATH::add_frame (IplImage* src) {
    // shift the frames back by 1
    shift_frame_data (m_frame_data_vector, read_index, N_FRAMES_TO_KEEP);

    watershed_filter.watershed(src, gray_img);
    //window.showImage (src);

    COLOR_TRIPLE color;
    int H,S,V;
    MvRotatedBox rbox;
    MvRBoxVector rbox_vector;

    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        // check that the segment is roughly red
        tripletBGR2HSV (color.m1,color.m2,color.m3, H,S,V);
        if (S < 20 || V < 50 || !(H >= 150 || H < 80)) {
            //printf ("VISION_BUOY: rejected rectangle due to color: HSV=(%3d,%3d,%3d)\n", H,S,V);
            continue;
        }

        contour_filter.match_rectangle(gray_img_2, &rbox_vector, color, 7.0, 12.0);
        contour_filter.drawOntoImage(gray_img_2);
    window.showImage (gray_img_2);
    }

    // debug only
    cvCopy (gray_img, gray_img_2);

    if (rbox_vector.size() > 0) {
        MvRBoxVector::iterator iter = rbox_vector.begin();
        MvRBoxVector::iterator iter_end = rbox_vector.end();
        
        // for now, frame will store rect with best validity
        for (; iter != iter_end; ++iter) {
            m_frame_data_vector[read_index].assign_rbox_by_validity(*iter);
        }

        m_pixel_x = m_frame_data_vector[read_index].m_frame_boxes[0].center.x;
        m_pixel_y = m_frame_data_vector[read_index].m_frame_boxes[0].center.y;
        //m_range = (PATH_REAL_LENGTH * gray_img->width) / (m_frame_data_vector[read_index].m_frame_boxes[0].length * TAN_FOV_X);
        m_range = (PATH_REAL_WIDTH * gray_img->width) / (m_frame_data_vector[read_index].m_frame_boxes[0].width * TAN_FOV_X);
        m_angle = m_frame_data_vector[read_index].m_frame_boxes[0].angle;
    }

    if (m_frame_data_vector[read_index].is_valid()) {
        m_frame_data_vector[read_index].drawOntoImage(gray_img_2);
        window2.showImage (gray_img_2);
    }

    //print_frames();
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_PATH:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
   
        if (m_pixel_x == MV_UNDEFINED_VALUE || m_pixel_y == MV_UNDEFINED_VALUE || m_angle == MV_UNDEFINED_VALUE)
            return NO_TARGET;

        m_pixel_x -= gray_img->width/2;
        m_pixel_y = gray_img->height/2 - m_pixel_y; // something feels wrong here, must test on camera

        retval = FULL_DETECT;
        m_angular_x = RAD_TO_DEG * atan((float) m_pixel_x / m_pixel_y);
        if (m_pixel_y < 0) {
            if (m_pixel_x > 0)
                m_angular_x += 180;
            else
                m_angular_x -= 180;
        }

        DEBUG_PRINT ("Path: (%d,%d) angular_pos=%5.2f, angle=%5.2f\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angle);
        
        return retval;
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_PATH::frame_calc () {
    // returns: NO_TARGET, FULL_DETECT
    //MDA_VISION_RETURN_CODE retval = FATAL_ERROR;

    n_valid = 0;
    int i = read_index;
    std::vector<MvRotatedBox> paths;
    
    // pull all the valid objects into an array
    do {
        if (m_frame_data_vector[i].has_data() && m_frame_data_vector[i].rboxes_valid[0]) {
            n_valid++;
            paths.push_back(m_frame_data_vector[i].m_frame_boxes[0]);
        }
        if (++i >= N_FRAMES_TO_KEEP) i = 0;
    } while (i != read_index);
    // check that we have enough valid objects
    if (n_valid < 3) {
        return NO_TARGET;
    }

    // diff each object wrt the previous one, exit if difference too large
    // otherwise find the average of position and range
    MvRotatedBox last_path = paths.back();
    int x_sum = 0, y_sum = 0, range_sum = 0;
    n_valid = 0; // recompute n_valid
    for (std::vector<MvRotatedBox>::iterator cit = paths.begin(); cit != paths.end(); ++cit) {
        MvRotatedBox curr_path = *cit;
        int color_diff = curr_path.color_diff(last_path);
        if (color_diff > 120) {
            DEBUG_PRINT ("\tcolor_diff=%d exceeds 120.\n", color_diff);
            continue;
        }
        int center_diff = curr_path.center_diff(last_path);
        if (center_diff > 80) {
            DEBUG_PRINT ("\tcenter_diff=%d exceeds threshold of 80.\n", center_diff);
            continue;
        }
        last_path = curr_path;

        x_sum += curr_path.center.x;
        y_sum += curr_path.center.y;
        range_sum += (PATH_REAL_LENGTH * gray_img->width) / (curr_path.length * TAN_FOV_X);
        n_valid++;
    }

    if (n_valid < 2) {
        return NO_TARGET;
    }

    m_pixel_x = x_sum / n_valid - gray_img->width/2;
    m_pixel_y = y_sum / n_valid - gray_img->height/2;
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
    m_range = range_sum / n_valid;
    m_angle = last_path.angle;
    return FULL_DETECT;
}
