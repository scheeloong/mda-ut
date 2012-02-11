#ifndef _COMMON
#define _COMMON

#include <cv.h>
#include <highgui.h>

/** this file containts constants universal to all vision/task functions */

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
    void setSim1 (); // for orange gate in sim
    void setSim2 (); // for brown pipe in sim
};

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

enum retcode {NO_DETECT, DETECT_1, DETECT_2, ERROR, DONE};

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

/**######### HELPER FUNCTIONS ###########*/
int sqr_length (CvPoint** clusters, int i);
float tangent_angle (CvPoint** clusters, int i);

#endif
