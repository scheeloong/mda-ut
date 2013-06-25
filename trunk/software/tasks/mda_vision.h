#ifndef __MDA_VISION__MDA_VISION__
#define __MDA_VISION__MDA_VISION__

#include <highgui.h>
#include <cv.h>
#include <math.h>
#include <algorithm>

#include "mgui.h"
#include "mv.h"
#include "mvColorFilter.h"
#include "mvLines.h"
#include "mvShapes.h"
#include "mvContours.h"
#include "frame_data.h"

#define WEBCAM_CACHE 5

enum MDA_VISION_RETURN_CODE  {
    FATAL_ERROR,        // defaults to this if you dont change the value
    NO_TARGET,          // cant find anything, no defined data 
    UNKNOWN_TARGET,     // cant recognize target, returns centroid
    ONE_SEGMENT,
    TWO_SEGMENT,
    FULL_DETECT,
    FULL_DETECT_PLUS,
    DOUBLE_DETECT
};


/// ########################################################################
/** This is the base class for a vision module. Every vision module needs to implement
 *      - void filter (IplImage* src)
 *      - void calc_vci ()
 *  
 *  The first function filters the source image into filtered_img
 *  The second calculates relevant information using the filtered_img
 *
 *  Besides this you have the option of implementing get_range() and get_angle()
 *  You would do this if your module legitimately uses those values.
 */
/// ########################################################################
class MDA_VISION_MODULE_BASE {
protected:
    /// definitions related to camera and vision
    static const float TAN_FOV_X = 1.2;     // tangent of camera FOV, equal to real width/range
    static const float TAN_FOV_Y = 1.2;
    static const float RAD_TO_DEG = 57.2958;

    // stores numerical data that can be queried. The goal of the modules is to calculate this
    int m_pixel_x, m_pixel_y, m_range;
    float m_angular_x, m_angular_y;
    float m_angle;

    void clear_data () {
        m_pixel_x = m_pixel_y = m_range = MV_UNDEFINED_VALUE;
        m_angular_x = m_angular_y = m_angle = MV_UNDEFINED_VALUE;
    }

    /// you must implement these yourself!
    virtual void primary_filter (IplImage* src) = 0;
    virtual MDA_VISION_RETURN_CODE calc_vci () = 0;
    
public:
    MDA_VISION_MODULE_BASE () {}
    virtual ~MDA_VISION_MODULE_BASE () {} 

    virtual MDA_VISION_RETURN_CODE filter (IplImage* src) {
        assert (src != NULL);
        assert (src->nChannels == 3);
        
        clear_data();
        primary_filter (src);
        MDA_VISION_RETURN_CODE retval = calc_vci ();
 
        assert (retval != FATAL_ERROR);
        return retval;
    };

    virtual int get_pixel_x() {return m_pixel_x;}
    virtual int get_pixel_y() {return m_pixel_y;}
    virtual int get_angular_x() {return m_angular_x;}
    virtual int get_angular_y() {return m_angular_y;}
    virtual int get_range() {return m_range;}
    virtual int get_angle() {return m_angle;}

};
/// ########################################################################
/// ########################################################################

/// You probably dont want to touch anything above this line

/// ########################################################################
/// this class is to test stuff. write whatever you want
/// ########################################################################
class MDA_VISION_MODULE_TEST : public MDA_VISION_MODULE_BASE {
    static const char MDA_VISION_TEST_SETTINGS[];
    mvWindow window;
    mvWindow window2;
    mvHSVFilter HSVFilter;
    mvAdvancedColorFilter AdvancedColorFilter;
    mvWatershedFilter WatershedFilter;
    mvContours Contours;
    mvHoughLines HoughLines;
    mvKMeans KMeans;
    mvLines lines;
    
    mvHistogramFilter histogram_filter;

    IplImage* color_img;
    IplImage* gray_img;
    IplImage* gray_img_2;

    PROFILE_BIN bin_test;
    
public:
    MDA_VISION_MODULE_TEST ();
    ~MDA_VISION_MODULE_TEST ();
    
    void primary_filter (IplImage* src);
    MDA_VISION_RETURN_CODE calc_vci ();

    // prevent users from accessing range/angle
    virtual int get_range() {printf ("VISION_MODULE_TEST - get_range not allowed\n"); exit(1); return 0;}
    virtual int get_angle() {printf ("VISION_MODULE_TEST - get_angle not allowed\n"); exit(1); return 0;}
};


/// ########################################################################
/// this class is for the gate
/// ########################################################################
class MDA_VISION_MODULE_GATE : public MDA_VISION_MODULE_BASE {
    static const char MDA_VISION_GATE_SETTINGS[];
    static const float GATE_REAL_WIDTH = 300.0;
    static const float GATE_REAL_HEIGHT = 120.0;
    static const float GATE_REAL_SLENDERNESS = 0.017;
    static const float GATE_WIDTH_TO_HEIGHT_RATIO = 2.50;

    mvWindow window;
    mvHSVFilter HSVFilter;
    mvHoughLines HoughLines;
    mvKMeans KMeans;

    mvLines lines;
    
    IplImage* filtered_img;

public:
    MDA_VISION_MODULE_GATE ();
    ~MDA_VISION_MODULE_GATE ();
    
    void primary_filter (IplImage* src);
    MDA_VISION_RETURN_CODE calc_vci ();

    virtual int get_angle() {printf ("VISION_MODULE_GATE - get_angle not allowed\n"); exit(1); return 0;}
};


/// ########################################################################
/// this class is for the marker dropper
/// ########################################################################
class MDA_VISION_MODULE_MARKER : public MDA_VISION_MODULE_BASE {
    static const char MDA_VISION_MARKER_SETTINGS[];
    static const float HU_THRESH = 0.005;

    mvWindow window;
    mvHSVFilter HSVFilter;
    
    IplImage* filtered_img;

public:
    MDA_VISION_MODULE_MARKER ();
    ~MDA_VISION_MODULE_MARKER ();
    
    void primary_filter (IplImage* src);
    MDA_VISION_RETURN_CODE calc_vci ();

    bool* getFound() { return targets_found; }

private:
    bool targets_found[2];
};


/// ########################################################################
/// this class is for the path
/// ########################################################################
class MDA_VISION_MODULE_PATH : public MDA_VISION_MODULE_BASE {
    static const char MDA_VISION_PATH_SETTINGS[];
    static const float PATH_REAL_LENGTH = 120.0;
    static const float PATH_REAL_WIDTH = 15.0;

    //Thresholds for line grouping
    static const float LINE_DIS_THRESH = 50;    //PATH_REAL_WIDTH*1.33
    static const float LINE_ANG_THRESH = 15;    //Arbitrary
    static const float LINE_LEN_THRESH = 50;    //PATH_REAL_LENGTH*0.3
    
    //Relative importance/scaling of properties when grouping lines
    static const float K_ANG = 3.0;
    static const float K_LEN = 1.0;
    static const float K_DIS = 2.0;

    // for contour shape/color matching
    static const double COLOR_DIVISION_FACTOR = 2000;
    static const double DIFF_THRESHOLD = 1.8 + 80/200; // shape diff + color diff
    double DIFF_THRESHOLD_SETTING;
    int TARGET_BLUE, TARGET_GREEN, TARGET_RED;

    int m_pixel_x_alt, m_pixel_y_alt, m_range_alt;
    float m_angular_x_alt, m_angular_y_alt;
    float m_angle_alt;

    mvWindow window;
    mvWindow window2;
    mvHSVFilter HSVFilter;
    mvBinaryMorphology Morphology;
    mvBinaryMorphology Morphology2;    
    mvWatershedFilter watershed_filter;
    mvContours contour_filter;
    mvHoughLines HoughLines;
    mvKMeans KMeans;
    mvLines lines;
    
    IplImage* gray_img;
    IplImage* gray_img_2;
    
    IplImage* filtered_img;

public:
    MDA_VISION_MODULE_PATH ();
    ~MDA_VISION_MODULE_PATH ();

    //Unless FULL_DETECT_PLUS or DOUBLE_DETECT are returned, the data in *_alt have undefined values
    virtual int get_pixel_x_alt() {return m_pixel_x_alt;}
    virtual int get_pixel_y_alt() {return m_pixel_y_alt;}
    virtual int get_angular_x_alt() {return m_angular_x_alt;}
    virtual int get_range_alt() {return m_range_alt;}
    virtual int get_angle_alt() {return m_angle_alt;}
    
    void primary_filter (IplImage* src);
    virtual int get_angular_y() {
        printf ("MDA_VISION_MODULE_PATH does not support get_angular_y");
        exit (1);
    }
    virtual int get_angular_y_alt() {
        printf ("MDA_VISION_MODULE_PATH does not support get_angular_y");
        exit (1);
    }

    MDA_VISION_RETURN_CODE calc_vci ();
};

/// ########################################################################
/// this class is for the buoy
/// ########################################################################
class MDA_VISION_MODULE_BUOY : public MDA_VISION_MODULE_BASE {
    static const char MDA_VISION_BUOY_SETTINGS[];
    static const float BUOY_REAL_DIAMTER = 23;
    static const float MIN_PIXEL_RADIUS_FACTOR = 0.04;

    // for contour shape/color matching
    static const double COLOR_DIVISION_FACTOR = 2000;
    static const double DIFF_THRESHOLD = 0.3 + 40/200; // shape diff + color diff
    double DIFF_THRESHOLD_SETTING;
    int TARGET_BLUE, TARGET_GREEN, TARGET_RED;

    mvWindow window;
    mvWindow window2;
    mvBinaryMorphology Morphology5;
    mvBinaryMorphology Morphology3;
    mvWatershedFilter watershed_filter;
    mvContours contour_filter;
    mvAdvancedCircles AdvancedCircles;

    mvRect Rect;

    IplImage* gray_img;
    IplImage* gray_img_2;

    static const int N_FRAMES_TO_KEEP = 20;
    MDA_FRAME_DATA m_frame_data_vector[N_FRAMES_TO_KEEP];
    int read_index;
    int n_valid_frames;
    int n_valid_circle_frames;
    int n_valid_box_frames;

public:
    MDA_VISION_MODULE_BUOY ();
    MDA_VISION_MODULE_BUOY (const char* settings_file);
    ~MDA_VISION_MODULE_BUOY ();
    
    void primary_filter (IplImage* src);
    MDA_VISION_RETURN_CODE calc_vci ();

    MDA_VISION_RETURN_CODE filter (IplImage* src) {
        assert (src != NULL);
        assert (src->nChannels == 3);
        
        clear_data();
        add_frame (src);
        MDA_VISION_RETURN_CODE retval = calc_vci ();
 
        assert (retval != FATAL_ERROR);
        return retval;
    };

    virtual int get_angle() {printf ("VISION_MODULE_BUOY- get_angle not allowed\n"); exit(1); return 0;}

    // functions to support frame data stuff
    void add_frame (IplImage* src);
    void clear_frames () {
        for (int i = 0; i < N_FRAMES_TO_KEEP; i++)
            m_frame_data_vector[i].clear();
        read_index = 0;
    }  
    void print_frames ();
};

/// ########################################################################
/// this class is for the frame
/// ########################################################################
class MDA_VISION_MODULE_FRAME : public MDA_VISION_MODULE_BASE {
    static const char MDA_VISION_FRAME_SETTINGS[];
    static const float FRAME_REAL_WIDTH = 180.0;
    static const float FRAME_REAL_HEIGHT = 120.0;
    static const float FRAME_REAL_VERTICAL_SEGMENT_LENGTH = 40.0;

    mvWindow window;
    mvAdvancedColorFilter AdvancedColorFilter;
    mvHoughLines HoughLines;
    mvKMeans KMeans;

    mvLines lines;

    int IsRed;
    
    IplImage* filtered_img;

    int check_pixel_is_color (unsigned char* imgPtr, unsigned char color) {
        if ((imgPtr[0] == color || imgPtr[0] == MV_UNCOLORED) &&
            (imgPtr[-1] == color || imgPtr[-1] == MV_UNCOLORED) && 
            (imgPtr[1] == color || imgPtr[1] == MV_UNCOLORED)
          )
        {
            return 1;            
        }
        return 0;
    }

public:
    MDA_VISION_MODULE_FRAME ();
    ~MDA_VISION_MODULE_FRAME ();
    
    void primary_filter (IplImage* src);
    MDA_VISION_RETURN_CODE calc_vci ();

    virtual int get_angle() {printf ("VISION_MODULE_FRAME - get_angle not allowed\n"); exit(1); return 0;}
    int is_red() { return IsRed; } // 0 means no, 1 means yes, -1 means dunno
};

#endif
