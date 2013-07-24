#include "mda_vision.h"

#define M_DEBUG
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

#define CURR_FRAME m_frame_data_vector[i]

const char MDA_VISION_MODULE_BUOY::MDA_VISION_BUOY_SETTINGS[] = "vision_buoy_settings_new.csv";

/// #########################################################################
/// MODULE_BUOY methods
/// #########################################################################
MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY () :
    window (mvWindow("Buoy Vision Module")),
    window2 (mvWindow("Buoy Vision Module 2")),
    Morphology5 (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
    Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT))//,
    //AdvancedCircles(MDA_VISION_BUOY_SETTINGS),
    //Rect ("Rect_settings.csv")
{
    read_mv_setting (MDA_VISION_BUOY_SETTINGS, "TARGET_BLUE", TARGET_BLUE);
    read_mv_setting (MDA_VISION_BUOY_SETTINGS, "TARGET_GREEN", TARGET_GREEN);
    read_mv_setting (MDA_VISION_BUOY_SETTINGS, "TARGET_RED", TARGET_RED);
    read_mv_setting (MDA_VISION_BUOY_SETTINGS, "DIFF_THRESHOLD", DIFF_THRESHOLD_SETTING);

    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
    //filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY (const char* settings_file) :
    window (mvWindow("Buoy Vision Module")),
    window2 (mvWindow("Buoy Vision Module 2")),
    Morphology5 (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
    Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT))//,
    //AdvancedCircles(settings_file),
    //Rect ("Rect_settings.csv")
{
    read_mv_setting (settings_file, "TARGET_BLUE", TARGET_BLUE);
    read_mv_setting (settings_file, "TARGET_GREEN", TARGET_GREEN);
    read_mv_setting (settings_file, "TARGET_RED", TARGET_RED);

    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
    //filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_BUOY:: ~MDA_VISION_MODULE_BUOY () {
    mvReleaseScratchImage();
    mvReleaseScratchImage2();
}

void MDA_VISION_MODULE_BUOY::add_frame (IplImage* src) {
    // shift the frames back by 1
    shift_frame_data (m_frame_data_vector, read_index, N_FRAMES_TO_KEEP);

    watershed_filter.watershed(src, gray_img, 1);
    window.showImage (gray_img);

    COLOR_TRIPLE color;
    int H,S,V;
    MvCircle circle;
    MvCircleVector circle_vector;
    MvRotatedBox rbox;
    MvRBoxVector rbox_vector;

    float length_to_width = RBOX_REAL_LENGTH / RBOX_REAL_DIAMETER;

    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        cvCopy (gray_img_2, gray_img);
        tripletBGR2HSV (color.m1,color.m2,color.m3, H,S,V);
        if (S < 30 || V < 20) {
            //printf ("VISION_BUOY: rejected rectangle due to color: HSV=(%3d,%3d,%3d)\n", H,S,V);
            continue;
        }

        contour_filter.match_circle(gray_img_2, &circle_vector, color);
        contour_filter.match_rectangle(gray_img, &rbox_vector, color, length_to_width-0.5, length_to_width+0.8);        

        //window2.showImage (gray_img_2);
    }

    if (circle_vector.size() > 0) {
        MvCircleVector::iterator iter = circle_vector.begin();
        MvCircleVector::iterator iter_end = circle_vector.end();
        
        // for now, frame will store circle with best validity
        for (; iter != iter_end; ++iter) {
            m_frame_data_vector[read_index].assign_circle_by_validity(*iter);
        }
    }

    if (rbox_vector.size() > 0) {
        MvRBoxVector::iterator iter = rbox_vector.begin();
        MvRBoxVector::iterator iter_end = rbox_vector.end();
        
        // for now, frame will store rect with best validity
        for (; iter != iter_end; ++iter) {
            if (abs(iter->angle) <= 30 ) { // dont add if angle > 30 degree
                m_frame_data_vector[read_index].assign_rbox_by_validity(*iter);
            }
        }
    }

    m_frame_data_vector[read_index].sort_rbox_by_x();

    if (m_frame_data_vector[read_index].is_valid()) {
        m_frame_data_vector[read_index].drawOntoImage(gray_img_2);
        window2.showImage (gray_img_2);
    }

    //print_frames();
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_BUOY::frame_calc () {
    if (m_frame_data_vector[read_index].is_valid()) {
        return FULL_DETECT;
    }
    return UNKNOWN_TARGET;
}

void MDA_VISION_MODULE_BASE::print_frames () {
    printf ("\nSAVED FRAMES\n");
    int i = read_index;
    int i2 = 0;
    do {
        printf ("Frame[%-2d]:\t", i2);        
        if (m_frame_data_vector[i].is_valid()) {
            int n_circles = m_frame_data_vector[i].circle_valid?1:0;
            int n_boxes = (m_frame_data_vector[i].rboxes_valid[0]?1:0) + (m_frame_data_vector[i].rboxes_valid[1]?1:0);
            std::string color_str, color_str_2;

            color_str = color_int_to_string(m_frame_data_vector[i].m_frame_circle.color_int);
            printf ("%d Circles (%s)\t", n_circles, (n_circles > 0)?color_str.c_str():"---");

            color_str = color_int_to_string(m_frame_data_vector[i].m_frame_boxes[0].color_int);
            color_str_2 = color_int_to_string(m_frame_data_vector[i].m_frame_boxes[1].color_int);
            printf ("%d Boxes (%s,%s)\n", n_boxes,
                (n_boxes > 0)?color_str.c_str():"---", (n_boxes > 1)?color_str_2.c_str():"---"
                );
        }
        else
            printf ("Invalid\n");

        if (++i >= N_FRAMES_TO_KEEP) i = 0;
        i2++;
    } while (i != read_index && i2 < N_FRAMES_TO_KEEP);
}


bool MDA_VISION_MODULE_BUOY::rbox_stable (int rbox_index, float threshold) {
    assert (rbox_index >= 0 && rbox_index <= 1);

    n_valid = 0;
    int i = read_index;
    std::vector<MvRotatedBox> rboxes;
    
    // pull all the valid objects into an array
    do {
        if (m_frame_data_vector[i].has_data() && m_frame_data_vector[i].rboxes_valid[rbox_index]) {
            n_valid++;
            rboxes.push_back(m_frame_data_vector[i].m_frame_boxes[rbox_index]);
        }
        if (++i >= N_FRAMES_TO_KEEP) i = 0;
    } while (i != read_index);
    // check that we have enough valid objects
    if (n_valid < VALID_FRAMES) return false;

    // diff each object wrt the previous one, exit if difference too large
    // otherwise find the average of position and range
    MvRotatedBox last_rbox = rboxes.back();
    int x_sum = 0, y_sum = 0, range_sum = 0;
    n_valid = 0; // recompute n_valid
    for (std::vector<MvRotatedBox>::iterator cit = rboxes.begin(); cit != rboxes.end(); ++cit) {
        MvRotatedBox curr_rbox = *cit;
        if (curr_rbox.color_int != last_rbox.color_int) {
            DEBUG_PRINT ("rbox_stable: non-matching color! %s!=%s\n", 
                color_int_to_string(curr_rbox.color_int).c_str(), color_int_to_string(last_rbox.color_int).c_str());
            continue;
        }
        float difference = curr_rbox.diff(last_rbox);
        DEBUG_PRINT("\tcurr_rbox(%d,%d).  difference %f\n", curr_rbox.center.x, curr_rbox.center.y, difference);
        if (difference > threshold) {
            DEBUG_PRINT ("\texceeds threshold of %f.\n", threshold);
            continue;
        }
        //last_rbox = curr_rbox;

        x_sum += curr_rbox.center.x;
        y_sum += curr_rbox.center.y;
        range_sum += (RBOX_REAL_LENGTH * gray_img->width) / (curr_rbox.length * TAN_FOV_X);
        n_valid++;
    }

    if (n_valid < 2) {
        return false;
    }

    m_pixel_x = x_sum / n_valid - gray_img->width/2;
    m_pixel_y = y_sum / n_valid - gray_img->height/2;
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
    m_range = range_sum / n_valid;
    m_color = last_rbox.color_int;

    printf ("rbox_stable[%d]: OK: (%d, %d) range=%d, color=%s\n", rbox_index, m_pixel_x, m_pixel_y, m_range, 
        color_int_to_string(m_color).c_str());

    return true;
}

bool MDA_VISION_MODULE_BUOY::circle_stable (float threshold) {
    n_valid = 0;
    int i = read_index;
    std::vector<MvCircle> circles;

    // pull all the valid objects into an array
    do {
        if (m_frame_data_vector[i].is_valid()) {
            n_valid++;
            circles.push_back(m_frame_data_vector[i].m_frame_circle);
        }
        if (++i >= N_FRAMES_TO_KEEP) i = 0;
    } while (i != read_index);
    // check that we have enough valid objects
    DEBUG_PRINT("circle_stable: n_valid %d VALID_FRAMES %d\n", n_valid, VALID_FRAMES);
    if (n_valid < VALID_FRAMES) return false;

    // diff each object wrt the previous one, exit if difference too large
    // otherwise find the average of position and range
    MvCircle last_circle = circles.back();
    int x_sum = 0, y_sum = 0, range_sum = 0;
    for (std::vector<MvCircle>::iterator cit = circles.begin(); cit != circles.end(); ++cit) {
        MvCircle curr_circle = *cit;
        if (curr_circle.color_int != last_circle.color_int) {
            DEBUG_PRINT ("circle_stable: non-matching color! %s!=%s\n", 
                color_int_to_string(curr_circle.color_int).c_str(), color_int_to_string(last_circle.color_int).c_str());
        }
        float difference = curr_circle.diff(last_circle);
        DEBUG_PRINT("\tdifference %f\n", difference);
        if (difference > threshold) {
            DEBUG_PRINT ("\texceeds threshold of %f.\n", threshold);
            return false;
        }
        last_circle = curr_circle;

        x_sum += curr_circle.center.x;
        y_sum += curr_circle.center.y;
        range_sum += (BUOY_REAL_DIAMETER * gray_img->width) / (2*curr_circle.radius * TAN_FOV_X);
    }

    m_pixel_x = x_sum / n_valid - gray_img->width/2;
    m_pixel_y = y_sum / n_valid - gray_img->height/2;
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
    m_range = range_sum / n_valid;
    m_color = last_circle.color_int;

    printf ("circle_stable: OK: (%d, %d) range=%d, color=%s\n", m_pixel_x, m_pixel_y, m_range, 
        color_int_to_string(m_color).c_str());

    return true;
}
