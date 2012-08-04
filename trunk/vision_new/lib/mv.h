/** mv - MDA vision library
 *  includes objects for filters and detectors
 *  Author - Ritchie Zhao
 */
#ifndef __MDA_VISION_MV__
#define __MDA_VISION_MV__

#include <cv.h>
#include "mgui.h"

/** mvCreateImage is a wrapper for cvCreateImage which drops the need for specifying
 *  depth and nChannels. The default mvCreateImage always returns a greyscale image 
 *  while mvCreateImage_Color always returns a color image;
 */
inline IplImage* mvCreateImage () { // common size image
    unsigned width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    return cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 1);
}
inline IplImage* mvCreateImage (CvSize size) { // specified size
    return cvCreateImage (size, IPL_DEPTH_8U, 1);
}
inline IplImage* mvCreateImage (unsigned width, unsigned height) { // specified size
    return cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 1);
}
inline IplImage* mvCreateImage (IplImage *img) { // size and origin
    IplImage* temp = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1);
    temp->origin = img->origin;
    return temp;
}

inline IplImage* mvCreateImage_Color () { // common size image
    unsigned width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    return cvCreateImage (cvSize(width,height), IPL_DEPTH_8U, 3);
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

/** Gradient Filter object */
// Must provide it with 2 images, both of COMMON size 
class mvGradient {
    IplImage* scratch;
    IplConvKernel* kernel; 
    
    unsigned IMG_WIDTH, IMG_HEIGHT;
    unsigned KERNEL_WIDTH, KERNEL_HEIGHT;
    
    public:
    mvGradient (const char* settings_file);
    ~mvGradient ();
    
    void filter (const IplImage* img, IplImage* result) {
        assert (img != NULL);
        assert (img->nChannels == 1);
        assert (result != NULL);
        assert (result->nChannels == 1);
            
        cvMorphologyEx (img, result, scratch, kernel, CV_MOP_GRADIENT);
        //cvMorphologyEx (img, result, scratch, kernel, CV_MOP_GRADIENT, iterations);
    };
};

/** Canny edge detector */
// pretty trivial
class mvCanny {
    unsigned KERNEL_SIZE;
    float LOW_THRESHOLD, HIGH_THRESHOLD;
    
    public:
    mvCanny (const char* settings_file);
    
    void filter (const IplImage* img, IplImage* result) {
        assert (img != NULL);
        assert (img->nChannels == 1);
        assert (result != NULL);
        assert (result->nChannels == 1);
        
        cvCanny (img, result, LOW_THRESHOLD, HIGH_THRESHOLD, KERNEL_SIZE);
    }
};

/** HSV color limit filter */
// Currently support for changing the HSV values on the fly are limited
class mvHSVFilter {
    #define _SAME_ 999999
    IplImage* HSVImg;
    
    unsigned IMG_WIDTH, IMG_HEIGHT;
    int HMIN,HMAX;
    unsigned SMIN,SMAX, VMIN, VMAX;
    
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
