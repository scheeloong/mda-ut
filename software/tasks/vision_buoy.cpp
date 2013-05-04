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
    Morphology5 (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
    Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT)),
    AdvancedColorFilter (mvAdvancedColorFilter(MDA_VISION_BUOY_SETTINGS)),
    AdvancedCircles(MDA_VISION_BUOY_SETTINGS),
    Rect ("Rect_settings.csv")
{
    filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY (const char* settings_file) :
    window (mvWindow("Buoy Vision Module")),
    Morphology5 (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
    Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT)),
    AdvancedColorFilter (mvAdvancedColorFilter(settings_file)),
    AdvancedCircles(MDA_VISION_BUOY_SETTINGS),
    Rect ("Rect_settings.csv")
{
    filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_BUOY:: ~MDA_VISION_MODULE_BUOY () {
    mvReleaseScratchImage();
}

void MDA_VISION_MODULE_BUOY:: primary_filter (IplImage* src) {
    AdvancedColorFilter.filter (src, filtered_img);
    filtered_img->origin = src->origin;
  
    Morphology5.close(filtered_img, filtered_img);
    Rect.find (filtered_img);
    Rect.removeFromImage(filtered_img);

    Morphology5.gradient(filtered_img, filtered_img);

    AdvancedCircles.find (filtered_img);

    Rect.drawOntoImage(filtered_img);
    AdvancedCircles.drawOntoImage (filtered_img);

    window.showImage (filtered_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_BUOY:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;

    // We assume most of the time we have 1 circle. If we have >1 circle we take the first
    // circle in the list (which is the strongest) and return UNKNOWN_TARGET
    // We cant trust range if circle is too small, so in that case also return UNKNOWN_TARGET
    // and range is not valid if UNKNOWN_TARGET is returned

    unsigned nCircles = AdvancedCircles.ncircles();

    if (nCircles == 0) {
        printf ("Circles: No circles found =(\n");
        return NO_TARGET;
    }

    /*
    printf ("ncircles = %d\n", AdvancedCircles.ncircles());
    for (int i = 0; i < AdvancedCircles.ncircles(); i++) {
        printf ("Circle #%d: (%d,%d, %f)\n", i+1, AdvancedCircles[i].x, AdvancedCircles[i].y, AdvancedCircles[i].rad);
    }*/

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

    return retval;
}
