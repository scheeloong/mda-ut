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
    _AdvancedCircles("AdvancedCircles_settings.csv")
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
    printf ("ncircles = %d\n", _AdvancedCircles.ncircles());
    for (int i = 0; i < _AdvancedCircles.ncircles(); i++) {
        printf ("Circle #%d: (%d,%d, %f)\n", i+1, _AdvancedCircles[i].x, _AdvancedCircles[i].y, _AdvancedCircles[i].rad);
    }

    _window.showImage (_filtered_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_BUOY:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
    
    retval = NO_TARGET;
    return retval;
}
