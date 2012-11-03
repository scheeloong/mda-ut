#ifndef __MDA_VISION__MDA_VISION__
#define __MDA_VISION__MDA_VISION__

#include <highgui.h>
#include <cv.h>

#include "mgui.h"
#include "mv.h"
#include "mvLines.h"
#include "vci.h"

/// ########################################################################
/** This is the base class for a vision module. Every vision module needs to implement
 *      - void filter (const IplImage* src)
 *      - void calc_vci (VCI *interface)
 *  
 *  The first function filters the source image into _filtered_img
 *  The second calculates relevant information using the _filtered_img
 *
 *  Besides this you have the option of implementing get_range() and get_angle()
 *  You would do this if your module legitimately uses those values.
 */
/// ########################################################################
class MDA_VISION_MODULE_BASE {
protected:
    /// definitions related to camera and vision
    static const float TAN_FOV_X = 1.2;     // tangent of camera FOV, equal to real width/range
    static const float TAN_FOV_Y = 1.0;
    static const float RAD_TO_DEG = 57.2958;

    // stores resized image
    IplImage* _resized_; 

    /// you must implement these yourself!
    virtual void primary_filter (const IplImage* src) = 0;
    virtual int calc_vci (VCI* interface) = 0;
    
public:
    MDA_VISION_MODULE_BASE () { _resized_ = mvCreateImage_Color(); }
    virtual ~MDA_VISION_MODULE_BASE () { cvReleaseImage (&_resized_); } 

    int filter (const IplImage* src, VCI* interface) {
        assert (src != NULL);
        assert (src->nChannels == 3);
        
        cvResize (src, _resized_);
        _resized_->origin = src->origin;

        interface->clear();
        
        primary_filter (_resized_);
        return calc_vci (interface);
    };

};
/// ########################################################################
/// ########################################################################

/// You probably dont want to touch anything above this line

/// ########################################################################
/// this class is to test vision/sim/control interfaces. write whatever you want
/// ########################################################################
class MDA_VISION_MODULE_TEST : public MDA_VISION_MODULE_BASE {
#define MDA_VISION_MODULE_TEST_SETTINGS "vision_test_settings.csv"
    mvWindow _window;
    mvHSVFilter _HSVFilter;
    mvHoughLines _HoughLines;
    mvKMeans _KMeans;
    mvLines _lines;
    
    IplImage* _filtered_img;
    
public:
    MDA_VISION_MODULE_TEST ();
    ~MDA_VISION_MODULE_TEST ();
    
    void primary_filter (const IplImage* src);
    int calc_vci (VCI* interface);
};


/// ########################################################################
/// this class is for the gate
/// ########################################################################
class MDA_VISION_MODULE_GATE : public MDA_VISION_MODULE_BASE {
#define MDA_VISION_GATE_SETTINGS "vision_gate_settings.csv"
    static const float GATE_REAL_WIDTH = 300.0;
    static const float GATE_REAL_HEIGHT = 120.0;
    static const float GATE_REAL_SLENDERNESS = 0.017;
    static const float GATE_WIDTH_TO_HEIGHT_RATIO = 2.50;

    mvWindow _window;
    mvHSVFilter _HSVFilter;
    mvHoughLines _HoughLines;
    mvKMeans _KMeans;

    mvLines _lines;
    
    IplImage* _filtered_img;

public:
    MDA_VISION_MODULE_GATE ();
    ~MDA_VISION_MODULE_GATE ();
    
    void primary_filter (const IplImage* src);
    int calc_vci (VCI* interface);
};


/// ########################################################################
/// this class is for the path
/// ########################################################################
class MDA_VISION_MODULE_PATH : public MDA_VISION_MODULE_BASE {
#define MDA_VISION_PATH_SETTINGS "vision_path_settings.csv"
    static const float PATH_REAL_LENGTH = 120.0;
    static const float PATH_REAL_WIDTH = 15.0;

    mvWindow _window;
    mvHSVFilter _HSVFilter;
    mvBinaryMorphology _Morphology;
    mvHoughLines _HoughLines;
    mvKMeans _KMeans;

    mvLines _lines;
    
    IplImage* _filtered_img;

public:
    MDA_VISION_MODULE_PATH ();
    ~MDA_VISION_MODULE_PATH ();
    
    void primary_filter (const IplImage* src);
    int calc_vci (VCI* interface);
};

/// ########################################################################
/// this class is for the buoy
/// ########################################################################
class MDA_VISION_MODULE_BUOY : public MDA_VISION_MODULE_BASE {
    static const float BUOY_REAL_DIAMTER = 23;

public:
    MDA_VISION_MODULE_BUOY ();
    ~MDA_VISION_MODULE_BUOY ();
    
    void primary_filter (const IplImage* src);
    int calc_vci (VCI* interface);
};

/// ########################################################################
/// this class is for the frame
/// ########################################################################
class MDA_VISION_MODULE_FRAME : public MDA_VISION_MODULE_BASE {
#define MDA_VISION_FRAME_SETTINGS "vision_frame_settings.csv"
    static const float FRAME_REAL_WIDTH = 180.0;
    static const float FRAME_REAL_HEIGHT = 120.0;
    static const float FRAME_REAL_VERTICAL_SEGMENT_LENGTH = 40.0;

    mvWindow _window;
    mvHSVFilter _HSVFilter;
    mvHoughLines _HoughLines;
    mvKMeans _KMeans;

    mvLines _lines;
    
    IplImage* _filtered_img;

public:
    MDA_VISION_MODULE_FRAME ();
    ~MDA_VISION_MODULE_FRAME ();
    
    void primary_filter (const IplImage* src);
    int calc_vci (VCI* interface);
};

#endif