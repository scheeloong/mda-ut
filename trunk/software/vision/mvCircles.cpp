/** mvCircles.cpp  */

#include "mvCircles.h"
#include "mgui.h"
#include "mv.h"
#include "math.h"
#include <list>
#include <cv.h>

//#define FLAG_DEBUG
#ifdef FLAG_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

void mvCircles::drawOntoImage (IplImage* img) {
    assert (img != NULL);
    assert (_data != NULL);
    assert (img->nChannels == 1);
    
    float* circleData;
    for (int i = 0; i < _data->total; i++) {
        circleData = (float*)cvGetSeqElem(_data, i); 
        CvPoint center = cvPoint(circleData[0],circleData[1]); // [0],[1] are x and y of center
        // [2] is radius
        
        cvCircle(img, center, cvRound(circleData[2]), CV_RGB(50,50,50), CIRCLE_THICKNESS);
    }
}


/** mvHoughCircles methods */
mvHoughCircles::mvHoughCircles (const char* settings_file) :
    bin_findcircles ("mvHoughCircles - findCircles")
{
    read_mv_setting (settings_file, "_ACCUMULATOR_THRESHOLD_", _ACCUMULATOR_THRESHOLD_);
    read_mv_setting (settings_file, "_MIN_CENTER_DIST_", _MIN_CENTER_DIST_);
    read_mv_setting (settings_file, "CANNY_HIGH_THRESHOLD", CANNY_HIGH_THRESHOLD);
    read_mv_setting (settings_file, "PIXEL_RESOLUTION", PIX_RESOLUTION);
}

void mvHoughCircles::findCircles (IplImage *img, mvCircles* circles) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    assert (circles != NULL);
    assert (circles->_data == NULL); // make sure there isnt already data
    
    bin_findcircles.start();
    unsigned imgwidth = img->width;
    circles->_data = cvHoughCircles(
        img, 
        circles->_storage, 
        CV_HOUGH_GRADIENT,      // method
        PIX_RESOLUTION,
        _MIN_CENTER_DIST_ * imgwidth,
        CANNY_HIGH_THRESHOLD,
        _ACCUMULATOR_THRESHOLD_ * imgwidth
    );
    bin_findcircles.stop();
}

mvAdvancedCircles::mvAdvancedCircles () :
    bin_findcircles ("mvAdvancedCircles")
{
    int width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);

    grid_width = width / PIXELS_PER_GRID_POINT;
    grid_height = height / PIXELS_PER_GRID_POINT;
    grid = cvCreateImageHeader (cvSize(grid_width, grid_height), IPL_DEPTH_8U, 1);

    srand(time(NULL));

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

void mvAdvancedCircles::findCircles ( IplImage* img) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    assert (img->imageData != grid->imageData);

    bin_findcircles.start();

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
                DEBUG_PRINT ("#");
            }
            else 
                DEBUG_PRINT (".");
            ptr++;
        }
        DEBUG_PRINT ("\n");
    }
    DEBUG_PRINT ("\n\n");

    unsigned n_points = point_vector.size();
    std::vector<PR_CIRCLE> accepted_circles;
    //std::vector<PR_CIRCLE> rejected_circles; 
    std::vector<unsigned> circle_counts; 

    if (n_points < 5) {
        printf ("Not enough points in resampled image\n");
        return;
    }

    /// main working loop
    unsigned valid_circles = 0;
    for (int N = 0; N < 500; N++) {
        // choose 3 points at random to form a circle
        int c1 = rand() % n_points;
        int c2 = rand() % n_points;
        int c3 = rand() % n_points;
        
        // get the circle center and radius
        CvPoint Center;
        float Radius;
        if ( get_circle_from_3_points (point_vector[c1],point_vector[c2],point_vector[c3], Center,Radius) )
            continue;

        // scale back to normal-sized image
        Center.x *= PIXELS_PER_GRID_POINT;
        Center.y *= PIXELS_PER_GRID_POINT;
        Radius *= PIXELS_PER_GRID_POINT;

        // sanity roll
        if (Center.x < 0 || Center.x > img->width || Center.y < 0 || Center.y > img->height)
            continue;
        if (Radius < MIN_RADIUS)
            continue;

        int count = check_circle_validity(img, Center, Radius);
        DEBUG_PRINT("count: %d\n", count);
        if(count < 0.5*N_POINTS_TO_CHECK)
            continue;

        // check each existing circle. avg with an existing circle or push new circle
        bool success = false;
        //std::vector<PR_CIRCLE>::iterator iter = accepted_circles.begin();
        // std::vector<PR_CIRCLE>::iterator end_iter = accepted_circles.end();
        // for (; iter != end_iter; ++iter) {
        for (unsigned i = 0; i < accepted_circles.size(); i++) {
            int x = accepted_circles[i].first.x;
            int y = accepted_circles[i].first.y;
            int r = accepted_circles[i].second;
            int c = circle_counts[i];

            int dx = Center.x - x;
            int dy = Center.y - y;
            int dr = Radius - r;
            if (dx*dx + dy*dy + dr*dr < 48){
                accepted_circles[i].first.x = (x*c + Center.x)/(c + 1);    
                accepted_circles[i].first.y = (y*c + Center.y)/(c + 1);    
                accepted_circles[i].second = (r*c + Radius)/(c + 1);    
                circle_counts[i]++;

                success = true;
                break;
            }
        }
        if (!success) {
            accepted_circles.push_back( std::make_pair(Center, Radius) );
            circle_counts.push_back(1);
        }

        valid_circles++;
        if (valid_circles > 10)
            break;
    }

    std::vector<PR_CIRCLE>::iterator iter = accepted_circles.begin();
    std::vector<PR_CIRCLE>::iterator end_iter = accepted_circles.end();
    for (; iter != end_iter; ++iter) {
        cvCircle (img, iter->first, iter->second, CV_RGB(100,100,100));        
    }
    printf ("ncircles= %d, %ld\n", valid_circles, accepted_circles.size());

    bin_findcircles.stop();
}

int mvAdvancedCircles::get_circle_from_3_points (CvPoint p1, CvPoint p2, CvPoint p3, CvPoint &center, float &radius) {
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

    center = cvPoint(X,Y);
    radius = sqrt((float)((x1-X)*(x1-X) + (y1-Y)*(y1-Y)));
    return 0;
}

int mvAdvancedCircles::check_circle_validity (const IplImage* img, CvPoint center, float radius) {
    int x, y, count = 0;
    for(std::vector<FLOAT_PAIR>::iterator it = cos_sin_vector.begin(); it != cos_sin_vector.end(); ++it) {
        float cos_val = it->first;
        float sin_val = it->second;
        x = center.x + radius*cos_val;
        y = center.y + radius*sin_val;

         if (x < 0 || x > img->width || y < 0 || y > img->height)
            continue;

        unsigned char* ptr = (unsigned char*) (img->imageData + y*img->widthStep + x);
        if(*(ptr) == 255){
            count++;
        }
    }
    return count;
}
