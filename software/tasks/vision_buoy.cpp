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
    Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT)),
    AdvancedCircles(MDA_VISION_BUOY_SETTINGS),
    Rect ("Rect_settings.csv")
{
    read_mv_setting (MDA_VISION_BUOY_SETTINGS, "TARGET_BLUE", TARGET_BLUE);
    read_mv_setting (MDA_VISION_BUOY_SETTINGS, "TARGET_GREEN", TARGET_GREEN);
    read_mv_setting (MDA_VISION_BUOY_SETTINGS, "TARGET_RED", TARGET_RED);
    read_mv_setting (MDA_VISION_BUOY_SETTINGS, "DIFF_THRESHOLD", DIFF_THRESHOLD_SETTING);

    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
    //filtered_img = mvGetScratchImage (); // common size

    read_index = 0;
    n_valid_frames = 0;
    n_valid_circle_frames = 0;
    n_valid_box_frames = 0;
}

MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY (const char* settings_file) :
    window (mvWindow("Buoy Vision Module")),
    window2 (mvWindow("Buoy Vision Module 2")),
    Morphology5 (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
    Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT)),
    AdvancedCircles(settings_file),
    Rect ("Rect_settings.csv")
{
    read_mv_setting (settings_file, "TARGET_BLUE", TARGET_BLUE);
    read_mv_setting (settings_file, "TARGET_GREEN", TARGET_GREEN);
    read_mv_setting (settings_file, "TARGET_RED", TARGET_RED);

    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
    //filtered_img = mvGetScratchImage (); // common size
    
    read_index = 0;
    n_valid_frames = 0;
    n_valid_circle_frames = 0;
    n_valid_box_frames = 0;
}

MDA_VISION_MODULE_BUOY:: ~MDA_VISION_MODULE_BUOY () {
    mvReleaseScratchImage();
    mvReleaseScratchImage2();
}

void MDA_VISION_MODULE_BUOY:: primary_filter (IplImage* src) {
    /*AdvancedColorFilter.filter (src, filtered_img);
    filtered_img->origin = src->origin;
  
    Morphology5.close(filtered_img, filtered_img);
    Rect.find (filtered_img);
    Rect.removeFromImage(filtered_img);

    Morphology5.gradient(filtered_img, filtered_img);

    AdvancedCircles.find (filtered_img);

    Rect.drawOntoImage(filtered_img);
    AdvancedCircles.drawOntoImage (filtered_img);

    window.showImage (filtered_img);*/

    watershed_filter.watershed(src, gray_img);
    window.showImage (gray_img);

    COLOR_TRIPLE color;
    MvCircle circle;
    MvCircleVector circle_vector;
    
    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        if (contour_filter.match_circle(gray_img_2, &circle) < 0)
            continue;

        circle.m1 = color.m1;
        circle.m2 = color.m2;
        circle.m3 = color.m3;
        circle_vector.push_back(circle);
    }

    if (circle_vector.size() > 0) {
        MvCircleVector::iterator iter = circle_vector.begin();
        MvCircleVector::iterator iter_end = circle_vector.end();
        int index = 0;
        cvZero (gray_img_2);

        printf ("vision_buoy: %d Circles Detected:\n", static_cast<int>(circle_vector.size()));
        for (; iter != iter_end; ++iter) {
            printf ("\tCircle #%d: (%3d,%3d), Rad=%5.1f, <%3d,%3d,%3d>\n", ++index,
                iter->center.x, iter->center.y, iter->radius, iter->m1, iter->m2, iter->m3);
            iter->drawOntoImage(gray_img_2);
        }

        // for now best circle is first circle
        MvCircle* best_circle = &(circle_vector.front());
        m_pixel_x = best_circle->center.x;
        m_pixel_y = best_circle->center.y;
        m_range = (BUOY_REAL_DIAMETER * gray_img->width) / (2*best_circle->radius * TAN_FOV_X);
    }

    window2.showImage (gray_img_2);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_BUOY:: calc_vci () {
/*    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;

    // We assume most of the time we have 1 circle. If we have >1 circle we take the first
    // circle in the list (which is the strongest) and return UNKNOWN_TARGET
    // We cant trust range if circle is too small, so in that case also return UNKNOWN_TARGET
    // and range is not valid if UNKNOWN_TARGET is returned

    unsigned nCircles = AdvancedCircles.ncircles();

    if (nCircles == 0) {
        printf ("Circles: No circles found =(\n");
        return NO_TARGET;
    }
*/
    /*
    printf ("ncircles = %d\n", AdvancedCircles.ncircles());
    for (int i = 0; i < AdvancedCircles.ncircles(); i++) {
        printf ("Circle #%d: (%d,%d, %f)\n", i+1, AdvancedCircles[i].x, AdvancedCircles[i].y, AdvancedCircles[i].rad);
    }*/
/*
    m_pixel_x = AdvancedCircles[0].x - filtered_img->width/2;
    m_pixel_y = AdvancedCircles[0].y - filtered_img->height/2;
    unsigned rad = AdvancedCircles[0].rad;

    assert (rad != 0);
    
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / filtered_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / filtered_img->height);

    if (nCircles == 1 && rad > MIN_PIXEL_RADIUS_FACTOR*filtered_img->height) {
        m_range = (BUOY_REAL_DIAMETER * filtered_img->width) / (2*rad * TAN_FOV_X);
        DEBUG_PRINT ("Buoy: (%d,%d) (%5.2f,%5.2f). Color = %s. Range = %d\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y, color_int_to_string(AdvancedCircles[0].color).c_str(), m_range);

        retval = FULL_DETECT;
    }
    else {
        DEBUG_PRINT ("Buoy: (%d,%d) (%5.2f,%5.2f). Color = %s. Range = Unknown\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y, color_int_to_string(AdvancedCircles[0].color).c_str());

        retval = UNKNOWN_TARGET;
    }
*/
    if (m_pixel_x == MV_UNDEFINED_VALUE || m_pixel_y == MV_UNDEFINED_VALUE || m_range == MV_UNDEFINED_VALUE)
        return NO_TARGET;

    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
    DEBUG_PRINT ("Buoy: (%d,%d) (%5.2f,%5.2f). Range = %d\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y, m_range);

    return FULL_DETECT;
}

bool MDA_VISION_MODULE_BUOY::rbox_stable (float threshold) {
    n_valid = 0;
    int i = read_index;
    std::vector<MvRotatedBox> rboxes;
    do {
        if (m_frame_data_vector[i].valid) {
            n_valid++;
            rboxes.push_back(m_frame_data_vector[i].m_frame_box[0]);
        }
        if (++i >= N_FRAMES_TO_KEEP) i = 0;
    } while (i != read_index);

    printf("rbox: n_valid %d VALID_FRAMES %d\n", n_valid, VALID_FRAMES);
    if (n_valid < VALID_FRAMES) return false;

    MvRotatedBox last_rbox = rboxes.back();
    int x_sum = 0, y_sum = 0, range_sum = 0;
    for (std::vector<MvRotatedBox>::iterator cit = rboxes.begin(); cit != rboxes.end(); ++cit) {
       MvRotatedBox curr_rbox = *cit;
       float difference = curr_rbox.diff(last_rbox);
       printf("difference %f\n", difference);
       if (difference > threshold) return false;
       last_rbox = curr_rbox;

       x_sum += curr_rbox.center.x;
       y_sum += curr_rbox.center.y;
       range_sum = (RBOX_REAL_LENGTH * gray_img->width) / (sqrt(curr_rbox.length) * TAN_FOV_X);
    }

    m_pixel_x = x_sum / n_valid;
    m_pixel_y = y_sum / n_valid;
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
    m_range = range_sum / n_valid;

    return true;
}

bool MDA_VISION_MODULE_BUOY::circle_stable (float threshold) {
    n_valid = 0;
    int i = read_index;
    std::vector<MvCircle> circles;
    do {
        if (m_frame_data_vector[i].valid) {
            n_valid++;
            circles.push_back(m_frame_data_vector[i].m_frame_circle);
        }
        if (++i >= N_FRAMES_TO_KEEP) i = 0;
    } while (i != read_index);

    printf("circle: n_valid %d VALID_FRAMES %d\n", n_valid, VALID_FRAMES);
    if (n_valid < VALID_FRAMES) return false;

    MvCircle last_circle = circles.back();
    int x_sum = 0, y_sum = 0, range_sum = 0;
    for (std::vector<MvCircle>::iterator cit = circles.begin(); cit != circles.end(); ++cit) {
       MvCircle curr_circle = *cit;
       float difference = curr_circle.diff(last_circle);
       printf("difference %f\n", difference);
       if (difference > threshold) return false;
       last_circle = curr_circle;

       x_sum += curr_circle.center.x;
       y_sum += curr_circle.center.y;
       range_sum += (BUOY_REAL_DIAMETER * gray_img->width) / (2*curr_circle.radius * TAN_FOV_X);
    }

    m_pixel_x = x_sum / n_valid;
    m_pixel_y = y_sum / n_valid;
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
    m_range = range_sum / n_valid;

    return true;
}

void MDA_VISION_MODULE_BUOY::add_frame (IplImage* src) {
    // shift the frames back by 1
    shift_frame_data (m_frame_data_vector, read_index, N_FRAMES_TO_KEEP);

    watershed_filter.watershed(src, gray_img);
    window.showImage (src);

    COLOR_TRIPLE color;
    MvCircle circle;
    MvCircleVector circle_vector;
    MvRotatedBox rbox;
    MvRBoxVector rbox_vector;

    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        if (contour_filter.match_circle(gray_img_2, &circle) > 0) {
            assign_color_to_shape (color, &circle);
            circle_vector.push_back(circle);            
        }
        if (contour_filter.match_rectangle(gray_img_2, &rbox, 2.5, 3.1) > 0) {
            assign_color_to_shape (color, &rbox);
            rbox_vector.push_back(rbox);
        }

        //window2.showImage (gray_img_2);
    }

    cvCopy (gray_img, gray_img_2);

    if (circle_vector.size() > 0) {
        MvCircleVector::iterator iter = circle_vector.begin();
        MvCircleVector::iterator iter_end = circle_vector.end();
        
        // for now, frame will store circle with best validity
        float best_validity = -1;
        for (; iter != iter_end; ++iter) {
            if (iter->validity > best_validity) {
                m_frame_data_vector[read_index].assign_circle(*iter);
                best_validity = iter->validity;
            }
        }

        m_pixel_x = m_frame_data_vector[read_index].m_frame_circle.center.x;
        m_pixel_y = m_frame_data_vector[read_index].m_frame_circle.center.y;
        m_range = (BUOY_REAL_DIAMETER * gray_img->width) / (2*m_frame_data_vector[read_index].m_frame_circle.radius * TAN_FOV_X);
    }

    if (rbox_vector.size() > 0) {
        MvRBoxVector::iterator iter = rbox_vector.begin();
        MvRBoxVector::iterator iter_end = rbox_vector.end();
        
        // for now, frame will store rect with best validity
        float best_validity = -1;
        for (; iter != iter_end; ++iter) {
            if (iter->validity > best_validity) {
                m_frame_data_vector[read_index].assign_rbox(*iter);
                best_validity = iter->validity;
            }
        }
    }

    if (m_frame_data_vector[read_index].valid) {
        m_frame_data_vector[read_index].drawOntoImage(gray_img_2);
        window2.showImage (gray_img_2);
    }

    print_frames();
    if (cvWaitKey(20) == 'q')
        exit(0);
}

void MDA_VISION_MODULE_BUOY::print_frames () {
        printf ("\nBUOY SAVED FRAMES\n");
        int i = read_index;
        int i2 = 0;
        do {
            printf ("Frame[%-2d]:\t", i2);        
            if (m_frame_data_vector[i].valid) {
                int n_circles = m_frame_data_vector[i].n_circles;
                int n_boxes = m_frame_data_vector[i].n_boxes;
                std::string color_str, color_str_2;

                color_str = color_int_to_string(m_frame_data_vector[i].m_frame_circle.color_int);
                printf ("%d Circles (%s)\t", n_circles, (n_circles > 0)?color_str.c_str():"---");

                color_str = color_int_to_string(m_frame_data_vector[i].m_frame_box[0].color_int);
                color_str_2 = color_int_to_string(m_frame_data_vector[i].m_frame_box[1].color_int);
                printf ("%d Boxes (%s,%s)\n", m_frame_data_vector[i].n_boxes,
                    (n_boxes > 0)?color_str.c_str():"---", (n_boxes > 1)?color_str_2.c_str():"---"
                    );
            }
            else
                printf ("Invalid\n");

            if (++i >= N_FRAMES_TO_KEEP) i = 0;
            i2++;
        } while (i != read_index && i2 < N_FRAMES_TO_KEEP);
    }
