/** mv - MDA vision library
 *  includes objects for filters and detectors
 *  Author - Ritchie Zhao
 */


#ifndef __MDA_VISION_MV__
#define __MDA_VISION_MV__

#include <cv.h>
#include "mgui.h"

/** mvCreateImage is a wrapper for cvCreateImage which drops the need for specifying
 *  depth and nChannels 
 */
inline IplImage* mvCreateImage (CvSize size) {
    return cvCreateImage (size, IPL_DEPTH_8U, 1);
}
inline IplImage* mvCreateImage (unsigned width, unsigned height) {
    return cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 1);
}
inline IplImage* mvCreateImage (IplImage *img) {
    IplImage* temp = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
    temp->origin = img->origin;
    return temp;
}

inline IplImage* mvCreateImage_Color (CvSize size) {
    return cvCreateImage (size, IPL_DEPTH_8U, 3);
}
inline IplImage* mvCreateImage_Color (unsigned width, unsigned height) {
    return cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 3);
}
inline IplImage* mvCreateImage_Color (IplImage *img) {
    IplImage* temp = cvCreateImage (cvGetSize(img), img->depth, 3);
    temp->origin = img->origin;
    return temp;
}

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
    mvGradient (const char* settings_file);
    ~mvGradient ();
    void filter (const IplImage* img, IplImage* result);
};

/* HSV color limit filter */
class mvHSVFilter {
    #define _SAME_ 999999
    IplImage* HSVImg;
    mvWindow* window;
    char window_name[WINDOW_NAME_LEN];
    
    unsigned IMG_WIDTH, IMG_HEIGHT;
    int HMIN,HMAX;
    unsigned SMIN,SMAX, VMIN, VMAX;
    int _DISPLAY_;
    
    private:
    int hueInRange (unsigned char hue);
    
    public:
    mvHSVFilter (const char* settings_file);
    ~mvHSVFilter ();
    void filter (const IplImage* img, IplImage* result);
    void setHSV (int hmin=_SAME_, int hmax=_SAME_, 
                 unsigned smin=_SAME_, unsigned smax=_SAME_, 
                 unsigned vmin=_SAME_, unsigned vmax=_SAME_
                );    
};

#endif
