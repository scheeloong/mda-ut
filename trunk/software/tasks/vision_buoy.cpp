#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

const char MDA_VISION_MODULE_BUOY::MDA_VISION_BUOY_SETTINGS[] = "vision_buoy_settings.csv";

/// #########################################################################
/// MODULE_BUOY methods
/// #########################################################################
MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY () :
    _window (mvWindow("Buoy Vision Module")),
    _HSVFilter (mvHSVFilter(MDA_VISION_BUOY_SETTINGS)),
    _Morphology5 (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
    _Morphology3 (mvBinaryMorphology(3, 3, MV_KERN_RECT)),
    _AdvancedCircles(MDA_VISION_BUOY_SETTINGS)
{
    _filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_BUOY:: ~MDA_VISION_MODULE_BUOY () {
    mvReleaseScratchImage();
}

void MDA_VISION_MODULE_BUOY:: primary_filter (const IplImage* src) {
    _HSVFilter.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    
    //_Morphology5.open(_filtered_img, _filtered_img);
    _Morphology5.gradient(_filtered_img, _filtered_img);

    _AdvancedCircles.findCircles (_filtered_img);

    _AdvancedCircles.drawOntoImage (_filtered_img);

    _window.showImage (_filtered_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_BUOY:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;

    // We assume most of the time we have 1 circle. If we have >1 circle we take the first
    // circle in the list (which is the strongest) and return UNKNOWN_TARGET

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

    m_pixel_x = _AdvancedCircles[0].x;
    m_pixel_y = _AdvancedCircles[0].y;
    unsigned rad = _AdvancedCircles[0].rad;

    assert (rad != 0);

    m_range = (BUOY_REAL_DIAMTER * _filtered_img->width) / (rad * TAN_FOV_X);
    m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / _filtered_img->width);
    m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / _filtered_img->height);

    DEBUG_PRINT ("Buoy: (%d,%d) (%5.2f,%5.2f). Range = %d\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y, m_range); 
    
    retval = (nCircles == 1) ? FULL_DETECT : UNKNOWN_TARGET;
    return retval;
}
