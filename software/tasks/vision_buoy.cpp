#include "mda_vision.h"

#define M_DEBUG
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

const int ANGLE_LIMIT = 35;
const float LEN_TO_WIDTH_MAX = 4.0;
const float LEN_TO_WIDTH_MIN = 1.5;

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
    N_FRAMES_TO_KEEP = 60;
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

    // HSV hack!
    /*unsigned char *srcptr;
    int zeros = 0;
    for (int i = 0; i < src->height; i++) {
        srcptr = (unsigned char*)src->imageData + i*src->widthStep;
        for (int j = 0; j < src->width; j++) {
            if (srcptr[2] < 15) {
                srcptr[0] = 0;
                srcptr[1] = 0;
                srcptr[2] = 0;
                zeros++;
            }
            srcptr += 3;
        }
    }
    if (zeros > 0.999 * 400*300) {
        printf ("Path: add_frame: not enough pixels\n");
        return;
    }*/

    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        cvCopy (gray_img_2, gray_img);
        tripletBGR2HSV (color.m1,color.m2,color.m3, H,S,V);
        if (S < 30 || V < 20) {
            //printf ("VISION_BUOY: rejected rectangle due to color: HSV=(%3d,%3d,%3d)\n", H,S,V);
            continue;
        }

        contour_filter.match_circle(gray_img_2, &circle_vector, color);
        contour_filter.match_rectangle(gray_img, &rbox_vector, color, LEN_TO_WIDTH_MIN, LEN_TO_WIDTH_MAX);        

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
            if (abs(iter->angle) <= ANGLE_LIMIT) { // dont add if angle > 30 degree
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

void MDA_VISION_MODULE_BUOY::rbox_calc(MvRBoxVector* rboxes_returned, int nframes) {
    assert (rboxes_returned != NULL);
    MvRotatedBox input_rbox;
    m_rbox_segment_vector.clear();
    
    // go thru each frame and pull all individual segments into a vector
    // set i to point to the element 1 past read_index
    int i = read_index + 1;
    if (i >= N_FRAMES_TO_KEEP) i = 0;
    do {
        if (m_frame_data_vector[i].has_data() && m_frame_data_vector[i].rboxes_valid[0]) {
            m_rbox_segment_vector.push_back(m_frame_data_vector[i].m_frame_boxes[0]);
        }
        if (++i >= N_FRAMES_TO_KEEP) i = 0;
    } while (i != read_index);


    // bin the frames
    for (unsigned i = 0; i < m_rbox_segment_vector.size(); i++) {
        for (unsigned j = i+1; j < m_rbox_segment_vector.size(); j++) {
            CvPoint center1 = m_rbox_segment_vector[i].center;
            CvPoint center2 = m_rbox_segment_vector[j].center;  
            
            if (abs(center1.x-center2.x) < 25 &&
                abs(center1.y-center2.y) < 25 && 
                fabs(static_cast<float>(m_rbox_segment_vector[i].length)/m_rbox_segment_vector[j].length) < 1.5 &&
                fabs(static_cast<float>(m_rbox_segment_vector[i].width)/m_rbox_segment_vector[j].width) < 1.5 &&
                m_rbox_segment_vector[i].color_check(m_rbox_segment_vector[j], 20)
                //m_rbox_segment_vector[i].color_int == m_rbox_segment_vector[j].color_int
            )
            {
                m_rbox_segment_vector[i].shape_merge(m_rbox_segment_vector[j]);
                m_rbox_segment_vector.erase(m_rbox_segment_vector.begin()+j);
            }
        }
    }

    // figure out what if there is a color cycling buoy - this should be a buoy of the same position but different colors
    // for each buoy, look at all other buoys, if there are 2 more buoys with diff colors by similar position merge them
    // and flag as MV_UNCOLORED
    for (unsigned i = 0; i < m_rbox_segment_vector.size(); i++) {
        int num_other_colors = 0;
        for (unsigned j = i+1; j < m_rbox_segment_vector.size(); j++) {
            CvPoint center1 = m_rbox_segment_vector[i].center;
            CvPoint center2 = m_rbox_segment_vector[j].center;  
            
            if (abs(center1.x-center2.x) < 20 &&
                abs(center1.y-center2.y) < 20 &&
                !m_rbox_segment_vector[i].color_check(m_rbox_segment_vector[j], 20)
                )
            {
                m_rbox_segment_vector[i].shape_merge(m_rbox_segment_vector[j]);
                m_rbox_segment_vector.erase(m_rbox_segment_vector.begin()+j);;
                num_other_colors++;
            }
        }
        if (num_other_colors >= 2) {
            m_rbox_segment_vector[i].color_int = MV_UNCOLORED;
        }
    } 

    // sort by count
    std::sort (m_rbox_segment_vector.begin(), m_rbox_segment_vector.end(), shape_count_greater_than);

    // debug
    printf ("Buoy: rbox_calc segments:\n");
    for (unsigned i = 0; i < m_rbox_segment_vector.size(); i++) {
        printf ("\tSegment %d (%3d,%3d) h=%2.0f,w=%2.0f clr=%s  count=%d\n", i, 
            m_rbox_segment_vector[i].center.x, m_rbox_segment_vector[i].center.y,
            m_rbox_segment_vector[i].length, m_rbox_segment_vector[i].width, 
            color_int_to_string(m_rbox_segment_vector[i].color_int).c_str(), m_rbox_segment_vector[i].count);
    }

    //cvWaitKey(0);

    // put the best 2 rbox into the result for now
    if (0) {

    }
    else {
        rboxes_returned->clear();
        if (m_rbox_segment_vector.size() > 1 && m_rbox_segment_vector[1].validity > nframes/6)
            rboxes_returned->push_back(m_rbox_segment_vector[1]);
        if (m_rbox_segment_vector.size() > 0 && m_rbox_segment_vector[0].validity > nframes/6)
            rboxes_returned->push_back(m_rbox_segment_vector[0]);
    }
}

void MDA_VISION_MODULE_BUOY::circle_calc (MvCircleVector* circles_returned, int nframes) {

}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_BUOY::frame_calc () {
    
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
    const int ANGLE_LIMIT = 35;

    // go thru each frame and pull all individual segments into a vector
    // set i to point to the element 1 past read_index
    m_rbox_segment_vector.clear();
    int i = read_index + 1;
    if (i >= N_FRAMES_TO_KEEP) i = 0;
    do {
        if (m_frame_data_vector[i].has_data() && m_frame_data_vector[i].rboxes_valid[0]) {
            m_rbox_segment_vector.push_back(m_frame_data_vector[i].m_frame_boxes[0]);
        }
        if (++i >= N_FRAMES_TO_KEEP) i = 0;
    } while (i != read_index);


    // bin the frames
    for (unsigned i = 0; i < m_rbox_segment_vector.size(); i++) {
        for (unsigned j = i+1; j < m_rbox_segment_vector.size(); j++) {
            CvPoint center1 = m_rbox_segment_vector[i].center;
            CvPoint center2 = m_rbox_segment_vector[j].center;  
            
            if (abs(center1.x-center2.x)+abs(center1.y-center2.y) < 50 && 
                abs(m_rbox_segment_vector[i].length-m_rbox_segment_vector[j].length) < 30 &&
                abs(m_rbox_segment_vector[i].width-m_rbox_segment_vector[j].width) < 20
            )
            {
                m_rbox_segment_vector[i].shape_merge(m_rbox_segment_vector[j]);
                m_rbox_segment_vector.erase(m_rbox_segment_vector.begin()+j);
            }
        }
    }

    // sort by count
    std::sort (m_rbox_segment_vector.begin(), m_rbox_segment_vector.end(), shape_count_greater_than);

    // debug
    /*for (unsigned i = 0; i<=1 && i<m_rbox_segment_vector.size(); i++) {
        printf ("\tSegment %d (%3d,%3d) height=%3.0f, width=%3.0f   count=%d\n", i, m_rbox_segment_vector[i].center.x, m_rbox_segment_vector[i].center.y,
            m_rbox_segment_vector[i].length, m_rbox_segment_vector[i].width, m_rbox_segment_vector[i].count);
    }*/
    
    return FULL_DETECT;

if(0){
    if (m_rbox_segment_vector.size() == 0 || m_rbox_segment_vector[0].count < 2) { // not enough good segments, return no target
        printf ("Buoy: No Target\n");
        return NO_TARGET;
    }
    else if (m_rbox_segment_vector.size() > 1 && m_rbox_segment_vector[1].count < 2) { // first segment is good enough, use that only
        printf ("Buoy: ONE_SEGMENT\n");
        int buoy_pixel_height = m_rbox_segment_vector[0].length;
        
        // check segment is vertical
        if (abs(m_rbox_segment_vector[0].angle) > ANGLE_LIMIT) {
            DEBUG_PRINT("One Segment: angle outside limit\n");
            return NO_TARGET;
        }

        m_pixel_x = m_rbox_segment_vector[0].center.x;
        m_pixel_y = m_rbox_segment_vector[0].center.x;
        m_range = (RBOX_REAL_LENGTH * gray_img->height) / (buoy_pixel_height * TAN_FOV_Y);

        retval = ONE_SEGMENT;
    }
    else if (m_rbox_segment_vector.size() > 1) { // full detect, return both segments
        int buoy_pixel_height = (m_rbox_segment_vector[0].length + m_rbox_segment_vector[1].length) / 2;
        //const int BUOY_HEIGHT_TO_WIDTH_RATIO = RBOX_REAL_LENGTH / RBOX_REAL_DIAMETER;

        // check segment is vertical
        if (abs(m_rbox_segment_vector[0].angle) > ANGLE_LIMIT || abs(m_rbox_segment_vector[1].angle) > ANGLE_LIMIT) {
            DEBUG_PRINT("Full Detect: angle outside limit\n");
            return NO_TARGET;
        }
        // check length similarity
        if (m_rbox_segment_vector[0].length > 1.3*m_rbox_segment_vector[1].length || 1.3*m_rbox_segment_vector[0].length < m_rbox_segment_vector[1].length) {
            DEBUG_PRINT("Full Detect: similarity check failed\n");
            return NO_TARGET;
        }
        /*if (gate_width_to_height_ratio > 1.3*BUOY_HEIGHT_TO_WIDTH_RATIO || 1.3*gate_width_to_height_ratio < BUOY_HEIGHT_TO_WIDTH_RATIO) {
            DEBUG_PRINT("Full Detect: gate width to height check failed\n");
            return NO_TARGET  ; 
        }*/

        m_pixel_x = (m_rbox_segment_vector[0].center.x + m_rbox_segment_vector[1].center.x) / 2;
        m_pixel_y = (m_rbox_segment_vector[0].center.y + m_rbox_segment_vector[1].center.y) / 2;
        m_range = (RBOX_REAL_LENGTH * gray_img->height) / (buoy_pixel_height * TAN_FOV_Y);

        printf ("Buoy: FULL_DETECT\n");
        retval = FULL_DETECT;
    }

#ifdef M_DEBUG
    for (unsigned i = 0; i<=1 && i<m_rbox_segment_vector.size(); i++)
        m_rbox_segment_vector[i].drawOntoImage(gray_img);
      window2.showImage(gray_img);
#endif

    m_pixel_x -= gray_img->width/2;
    m_pixel_y -= gray_img->height/2;
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / gray_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / gray_img->height);
    printf ("Buoy (%3d,%3d) (%5.2f,%5.2f) range=%d\n", m_pixel_x, m_pixel_y, m_angular_x, m_angular_y, m_range);
    return retval;
}
}

bool MDA_VISION_MODULE_BUOY::rbox_stable (int rbox_index, float threshold) {
    assert (rbox_index >= 0 && rbox_index <= 1);
    return true;
}

bool MDA_VISION_MODULE_BUOY::circle_stable (float threshold) {
    return true;
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
