#ifndef __MDA_VISION__MDA_VISION__
#define __MDA_VISION__MDA_VISION__

#include "mgui.h"
#include "mv.h"
#include "mvLines.h"

/// ########################################################################
/** This is the base class for a vision module. Every vision module needs to implement
 *      - void filter (const IplImage* src)
 *      - void calc_vci (int &vci)
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
    // stores resized image
    IplImage* _resized_; 

    /// this is the interface data you are passing to control
    int x, y, range;
    int angle;

    /// you must implement these yourself!
    virtual void primary_filter (const IplImage* src) = 0;
    virtual void calc_vci () = 0;
    
public:
    MDA_VISION_MODULE_BASE () { _resized_ = mvCreateImage_Color(); }
    ~MDA_VISION_MODULE_BASE () { delete _resized_; }

    void filter (const IplImage* src, int &vci) {
        assert (src != NULL);
        assert (src->nChannels == 3);
        
        cvResize (src, _resized_);
        _resized_->origin = src->origin;
        
        primary_filter (_resized_);
        calc_vci ();
    };

    /// accessors for the vision-control interface. Note you only overload
    /// the values your module is responsible for!
    int get_x () { return x; }
    int get_y () { return y; }
    virtual int get_range() { 
        printf ("Fatal Error: vision_module - You are accessing invalid variable 'range' !\n");
        assert (0);
    }
    virtual int get_angle() { 
        printf ("Fatal Error: vision_module - You are accessing invalid variable 'angle' !\n");
        assert (0);
    }
};
/// ########################################################################
/// ########################################################################

/// You probably dont want to touch anything above this line

/// ########################################################################
/// this class is to test vision/sim/control interfaces. write whatever you want
/// ########################################################################
class MDA_VISION_MODULE_TEST : public MDA_VISION_MODULE_BASE {
#define MDA_VISION_MODULE_TEST_SETTINGS "vision_test_settings.csv"
    mvWindow* _window;
    mvHSVFilter* _HSVFilter;
    mvHoughLines* _HoughLines;
    mvLines* _lines;
    
    IplImage* _filtered_img;
    
public:
    MDA_VISION_MODULE_TEST ();
    ~MDA_VISION_MODULE_TEST ();
    
    void primary_filter (const IplImage* src);
    void calc_vci ();

    int get_range() { return range; }
};


/// ########################################################################
/// this class is for the gate
/// ########################################################################
class MDA_VISION_MODULE_GATE : public MDA_VISION_MODULE_BASE {

public:
    MDA_VISION_MODULE_GATE ();
    ~MDA_VISION_MODULE_GATE ();
    
    void primary_filter (const IplImage* src);
    void calc_vci ();
    int get_range() { return range; }
};


/// ########################################################################
/// this class is for the path
/// ########################################################################
class MDA_VISION_MODULE_PATH : public MDA_VISION_MODULE_BASE {

public:
    MDA_VISION_MODULE_PATH ();
    ~MDA_VISION_MODULE_PATH ();
    
    void primary_filter (const IplImage* src);
    void calc_vci ();
    int get_range() { return range; }
    int get_angle() { return angle; }
};

/// ########################################################################
/// this class is for the buoy
/// ########################################################################
class MDA_VISION_MODULE_BUOY : public MDA_VISION_MODULE_BASE {

public:
    MDA_VISION_MODULE_BUOY ();
    ~MDA_VISION_MODULE_BUOY ();
    
    void primary_filter (const IplImage* src);
    void calc_vci ();
    int get_range() { return range; }
};

#endif
