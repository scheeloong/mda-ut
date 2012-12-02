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

/** mvCircles - class representing a set of circles */
// The class automatically allocates memory needed for storage
// 
// If you declare mvCircles circles; and want to access data for circle i, use 
// circles[i].x , circles[i].y , circles[i].rad
class mvCircles {
    CvSeq* _data;
    CvMemStorage* _storage; // this stores actual circle data in opencv
    
    friend class mvHoughCircles;
    
    public:
    // the constructor allocates 800 bytes of storage space, which is like 70 circles...
    mvCircles () { _data=NULL; _storage=cvCreateMemStorage(800); } 
    ~mvCircles () {} // opencv will clean up the memory used (i hope)
    
    unsigned ncircles () { return (_data != NULL) ? unsigned(_data->total) : 0; }
    void drawOntoImage (IplImage* img); // defined in .cpp
    
    CIRCLE_STRUCT operator [] (unsigned index) { return  (*(CIRCLE_STRUCT*)cvGetSeqElem(_data,index)); }
    
    // note clearData does NOT deallocate memory, it only allows recycling of used memory. 
    void clearData () { 
        if (_data) {
            cvClearSeq(_data); 
            _data=NULL; 
            cvClearMemStorage(_storage);
        }
    } 
};

/** mvHoughLines - Hough Line finding filter */
// you must provide an mvCircles object for it to work
// for a filled circle, it is best to take the gradient seperately 
// before using HoughCircles
class mvHoughCircles {
    unsigned PIX_RESOLUTION;
    float _MIN_CENTER_DIST_; // ang_resolution is in radians
    unsigned CANNY_HIGH_THRESHOLD;
    float _ACCUMULATOR_THRESHOLD_;

    PROFILE_BIN bin_findcircles;
    
    public:
    mvHoughCircles (const char* settings_file);
    /* use default destructor */
    void findCircles (IplImage *img, mvCircles* circles);
};

class mvAdvancedCircles {
    static const unsigned PIXELS_PER_GRID_POINT = 5;
    static const float MIN_RADIUS = 20;
    static const int MIN_CENTER_DIST = 100;
    static const int N_POINTS_TO_CHECK = 18;
    static const float POINTS_THRESHOLD = 0.5;
    static const int N_CIRCLES_REQUIRED = 10;
    static const int CIRCLE_SIMILARITY_CONSTANT = 48;

    IplImage* grid;                             // downsampled image
    unsigned grid_width, grid_height;

    std::vector<FLOAT_PAIR> cos_sin_vector;     // list of cos/sin values, precalculated
    std::vector<M_CIRCLE> accepted_circles;    // list of circles found

    PROFILE_BIN bin_findcircles;

    private:
    int get_circle_from_3_points (CvPoint p1, CvPoint p2, CvPoint p3, CIRCLE_STRUCT &Circle);
    int check_circle_validity (IplImage* img, CIRCLE_STRUCT Circle);

    public:
    mvAdvancedCircles ();
    ~mvAdvancedCircles ();
    void findCircles (IplImage* img);
    int ncircles ();
    CIRCLE_STRUCT operator [] (unsigned index) { return accepted_circles[index].first; }
    void drawOntoImage (IplImage* img);
};

#endif