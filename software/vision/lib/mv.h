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

/** Binary Filters */
// These are fast filters designed specifically for binary images with very
// few bright pixels
enum MV_KERNEL_SHAPE {MV_KERN_RECT, MV_KERN_ELLIPSE};
enum MV_MORPHOLOGY_TYPE {MV_DILATE, MV_ERODE, MV_OPEN, MV_CLOSE, MV_GRADIENT};

class mvBinaryMorphology {
	IplImage* temp;
	int kernel_width, kernel_height;
	unsigned kernel_area;
	int* kernel_point_array;

	void mvBinaryMorphologyMain (MV_MORPHOLOGY_TYPE morphology_type, const IplImage* src, IplImage* dst);

	public:
	mvBinaryMorphology (int Kernel_Width, int Kernel_Height, MV_KERNEL_SHAPE Shape);
	~mvBinaryMorphology ();

	void dilate (const IplImage* src, IplImage* dst) {
		mvBinaryMorphologyMain (MV_DILATE, src, dst);
	}
	void erode (const IplImage* src, IplImage* dst) {
		mvBinaryMorphologyMain (MV_ERODE, src, dst);
	}
	void close (const IplImage* src, IplImage* dst) {
		mvBinaryMorphologyMain (MV_CLOSE, src, dst);
	}
	void open (const IplImage* src, IplImage* dst) {
		mvBinaryMorphologyMain (MV_OPEN, src, dst);
	}
	void gradient (const IplImage* src, IplImage* dst) {
		mvBinaryMorphologyMain (MV_GRADIENT, src, dst);
	}
};

/*
void mvBinaryMorphology (
        MV_MORPHOLOGY_TYPE morphology_type,
        const IplImage* src, IplImage* dst, IplImage* temp=NULL, 
        int kernel_width=3, int kernel_height=3,
        MV_KERNEL_SHAPE shape=MV_KERN_RECT
        );
*/
/** HSV color limit filter */
// Currently support for changing the HSV values on the fly are limited
class mvHSVFilter {
    static const unsigned UNCHANGED = 9999;  // value passed in to keep a value unchanged  
    IplImage* HSVImg;
    
    unsigned IMG_WIDTH, IMG_HEIGHT;
    int HMIN,HMAX;
    unsigned SMIN,SMAX, VMIN, VMAX;
    
    PROFILE_BIN bin_WorkingLoop;
    PROFILE_BIN bin_CvtColor;

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
