/** mvCircles.cpp  */

#include "mvCircles.h"
#include "mgui.h"
#include "mv.h"
#include "math.h"
#include <cv.h>

//#define FLAG_DEBUG
#ifdef FLAG_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

bool m_circle_has_greater_count (M_CIRCLE c1, M_CIRCLE c2) { return (c1.second > c2.second); }

mvAdvancedCircles::mvAdvancedCircles (const char* settings_file) :
    bin_findcircles ("mvAdvancedCircles")
{
    srand(time(NULL)); // seed for the partly random circle finding algorithm

    int width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    read_mv_setting (settings_file, "DOWNSAMPLING_FACTOR", PIXELS_PER_GRID_POINT);
    read_mv_setting (settings_file, "_MIN_RADIUS_", _MIN_RADIUS_);
    read_mv_setting (settings_file, "_THRESHOLD_", _THRESHOLD_);
    read_mv_setting (settings_file, "N_CIRCLES_REQUIRED", N_CIRCLES_REQUIRED);

    grid_width = width / PIXELS_PER_GRID_POINT;
    grid_height = height / PIXELS_PER_GRID_POINT;
    grid = cvCreateImageHeader (cvSize(grid_width, grid_height), IPL_DEPTH_8U, 1);

    float angular_division = 2*CV_PI/N_POINTS_TO_CHECK;
    float angle = 0;
    for (int i = 0; i < N_POINTS_TO_CHECK; i++) {
        cos_sin_vector.push_back(std::make_pair(cos(angle),sin(angle)));
        angle += angular_division;
    }

    IplImage* temp = mvGetScratchImage2();
    grid->imageData = temp->imageData;
}

mvAdvancedCircles::~mvAdvancedCircles () {
    cvReleaseImageHeader(&grid);
    mvReleaseScratchImage2();
}

void mvAdvancedCircles::findCircles (IplImage* img) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    assert (img->imageData != grid->imageData);

    bin_findcircles.start();
    accepted_circles.clear();

    /// massively downsample the image
    cvResize (img, grid, CV_INTER_NN);

    /// extract all the nonzero pixels and put them in an array
    std::vector<CvPoint> point_vector;
    unsigned char* ptr;
    for (int i = 0; i < grid->height; i++) {
        ptr = (unsigned char*) (grid->imageData + i*grid->widthStep);
        for (int j = 0; j < grid->width; j++) {
            if (*ptr != 0) {
                point_vector.push_back(cvPoint(j,i));
                //DEBUG_PRINT ("#");
            }
            else {
                //DEBUG_PRINT (".");
            }
            ptr++;
        }
        //DEBUG_PRINT ("\n");
    }
    //DEBUG_PRINT ("\n\n");
    unsigned n_points = point_vector.size();

    if (n_points < 5) {
        printf ("Not enough points in resampled image\n");
        return;
    }

    /// main working loop
    int valid_circles = 0;
    for (int N = 0; N < 200; N++) {
        // choose 3 points at random to form a circle
        int c1 = rand() % n_points;
        int c2 = rand() % n_points;
        int c3 = rand() % n_points;
        
        // get the circle center and radius
        CIRCLE_STRUCT Circle;
        if ( get_circle_from_3_points (point_vector[c1],point_vector[c2],point_vector[c3], Circle) )
            continue;

        // scale back to normal-sized image
        Circle.x *= PIXELS_PER_GRID_POINT;
        Circle.y *= PIXELS_PER_GRID_POINT;
        Circle.rad *= PIXELS_PER_GRID_POINT;

        // sanity roll
        if (Circle.x < 0 || Circle.x > img->width || Circle.y < 0 || Circle.y > img->height)
            continue;
        if (Circle.rad < _MIN_RADIUS_*img->width)
            continue;

        int count = check_circle_validity(img, Circle);
        DEBUG_PRINT("count: %d\n", count);
        if(count < _THRESHOLD_*N_POINTS_TO_CHECK)
            continue;

        // check each existing circle. avg with an existing circle or push new circle
        bool success = false;
        for (unsigned i = 0; i < accepted_circles.size(); i++) {
            int x = accepted_circles[i].first.x;
            int y = accepted_circles[i].first.y;
            int r = accepted_circles[i].first.rad;
            int c = accepted_circles[i].second;

            int dx = Circle.x - x;
            int dy = Circle.y - y;
            int dr = Circle.rad - r;
            if (abs(dr) < RADIUS_SIMILARITY_CONSTANT && dx*dx + dy*dy < CENTER_SIMILARITY_CONSTANT) {
                accepted_circles[i].first.x = (x*c + Circle.x)/(c + 1);    
                accepted_circles[i].first.y = (y*c + Circle.y)/(c + 1);    
                accepted_circles[i].first.rad = (r*c + Circle.rad)/(c + 1);    
                accepted_circles[i].second++;

                success = true;
                break;
            }
        }
        if (!success) {
            accepted_circles.push_back( std::make_pair(Circle, 1) );
        }

        valid_circles++;
        if (valid_circles >= N_CIRCLES_REQUIRED)
            break;
    }

    /// sort the circles by their counts
    std::sort (accepted_circles.begin(), accepted_circles.end(), m_circle_has_greater_count);

    bin_findcircles.stop();
}

void mvAdvancedCircles::drawOntoImage (IplImage* img) {
    std::vector<M_CIRCLE>::iterator iter = accepted_circles.begin();
    std::vector<M_CIRCLE>::iterator end_iter = accepted_circles.end();
    for (; iter != end_iter; ++iter) {
        cvCircle (img, cvPoint(iter->first.x,iter->first.y), iter->first.rad, CV_RGB(100,100,100), CIRCLE_THICKNESS);        
    }
}

int mvAdvancedCircles::ncircles() {
    return (int)(accepted_circles.size());
}

int mvAdvancedCircles::get_circle_from_3_points (CvPoint p1, CvPoint p2, CvPoint p3, CIRCLE_STRUCT &Circle) {
    /** Basically, if you have 3 points, you can solve for the point which is equidistant to all 3.
     *  You get a matrix equation Ax = B, where A is a 2x2 matrix, x is trans([X, Y]), and B is trans([B1 B2])
     *  Then you can solve for x = inv(A) * B. Which is what we do below.
    */
    int x1=p1.x,  y1=p1.y;
    int x2=p2.x,  y2=p2.y;
    int x3=p3.x,  y3=p3.y;

    // calcate the determinant of A
    int DET = (x1-x2)*(y1-y3)-(y1-y2)*(x1-x3);
    if (DET == 0) { // this means the points are colinear
        return 1;
    }

    // caculate the 2 components of B
    int B1 = x1*x1 + y1*y1 - x2*x2 - y2*y2;
    int B2 = x1*x1 + y1*y1 - x3*x3 - y3*y3;

    // solve for X and Y
    int X = ( (y1-y3)*B1 + (y2-y1)*B2 ) / 2 / DET;
    int Y = ( (x3-x1)*B1 + (x1-x2)*B2 ) / 2 / DET;

    Circle.x = X;
    Circle.y = Y;
    Circle.rad = sqrt((float)((x1-X)*(x1-X) + (y1-Y)*(y1-Y)));
    return 0;
}

int mvAdvancedCircles::check_circle_validity (IplImage* img, CIRCLE_STRUCT Circle) {
    int x, y, count = 0;
    std::vector<FLOAT_PAIR>::iterator it = cos_sin_vector.begin();
    std::vector<FLOAT_PAIR>::iterator end_it = cos_sin_vector.end();

    for(; it != end_it; ++it) {
        float cos_val = it->first;
        float sin_val = it->second;
        x = Circle.x + Circle.rad*cos_val;
        y = Circle.y + Circle.rad*sin_val;

         if (x < 0 || x > img->width || y < 0 || y > img->height)
            continue;

        unsigned char* ptr = (unsigned char*) (img->imageData + y*img->widthStep + x);
        if(*(ptr) == 255){
            count++;
        }
    }

    return count;
}
