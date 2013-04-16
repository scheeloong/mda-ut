#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

const char MDA_VISION_MODULE_BUOY::MDA_VISION_BUOY_SETTINGS[] = "vision_buoy_settings_new.csv";

/// #########################################################################
/// MODULE_BUOY methods
/// #########################################################################
MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY () :
    _window (mvWindow("Buoy Vision Module")),
    _MeanShift (mvMeanShift(MDA_VISION_BUOY_SETTINGS)),
    _Morphology5 (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
    _Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT)),
    _AdvancedCircles(MDA_VISION_BUOY_SETTINGS),
    _Rect ("Rect_settings.csv")
{
    _filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY (const char* settings_file) :
    _window (mvWindow("Buoy Vision Module")),
    _MeanShift (mvMeanShift(settings_file)),
    _Morphology5 (mvBinaryMorphology(9, 9, MV_KERN_RECT)),
    _Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT)),
    _AdvancedCircles(MDA_VISION_BUOY_SETTINGS),
    _Rect ("Rect_settings.csv")
{
    _filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_BUOY:: ~MDA_VISION_MODULE_BUOY () {
    mvReleaseScratchImage();
}

void MDA_VISION_MODULE_BUOY:: primary_filter (IplImage* src) {
    _MeanShift.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
  
    //_Morphology5.open(_filtered_img, _filtered_img);
    _Rect.find (_filtered_img);
    _Rect.removeFromImage(_filtered_img);

    _Morphology5.gradient(_filtered_img, _filtered_img);

    _AdvancedCircles.find (_filtered_img);

    _Rect.drawOntoImage(_filtered_img);
    _AdvancedCircles.drawOntoImage (_filtered_img);

    _window.showImage (_filtered_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_BUOY:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;

    // We assume most of the time we have 1 circle. If we have >1 circle we take the first
    // circle in the list (which is the strongest) and return UNKNOWN_TARGET
    // We cant trust range if circle is too small, so in that case also return UNKNOWN_TARGET
    // and range is not valid if UNKNOWN_TARGET is returned

    unsigned nCircles = _AdvancedCircles.ncircles();

    if (nCircles == 0) {
        printf ("Circles: No circles found =(\n");
        return NO_TARGET;
    }

    /*
    printf ("ncircles = %d\n", _AdvancedCircles.ncircles());
    for (int i = 0; i < _AdvancedCircles.ncircles(); i++) {
        printf ("Circle #%d: (%d,%d, %f)\n", i+1, _AdvancedCircles[i].x, _AdvancedCircles[i].y, _AdvancedCircles[i].rad);
    }*/

    m_pixel_x = _AdvancedCircles[0].x - _filtered_img->width/2;
    m_pixel_y = _AdvancedCircles[0].y - _filtered_img->height/2;
    unsigned rad = _AdvancedCircles[0].rad;

    assert (rad != 0);
    
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / _filtered_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / _filtered_img->height);

    if (nCircles == 1 && rad > MIN_PIXEL_RADIUS_FACTOR*_filtered_img->height) {
        m_range = (BUOY_REAL_DIAMTER * _filtered_img->width) / (2*rad * TAN_FOV_X);
        DEBUG_PRINT ("Buoy: (%d,%d) (%5.2f,%5.2f). Color = %s. Range = %d\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y, color_int_to_string(_AdvancedCircles[0].color).c_str(), m_range);

        retval = FULL_DETECT;
    }
    else {
        DEBUG_PRINT ("Buoy: (%d,%d) (%5.2f,%5.2f). Color = %s. Range = Unknown\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y, color_int_to_string(_AdvancedCircles[0].color).c_str());

        retval = UNKNOWN_TARGET;
    }

    return retval;
}
