/** mv - MDA vision library
 *  includes objects for filters and detectors
 *  Author - Ritchie Zhao
 */


#ifndef __MDA_VISION_MV__
#define __MDA_VISION_MV__

#include <cv.h>
#include "mgui.h"

/* Gradient Filter */
class mvGradient {
    IplImage* scratch;
    IplConvKernel* kernel; 
    mvWindow* window;
    char window_name[WINDOW_NAME_LEN];
    
    unsigned IMG_WIDTH, IMG_HEIGHT;
    unsigned KERNEL_WIDTH, KERNEL_HEIGHT;
    int _DISPLAY_, _QUIET_;
    
    public:
    mvGradient (char settings_file[]);
    ~mvGradient ();
    void filter (const IplImage* img, IplImage* result);
};

#endif
