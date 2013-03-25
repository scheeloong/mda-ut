/** mv - MDA vision library
 *  includes objects for filters and detectors
 *  Author - Ritchie Zhao
 */
#ifndef __MDA_VISION_MV__
#define __MDA_VISION_MV__

#include <cv.h>
#include "mgui.h"

typedef unsigned char uchar;
const int MV_UNDEFINED_VALUE = -8888888;

const int MV_NUMBER_OF_COLORS = 4;
const int MV_RED = 50;
const int MV_YELLOW = 100;
const int MV_GREEN = 150;
const int MV_BLUE = 200;
const int MV_COLOR_VECTOR[4] = { MV_RED, MV_YELLOW, MV_GREEN, MV_BLUE };
const int MV_UNCOLORED = 255;

inline int color_str_to_int (std::string str) {
    if (str == "RED")
        return MV_RED;
    else if (str == "YELLOW")
        return MV_YELLOW;
    else if (str == "GREEN")
        return MV_GREEN;
    else if (str == "BLUE")
        return MV_BLUE;
    return MV_UNCOLORED;
}
inline std::string color_int_to_string (int color) {
    switch (color) {
        case MV_RED: return std::string("RED");
        case MV_YELLOW: return std::string("YELLOW");
        case MV_GREEN: return std::string("GREEN");
        case MV_BLUE: return std::string("BLUE");
        default:;
    }
    return std::string("UNKNOWN");
}

/** mvHuMoments calculates the Hu moments for a bitmap image
 *  It is a wrapper for cvMoments and cvHuMoments
 */
void mvHuMoments(IplImage *src, double *hus);

/** mvCreateImage is a wrapper for cvCreateImage which drops the need for specifying
 *  depth and nChannels. The default mvCreateImage always returns a greyscale image 
 *  while mvCreateImage_Color always returns a color image;
 */
inline IplImage* mvCreateImageHeader () { // common size image
    unsigned width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    return cvCreateImageHeader (cvSize(width,height), IPL_DEPTH_8U, 1);
}
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
IplImage* mvGetScratchImage3();
void mvReleaseScratchImage3();
IplImage* mvGetScratchImage_Color();
void mvReleaseScratchImage_Color();

/*BRG2HSV -- NOTfaster implementation to convert BRG images into HSV format */
void mvBRG2HSV(IplImage* src, IplImage* dst);

inline void mvGaussian (IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h) {
    cvSmooth (src, dst, CV_GAUSSIAN, kern_w, kern_h);
}
inline void mvDilate (IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);
    cvDilate (src, dst, kernel, iterations);
    cvReleaseStructuringElement (&kernel);
}
inline void mvErode (IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);
    cvErode (src, dst, kernel, iterations);
    cvReleaseStructuringElement (&kernel);
}
inline void mvOpen (IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);
    cvMorphologyEx (src, dst, NULL, kernel, CV_MOP_OPEN, iterations);
    cvReleaseStructuringElement (&kernel);
}
inline void mvClose (IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);
    cvMorphologyEx (src, dst, NULL, kernel, CV_MOP_CLOSE, iterations);
    cvReleaseStructuringElement (&kernel);
}
inline void mvGradient (IplImage* src, IplImage* dst, unsigned kern_w, unsigned kern_h, unsigned iterations=1) {
    IplImage* temp = cvCreateImage (cvGetSize(src), IPL_DEPTH_8U, 1);
    IplConvKernel* kernel = cvCreateStructuringElementEx (kern_w, kern_h, (kern_w+1)/2, (kern_h+1)/2, CV_SHAPE_ELLIPSE);

    cvMorphologyEx (src, dst, temp, kernel, CV_MOP_GRADIENT, iterations);
    
    cvReleaseImage (&temp);
    cvReleaseStructuringElement (&kernel);
}

/// split the image into any of its 2 planes in place
void mvSplitImage (IplImage* src, IplImage** plane1=NULL, IplImage** plane2=NULL);

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

	void mvBinaryMorphologyMain (MV_MORPHOLOGY_TYPE morphology_type, IplImage* src, IplImage* dst);

	public:
	mvBinaryMorphology (int Kernel_Width, int Kernel_Height, MV_KERNEL_SHAPE Shape);
	~mvBinaryMorphology ();

	void dilate (IplImage* src, IplImage* dst) {
          bin_morph.start();
		mvBinaryMorphologyMain (MV_DILATE, src, dst);
          bin_morph.stop();
	}
	void erode (IplImage* src, IplImage* dst) {
          bin_morph.start();
		mvBinaryMorphologyMain (MV_ERODE, src, dst);
          bin_morph.stop();
	}
	void close (IplImage* src, IplImage* dst) {
          bin_morph.start();
		mvBinaryMorphologyMain (MV_CLOSE, src, dst);
          bin_morph.stop();
	}
	void open (IplImage* src, IplImage* dst) {
          bin_morph.start();
		mvBinaryMorphologyMain (MV_OPEN, src, dst);
          bin_morph.stop();
	}
	void gradient (IplImage* src, IplImage* dst) {
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
    
    IplImage* scratch_3;

    PROFILE_BIN bin_WorkingLoop;
    PROFILE_BIN bin_CvtColor;
    
    void filter_internal (IplImage* HSV_img, IplImage* result);

    public:
    mvHSVFilter (const char* settings_file);
    ~mvHSVFilter ();
    
    void filter (IplImage* img, IplImage* result) {
        bin_CvtColor.start();
        cvCvtColor (img, scratch_3, CV_BGR2HSV); // convert to HSV 
        bin_CvtColor.stop();

        filter_internal (scratch_3, result);
    }

    void filter_non_common_size (IplImage* img, IplImage* result) {
        IplImage * temp_scratch; // scratch_3 is common size. temp_scratch will be same size as img
        temp_scratch = mvCreateImage_Color (img);

        bin_CvtColor.start();
        cvCvtColor (img, temp_scratch, CV_BGR2HSV); // convert to HSV 
        bin_CvtColor.stop();

        filter_internal (temp_scratch, result);

        cvReleaseImage (&temp_scratch);
    }

    void setHSV (int hmin=UNCHANGED, int hmax=UNCHANGED, 
                 unsigned smin=UNCHANGED, unsigned smax=UNCHANGED, 
                 unsigned vmin=UNCHANGED, unsigned vmax=UNCHANGED
                );    
};

void ManhattanDistanceFilter(IplImage* src, IplImage* dst);

/** mvAdaptiveFilter
 *  This class is a set of AdaptiveBox's to represent the water, the target, and 
 *  possibly any other objects we might see.
 */
class mvAdaptiveFilter {
    static const int nbins_hue = 30;
    static const int nbins_sat = 25;
    static const int hue_range_min = 0;
    static const int hue_range_max = 179;
    static const int sat_range_min = 6;
    static const int sat_range_max = 255;
    static const int MAX_BINS_MARKED = 9;
    static const int HISTOGRAM_NORM_FACTOR = 100000;
    static const bool DISPLAY_HIST = true;
    static const int NUM_SIDES_RECTANGLE = 4;

    struct Quad{
        int h0, s0, h1, s1;
    };

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

    private:
    void setQuad (Quad &Q, int h0, int s0, int h1, int s1);
    int getQuadValue (Quad Q);
    void getRectangleNeighbours(Quad rect, Quad sides[]);

    public:
    mvAdaptiveFilter (const char* settings_file);
    ~mvAdaptiveFilter ();
    void filter (IplImage* src, IplImage* dst);
    void print_histogram ();
    void show_histogram ();
};


/** Hue_Box:
 * This is a helper class for mvMeanShift.
 * It represents a moving pair of Hue_min and Hue_max
 */
class Hue_Box {
    static const bool DEBUG = 1;
public:
    unsigned char HUE_MIN;
    unsigned char HUE_MAX;
    unsigned char SAT_MIN;
    unsigned char VAL_MIN;
    int BOX_NUMBER;
    int BOX_COLOR;
    bool BOX_ENABLED; // whether the box is being used or not

    Hue_Box (const char* settings_file, int box_number);

    bool check_hue (unsigned char hue, unsigned char sat, unsigned char val) {
        // shifting logic goes here
        if (sat >= SAT_MIN && val >= VAL_MIN) {
            if (HUE_MAX >= HUE_MIN) 
                return (hue >= HUE_MIN && hue <= HUE_MAX);
            else
                return ((hue <= HUE_MIN && hue <= HUE_MAX) || (hue >= HUE_MIN && hue >= HUE_MAX)); 
        }
        return false;
    }

    bool is_enabled () {
        return BOX_ENABLED;
    }

};

/** mvMeanShift:
 * This class takes average values of a kernel surrounding each pixel
 * Then performs color filtering
 */
class mvMeanShift {
    //#define M_DEBUG
    //declare constants here
    static const int DS_FACTOR = 1; // downsampling
    static const int GOOD_PIXELS_FACTOR = 6;
    static const int KERNEL_SIZE = 5;
    static const int S_MIN = 60;
    static const int V_MIN = 30;

public:
    static const int NUM_BOXES = 3;

private:
    // parameters read from settings file
    int H_DIST;
    int S_DIST;
    int V_DIST;

    // internal data
    Hue_Box* hue_box[NUM_BOXES]; // array of pointers to boxes
    int* kernel_point_array;
    IplImage* ds_scratch_3;   // downsampled scratch image 3 channel
    IplImage* ds_scratch;   // 1 channel

    // profile bins
    PROFILE_BIN bin_Resize;
    PROFILE_BIN bin_MeanShift;
    PROFILE_BIN bin_Filter;
    
    void downsample_from(IplImage* src) {    // downsamples src to internal scratch image
        assert (src->nChannels == 3);
        bin_Resize.start();
          cvResize (src, ds_scratch_3, CV_INTER_NN);
        bin_Resize.stop();
    }
    void upsample_to_3 (IplImage* dst) {     // upsamples internal scrach to dst
        assert (dst->nChannels == 3);
        bin_Resize.start();
          cvResize (ds_scratch_3, dst, CV_INTER_NN);
        bin_Resize.stop();
    }
    void upsample_to (IplImage* dst) {     // upsamples internal scrach to dst
        assert (dst->nChannels == 1);
        bin_Resize.start();
          cvResize (ds_scratch, dst, CV_INTER_NN);
        bin_Resize.stop();
    }

    void mvMeanShift_internal(IplImage* scratch);
    void colorFilter_internal();

public: 
    mvMeanShift (const char* settings_file); //constructor
    ~mvMeanShift(); // destructor
    void mean_shift(IplImage* src, IplImage* dst);
    void filter(IplImage *src, IplImage* dst);
    void combined_filter(IplImage *src, IplImage* dst);
};

#endif
