#include "mda_vision.h"

#define M_DEBUG
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

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

    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
    //filtered_img = mvGetScratchImage (); // common size
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
    //window.showImage (gray_img);

    int seg = 1;
    COLOR_TRIPLE color;
    COLOR_TRIPLE color_template (TARGET_BLUE,TARGET_GREEN,TARGET_RED,0);

    CvPoint best_centroid = cvPoint(MV_UNDEFINED_VALUE, MV_UNDEFINED_VALUE);
    float best_radius = MV_UNDEFINED_VALUE;
    double best_shape_diff=1000000, best_color_diff=1000000;
    double best_diff = 1000000;
    
    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        DEBUG_PRINT ("\nSegment %d\n", seg);
        DEBUG_PRINT ("\tColor (%3d,%3d,%3d)\n", color.m1, color.m2, color.m3);

        // calculate color diff
        double color_diff = static_cast<double>(color.diff(color_template)) / COLOR_DIVISION_FACTOR;

        // calculate shape diff
        CvPoint centroid;
        float radius;
        double shape_diff = contour_filter.match_circle(gray_img_2, centroid, radius);
        if (shape_diff < 0) // error from the shape matching
            continue;

        double diff = color_diff + shape_diff;
        DEBUG_PRINT ("\tColor_Diff=%6.4f  Shape_Diff=%6.4f\n\tFinal_Diff=%6.4f\n", color_diff, shape_diff, diff);
        window2.showImage (gray_img_2);
        //cvWaitKey(300);

        if (seg == 1 || diff < best_diff) {
            best_diff = diff;  best_shape_diff = shape_diff;  best_color_diff = color_diff;
            best_centroid = centroid;
            best_radius = radius;
            cvCopy (gray_img_2, gray_img);
        }
        seg++;
    }

    double confidence_level = DIFF_THRESHOLD / best_diff;
    DEBUG_PRINT ("Confidence Level = %5.3lf (%4.2lf shape diff, %4.2lf color diff)\n", 
        confidence_level, best_shape_diff, best_color_diff);
    if (confidence_level < 1.0) {
        DEBUG_PRINT ("Confidence level does not meet threshold\n");
        return;
    }

    window.showImage (gray_img);
    
    m_pixel_x = best_centroid.x;
    m_pixel_y = best_centroid.y;
    m_range = (BUOY_REAL_DIAMTER * gray_img->width) / (2*best_radius * TAN_FOV_X);
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
        m_range = (BUOY_REAL_DIAMTER * filtered_img->width) / (2*rad * TAN_FOV_X);
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
