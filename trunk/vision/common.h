#ifndef _COMMON
#define _COMMON

#include <cv.h>
#include <highgui.h>

#define ABS(x) ((x>0) ? (x) : (-(x)))
#define TAN_FOV_X 1.2 // Tan of the horizontal FOV of cam. Equal to full_width/range
#define TAN_FOV_Y 1.0

/** this file containts constants universal to all vision/task functions */

#define TAN_FOV_X 1.2 // Tan of the horizontal FOV of cam. Equal to full_width/range
#define TAN_FOV_Y 1.0

// flags for vision functions
#define _DISPLAY 1
#define _INVERT 2
#define _QUIET 4
#define _ADJ_COLOR 8
//#define _RETURN_OBJ 8

#define WIN0 "window0"
#define WIN1 "window1"
#define WIN2 "window2"

/// HSV SETTINGS CLASS 
class HSV_settings {
    public:
    int H_MIN, H_MAX;
    unsigned S_MIN, S_MAX;
    unsigned V_MIN, V_MAX;
    
    HSV_settings ();
    HSV_settings (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax);
    void setAll (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax); // set all values
    void setHue (int hmin, int hmax); // set hue
    void setSat (unsigned smin, unsigned smax); // set saturation
    void setVal (unsigned vmin, unsigned vmax);
    int HueInRange (unsigned hue);
    void setSim_gate (); // for orange gate in sim
    void setSim_path (); // for brown pipe in sim
    void setSim_buoyR (); // for red buoy
    void setSim_buoyG (); // for green
    void setSim_buoyY (); // for yellow
    void setSim_torp (); // for red side of torpedo target
    void setSim_U (); // for green U obstacle
};

/// FOR CONTROLLERS
enum retcode {NO_DETECT, DETECT_1, DETECT_2, DETECT_3, 
	      ERROR, DONE};

// a struct to hold all the inputs to the task functions
class vision_in {
public:
    IplImage *img;
    HSV_settings HSV;
    char **window;

    vision_in ();
    ~vision_in ();
};
void create_windows ();
void destroy_windows ();

struct vision_out {
    float real_x, real_y; // angular deviation from a ray perpendicular to image from image center
    float range;        // in centimeters
    float tan_PA;    // tangent of position angle (position angle is angle of say a pipe in the viewing plane)
};

/// CLASS PI_CONTROLLER
class PI_Controller {
private:
    #define ALPHA 0.94
    float err, integral; // error and the integral of error
    float target;
    float K1, K2;
    
public:
    // constructor, set variables
    PI_Controller () { target = 0; }
    void setTarget (float t) { target = t; }
    void setK1K2 (float k1, float k2) { K1 = k1; K2 = k2; }
    // controller functions
    void reset () { integral = 0; err = 0; }
    float out () { return K1*err + K2*integral; }
    void update (float signal) {
        err = signal - target;
        integral = err + ALPHA * integral; // depreciate the integral so it goes to zero after a while
    }
};

/**######### HELPER FUNCTIONS ###########*/
int sqr_length (CvPoint** clusters, int i);
float tangent_angle (CvPoint** clusters, int i);

#endif
