/** mvlines - mda circle finding algorithm library
 *  Author - Ritchie Zhao
 *  July 2012
 */ 
#ifndef __MDA_VISION_MVCIRCLES__
#define __MDA_VISION_MVCIRCLES__ 

#include <cv.h>
#include "mgui.h"
#include "mvLines.h"
#include "profile_bin.h"

typedef std::vector<CvPoint> POINT_VECTOR;

/** MV_SHAPE */
class mvShape {
    static const bool DEBUG = 1;
    int DOWNSAMPLING_FACTOR;

protected:
    IplImage* ds_image; // downsampled image
    mvWindow *window; 

    // function to downsample src to ds_image. If target_brightness is set then it
    // excludes (sets to 0) all pixels with brightness not equal to target_brightness
    void downsample_from_image (IplImage* src, int target_brightness=-1);
    void upsample_to_image(IplImage *dst);
    // puts all pixels with brightness equal to target_brightness into an array
    // if target_brightness is not set it takes all non-zero pixels
    int collect_pixels (IplImage* src, POINT_VECTOR &p_vector, int target_brightness=-1);

public:
    mvShape ();
    mvShape (const char* settings_file);
    ~mvShape ();
};


/** MV_RECTANGLE */
class mvRect : mvShape {
    typedef struct _row_ {
        int y, x1, x2;  // a row has a vertical coord, a starting x coord, an ending x coord
    } ROW; 
    typedef struct _rect_ {
        int x1, y1, x2, y2;
        int num;
    } RECT;

    ROW make_row (int y, int x1, int x2) {
        ROW R;
        R.y = y; R.x1 = x1; R.x2 = x2;
        return R;
    }
    RECT make_rect (int x1, int y1, int x2, int y2) {
        RECT R;
        R.x1 = x1; R.y1 = y1; R.x2 = x2; R.y2 = y2;
        R.num = 0;
        return R;
    }

    static const int MIN_POINTS_IN_RESAMPLED_IMAGE = 20;

    int MIN_STACKED_ROWS;
    int MIN_ROW_LENGTH;
    float RECT_HEIGHT_TO_WIDTH_RATIO;
    
    mvHoughLines LINES;
    mvKMeans KMEANS;

    std::vector<RECT> m_rect_v;

    PROFILE_BIN bin_rect;

public:
    mvRect (const char* settings_file);
    ~mvRect ();

    int find_internal (IplImage* img, int target_brightness);
    int find_internal2 (IplImage* img, int target_brightness);
    int find (IplImage* img, int target_brightness=-1);

    //CvPoint operator [] (unsigned index) { return m_rect[index]; }
    void draw_rectangle (IplImage *img) {
        for (std::vector<RECT>::iterator it = m_rect_v.begin(); it != m_rect_v.end(); it++) {
            cvRectangle (img, cvPoint(it->x1-1,it->y1-2), cvPoint(it->x2+1,it->y2+2), cvScalar(120));
            printf ("\tRect: (%d,%d) (%d,%d)\n", it->x1,it->y1, it->x2,it->y2);
        }
        //cvRectangle (img, m_rect[0], m_rect[1], cvScalar(120));
    }
    void remove_rectangle (IplImage* img) {
        for (std::vector<RECT>::iterator it = m_rect_v.begin(); it != m_rect_v.end(); it++)
            cvRectangle (img, cvPoint(it->x1-6,it->y1-8), cvPoint(it->x2+6,it->y2+8), cvScalar(0), CV_FILLED);
        //cvRectangle (img, m_rect[0], m_rect[1], cvScalar(0), CV_FILLED);
    }

};



/** MV_CIRCLE - simple structure to represent a circle */
// note OpenCV stores its circle finding algorithm's output in a
// float* array, where [0],[1] are x,y and [2] is radius
// so you can easily typecast the float* into MV_CIRCLE*
// and have easier, more readable code
typedef struct _Circle_ {
    int x;
    int y;
    float rad;
} MV_CIRCLE;

typedef std::pair<MV_CIRCLE, unsigned> CIRCLE_U_PAIR;
typedef std::pair<float,float> FLOAT_PAIR;

class mvAdvancedCircles {
    static const int CENTER_SIMILARITY_CONSTANT = 64; // squared
    static const int RADIUS_SIMILARITY_CONSTANT = 8;
    static const int N_POINTS_TO_CHECK = 20;
    static const int POINTS_NEEDED_IN_RESAMPLED_IMAGE = 10;
    static const int CIRCLE_THICKNESS = 2;

    int PIXELS_PER_GRID_POINT;
    float _MIN_RADIUS_;
    float _THRESHOLD_;
    int N_CIRCLES_GENERATED;
    unsigned N_CIRCLES_CUTOFF;

    IplImage* grid;                             // downsampled image
    unsigned grid_width, grid_height;

    std::vector<FLOAT_PAIR> cos_sin_vector;     // list of cos/sin values, precalculated
    std::vector<CIRCLE_U_PAIR> accepted_circles;    // list of circles found

    PROFILE_BIN bin_findcircles;

    private:
    int get_circle_from_3_points (CvPoint p1, CvPoint p2, CvPoint p3, MV_CIRCLE &Circle);
    int check_circle_validity (IplImage* img, MV_CIRCLE Circle);

    public:
    mvAdvancedCircles (const char* settings_file);
    ~mvAdvancedCircles ();
    void findCircles (IplImage* img);
    int ncircles ();
    MV_CIRCLE operator [] (unsigned index) { return accepted_circles[index].first; }
    void drawOntoImage (IplImage* img);
};


int find_rectangle (IplImage* img);
void remove_rectangle (IplImage* img);

#endif
