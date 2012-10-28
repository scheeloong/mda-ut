#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

/// ########################################################################
/// MODULE_FRAME methods
/// ########################################################################
MDA_VISION_MODULE_FRAME:: MDA_VISION_MODULE_FRAME () :
    _window (mvWindow("Frame Vision Module")),
    _HSVFilter (mvHSVFilter(MDA_VISION_MODULE_TEST_SETTINGS)),
    _HoughLines (mvHoughLines(MDA_VISION_MODULE_TEST_SETTINGS)),
    _lines (mvLines())
{
    _filtered_img = mvCreateImage (); // common size
}

MDA_VISION_MODULE_FRAME:: ~MDA_VISION_MODULE_FRAME () {
    cvReleaseImage (&_filtered_img);
}

void MDA_VISION_MODULE_FRAME:: primary_filter (const IplImage* src) {
    _HSVFilter.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines.clearData ();
    _KMeans.clearData ();
    
    _HoughLines.findLines (_filtered_img, &_lines);
    _KMeans.cluster_auto (1, 3, &_lines, 1);

    _lines.drawOntoImage (_filtered_img);
    _KMeans.drawOntoImage (_filtered_img);

    _window.showImage (_filtered_img);
}

int MDA_VISION_MODULE_FRAME:: calc_vci (VCI* interface) {
    return 0;
}
