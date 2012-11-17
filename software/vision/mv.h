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

/* These allow you access to static scratch images */
IplImage* mvGetScratchImage();
void mvReleaseScratchImage();
IplImage* mvGetScratchImage2();
void mvReleaseScratchImage2();
IplImage* mvGetScratchImage_Color();
void mvReleaseScratchImage_Color();

/*BRG2HSV -- faster implementation to convert BRG images into HSV format */

void mvBRG2HSV(const IplImage* src, IplImage* dst);

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

/// split the image into any of its 2 planes in place
void mvSplitImage (const IplImage* src, IplImage** plane1=NULL, IplImage** plane2=NULL);

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

    PROFILE_BIN bin_morph;
    PROFILE_BIN bin_gradient;

	void mvBinaryMorphologyMain (MV_MORPHOLOGY_TYPE morphology_type, const IplImage* src, IplImage* dst);

	public:
	mvBinaryMorphology (int Kernel_Width, int Kernel_Height, MV_KERNEL_SHAPE Shape);
	~mvBinaryMorphology ();

	void dilate (const IplImage* src, IplImage* dst) {
          bin_morph.start();
		mvBinaryMorphologyMain (MV_DILATE, src, dst);
          bin_morph.stop();
	}
	void erode (const IplImage* src, IplImage* dst) {
          bin_morph.start();
		mvBinaryMorphologyMain (MV_ERODE, src, dst);
          bin_morph.stop();
	}
	void close (const IplImage* src, IplImage* dst) {
          bin_morph.start();
		mvBinaryMorphologyMain (MV_CLOSE, src, dst);
          bin_morph.stop();
	}
	void open (const IplImage* src, IplImage* dst) {
          bin_morph.start();
		mvBinaryMorphologyMain (MV_OPEN, src, dst);
          bin_morph.stop();
	}
	void gradient (const IplImage* src, IplImage* dst) {
          bin_gradient.start();
		mvBinaryMorphologyMain (MV_GRADIENT, src, dst);
          bin_gradient.stop();
	}
};

///#################################################################################
///#################################################################################
///#################################################################################

inline bool hue_in_range (unsigned char hue, int HMIN, int HMAX) { // helper function for the filter
    if (HMAX >= HMIN) 
        return (hue >= HMIN && hue <= HMAX);
    else
        return ((hue <= HMIN && hue <= HMAX) || (hue >= HMIN && hue >= HMAX)); 
}

/** HSV color limit filter */
// Currently support for changing the HSV values on the fly are limited
class mvHSVFilter {
    static const unsigned UNCHANGED = 9999;  // value passed in to keep a value unchanged  
    
    unsigned IMG_WIDTH, IMG_HEIGHT;
    int HMIN,HMAX;
    unsigned SMIN,SMAX, VMIN, VMAX;
    
    IplImage* HSVImg;

    PROFILE_BIN bin_WorkingLoop;
    PROFILE_BIN bin_CvtColor;
    
    public:
    mvHSVFilter (const char* settings_file);
    ~mvHSVFilter ();
    void filter (const IplImage* img, IplImage* result);
    void setHSV (int hmin=UNCHANGED, int hmax=UNCHANGED, 
                 unsigned smin=UNCHANGED, unsigned smax=UNCHANGED, 
                 unsigned vmin=UNCHANGED, unsigned vmax=UNCHANGED
                );    
};


/** mvAdaptiveFilter
 *  This class is a set of AdaptiveBox's to represent the water, the target, and 
 *  possibly any other objects we might see.
 */
class mvAdaptiveFilter3 {
    static const int nbins_hue = 30;
    static const int nbins_sat = 25;
    static const int hue_range_min = 0;
    static const int hue_range_max = 179;
    static const int sat_range_min = 6;
    static const int sat_range_max = 255;
    static const int MAX_BINS_MARKED = 9;
    static const int HISTOGRAM_NORM_FACTOR = 100000;
    static const bool DISPLAY_HIST = true;

    int hue_min;
    int hue_max;
    int sat_min;
    int sat_max;
    int val_min;
    int val_max;

    IplImage* src_HSV;
    IplImage* hue_img;
    IplImage* sat_img;
    IplImage* hist_img;

    CvHistogram* hist; 

    PROFILE_BIN bin_adaptive;
    PROFILE_BIN bin_CvtColor;
    mvWindow* win;

    public:
    mvAdaptiveFilter3 (const char* settings_file);
    ~mvAdaptiveFilter3 ();
    void filter (const IplImage* src, IplImage* dst);
    void show_histogram ();
};


#endif
