/** mv - MDA vision library
 *  includes objects for filters and detectors
 *  Author - Ritchie Zhao
 */
#ifndef __MDA_VISION_MV__
#define __MDA_VISION_MV__

#include <cv.h>
#include "mgui.h"

typedef unsigned char uchar;

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

inline void mvGaussian (const IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h) {
    cvSmooth (src, dst, CV_GAUSSIAN, kern_w, kern_h);
}
inline void mvDilate (const IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);
    cvDilate (src, dst, kernel, iterations);
    cvReleaseStructuringElement (&kernel);
}
inline void mvErode (const IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);
    cvErode (src, dst, kernel, iterations);
    cvReleaseStructuringElement (&kernel);
}
inline void mvOpen (const IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);
    cvMorphologyEx (src, dst, NULL, kernel, CV_MOP_OPEN, iterations);
    cvReleaseStructuringElement (&kernel);
}
inline void mvClose (const IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);
    cvMorphologyEx (src, dst, NULL, kernel, CV_MOP_CLOSE, iterations);
    cvReleaseStructuringElement (&kernel);
}
inline void mvGradient (const IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplImage* temp = cvCreateImage (cvGetSize(src), IPL_DEPTH_8U, 1);
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);

    cvMorphologyEx (src, dst, temp, kernel, CV_MOP_GRADIENT, iterations);
    
    cvReleaseImage (&temp);
    cvReleaseStructuringElement (&kernel);
}

/** HSV color limit filter */
// Currently support for changing the HSV values on the fly are limited
class mvHSVFilter {
    static const unsigned UNCHANGED = 9999;  // value passed in to keep a value unchanged  
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
    void setHSV (int hmin=UNCHANGED, int hmax=UNCHANGED, 
                 unsigned smin=UNCHANGED, unsigned smax=UNCHANGED, 
                 unsigned vmin=UNCHANGED, unsigned vmax=UNCHANGED
                );    
};


/** mvAdaptiveBox
 *  This class represents a learning filter. Values between _min and _max are
 *  accepted. However, values between _min-_min_bound and _max+_max_bound
 *  will "stretch" the bounds (_min,_max). Over time the box will grow adapt
 *  to new data
 */
class mvAdaptiveBox {
    uchar min, max;         // min and max bounds
    uchar min_50, max_50;   // min and max for 50% of the box
    uchar min_bound, max_bound;  // threshold for growing the box

    uchar orig_min, orig_max; // min and max when initialized. Stored so box doesnt move too far away.
    static const uchar adjust_unit = 2;      // how much box bounds grow each time
    static const uchar adjust_limit = 20;     // how much box bounds are allowed to grow

    uchar box_count;         // how many falls within the box
    uchar box_50_count;      // how many falls within inner 50% of the box
    uchar box_min_bound_count, box_max_bound_count;  // how many falls inside the flaps

    public:
    mvAdaptiveBox (uchar Min, uchar Max, uchar Min_Bound, uchar Max_Bound);
    int accumulate (uchar Data);
    void adjust_box ();
};

class mvAdaptiveFilter {
    mvAdaptiveBox* box_bg[3];       // one for each channel of background
    mvAdaptiveBox* box_target[3];

    public:
    mvAdaptiveFilter (const char* Settings_File);
};

#endif
