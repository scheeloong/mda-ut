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
    IplImage* HSVImg;
    
    unsigned IMG_WIDTH, IMG_HEIGHT;
    int HMIN,HMAX;
    unsigned SMIN,SMAX, VMIN, VMAX;
    
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
class mvAdaptiveFilter {
    static const unsigned nBoxes = 100;
    struct BGR_box {
        int B, G, R;
        int count;
    };

    int guess[3];
    IplImage* src_HSV;
    BGR_box box_array[nBoxes];
    PROFILE_BIN bin_adaptive;

    public:
    mvAdaptiveFilter (const char* Settings_File);
    ~mvAdaptiveFilter ();
    void filter (const IplImage* src, IplImage* dst);
};

class mvAdaptiveFilter2 {
    struct BRG_box2 {
        int c1,c2,c3; // color center value
        int d1,d2,d3; // deltas
        unsigned count;
    };

    int min1, max1;
    int min2, max2;
    int min3, max3;
    int dmin1, dmax1; // double 
    int dmin2, dmax2;
    int dmin3, dmax3;

    void calc_limits (BRG_box2* box) {
        min1 = box->c1-box->d1; 
        max1 = box->c1+box->d1;
        min2 = box->c2-box->d2; 
        max2 = box->c2+box->d2;
        min3 = box->c3-box->d3; 
        max3 = box->c3+box->d3;
        dmin1 = box->c1-2*box->d1; 
        dmax1 = box->c1+2*box->d1;
        dmin2 = box->c2-2*box->d2; 
        dmax2 = box->c2+2*box->d2;
        dmin3 = box->c3-2*box->d3; 
        dmax3 = box->c3+2*box->d3;
        if (min1 < 0) min1 += 180;
        if (max1 > 179) max1 -= 180;
        if (dmin1 < 0) dmin1 += 180;
        if (dmax1 > 179) dmax1 -= 180;
    }
    void copy_box (BRG_box2* box1, BRG_box2* box2){
        box2->c1 = box1->c1; box2->c2 = box1->c2; box2->c3 = box1->c3;
        box2->d1 = box1->d1; box2->d2 = box1->d2; box2->d3 = box1->d3;
        box2->count = box1->count;
    }
    bool in_box (int C1, int C2, int C3) {
        return ((C2 >= min2) && (C2 <= max2) && (C3 >= min3) && (C3 <= max3) && hue_in_range(C1, min1, min2));
    }
    bool in_2box (int C1, int C2, int C3) {
        return ((C2 >= dmin2) && (C2 <= dmax2) && (C3 >= dmin3) && (C3 <= dmax3) && hue_in_range(C1, dmin1, dmin2));
    }
    void accumulate_box (BRG_box2* box, int C1, int C2, int C3) {
        box->c1 = ((box->c1)*box->count + C1) / (box->count+1);
        box->c2 = ((box->c2)*box->count + C2) / (box->count+1);
        box->c3 = ((box->c3)*box->count + C3) / (box->count+1);
        box->count++;
    }

    BRG_box2 target;
    BRG_box2 bg;
    BRG_box2 next_target;
    BRG_box2 next_bg;

    IplImage* src_HSV;

    PROFILE_BIN bin_adaptive;

    public:
    mvAdaptiveFilter2 (const char* Settings_File);
    ~mvAdaptiveFilter2 ();
    void filter (const IplImage* src, IplImage* dst);
};

class mvAdaptiveFilter3 {
    static const int nbins1 = 18;

    int hue_target;
    int hue_target_delta;
    int sat_target;
    int sat_target_delta;
    int val_target;
    int val_target_delta;

    IplImage* src_HSV;
    IplImage* hue_img;

    CvHistogram* hist1; 

    PROFILE_BIN bin_adaptive;

    public:
    mvAdaptiveFilter3 (const char* Settings_File);
    ~mvAdaptiveFilter3 ();
    void filter (const IplImage* src, IplImage* dst);
};


#endif
