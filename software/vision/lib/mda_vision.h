#ifndef __MDA_VISION__MDA_VISION__
#define __MDA_VISION__MDA_VISION__

#include "mgui.h"
#include "mv.h"
#include "mvLines.h"

#define MDA_VISION_MODULE_TEST_SETTINGS "./settings/vision_module_test_settings.csv"

// this class is to test vision/sim/control interfaces. write whatever you want
class MDA_VISION_MODULE_TEST {
    mvWindow* _window;
    mvHSVFilter* _HSVFilter;
    mvHoughLines* _HoughLines;
    mvLines* _lines;
    
    IplImage* _resized_img;
    IplImage* _filtered_img;
    
public:
    MDA_VISION_MODULE_TEST ();
    ~MDA_VISION_MODULE_TEST ();
    
    void filter (const IplImage* src, IplImage* &dst);
};

#endif
