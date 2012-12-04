/** mvlines - mda circle finding algorithm library
 *  Author - Ritchie Zhao
 *  July 2012
 */ 
#ifndef __MDA_VISION_MVCIRCLES__
#define __MDA_VISION_MVCIRCLES__ 

#include <cv.h>
#include "mgui.h"
#include "profile_bin.h"

#define CIRCLE_THICKNESS 2

/** CIRCLE_STRUCT - simple structure to represent a circle */
// note OpenCV stores its circle finding algorithm's output in a
// float* array, where [0],[1] are x,y and [2] is radius
// so you can easily typecast the float* into CIRCLE_STRUCT*
// and have easier, more readable code
typedef struct _Circle_ {
    int x;
    int y;
    float rad;
} CIRCLE_STRUCT;

typedef std::pair<CIRCLE_STRUCT, unsigned> M_CIRCLE;
typedef std::pair<float,float> FLOAT_PAIR;

class mvAdvancedCircles {
    static const int CENTER_SIMILARITY_CONSTANT = 64; // squared
    static const int RADIUS_SIMILARITY_CONSTANT = 8;
    static const int N_POINTS_TO_CHECK = 20;

    int PIXELS_PER_GRID_POINT;
    float _MIN_RADIUS_;
    float _THRESHOLD_;
    int N_CIRCLES_REQUIRED;

    IplImage* grid;                             // downsampled image
    unsigned grid_width, grid_height;

    std::vector<FLOAT_PAIR> cos_sin_vector;     // list of cos/sin values, precalculated
    std::vector<M_CIRCLE> accepted_circles;    // list of circles found

    PROFILE_BIN bin_findcircles;

    private:
    int get_circle_from_3_points (CvPoint p1, CvPoint p2, CvPoint p3, CIRCLE_STRUCT &Circle);
    int check_circle_validity (IplImage* img, CIRCLE_STRUCT Circle);

    public:
    mvAdvancedCircles (const char* settings_file);
    ~mvAdvancedCircles ();
    void findCircles (IplImage* img);
    int ncircles ();
    CIRCLE_STRUCT operator [] (unsigned index) { return accepted_circles[index].first; }
    void drawOntoImage (IplImage* img);
};

#endif