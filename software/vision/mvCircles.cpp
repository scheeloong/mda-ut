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

// ######################################################################################
//  mvShape base class
// ######################################################################################

mvShape::mvShape() {
    DOWNSAMPLING_FACTOR = 1; // default constructor - use no ds
    ds_image = mvGetScratchImage(); // can use scratch image since we are not doing ds
    
    if (DEBUG) window = new mvWindow("mvShape Debug");
}

mvShape::mvShape(const char* settings_file) {
    int width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);

    read_mv_setting (settings_file, "DOWNSAMPLING_FACTOR", DOWNSAMPLING_FACTOR);
    assert (DOWNSAMPLING_FACTOR >= 1);

    IplImage* temp = mvGetScratchImage();
    ds_image = cvCreateImageHeader(cvSize(width/DOWNSAMPLING_FACTOR, height/DOWNSAMPLING_FACTOR), IPL_DEPTH_8U, 1);
    ds_image->imageData = temp->imageData;

    if (DEBUG) window = new mvWindow("mvShape Debug");
}

mvShape::~mvShape() {
    cvReleaseImageHeader(&ds_image);
    mvReleaseScratchImage();
    if (DEBUG) delete window;
}

void mvShape::downsample_from_image(IplImage* src, int target_brightness) {
    assert (src->nChannels = 1);

    // downsample image if needed
    if (src->width == ds_image->width && src->height == ds_image->height)
        cvCopy (src, ds_image);
    else
        cvResize (src, ds_image);

    // remove non-target_brightness pixels if needed
    if (target_brightness >= 0) {
        unsigned char* ptr;
        for (int i = 0; i < ds_image->height; i++) {
            ptr = (unsigned char*) (ds_image->imageData + i*ds_image->widthStep);
            for (int j = 0; j < ds_image->width; j++)
                if (*ptr != target_brightness)
                    *(ptr++) = 0;
        }    
    }
}

void mvShape::upsample_to_image(IplImage *dst) {

}

int mvShape::collect_pixels (IplImage* src, POINT_VECTOR &p_vector, int target_brightness) {
    unsigned char* ptr;

    if (target_brightness >= 0) {   // target_brightness pixels only
        for (int i = 0; i < src->height; i++) {
            ptr = (unsigned char*) (src->imageData + i*src->widthStep);
            for (int j = 0; j < src->width; j++)
                if (*(ptr++) == target_brightness)
                    p_vector.push_back(cvPoint(j,i));
        }
    }
    else {  // all nonzero pixels
        for (int i = 0; i < src->height; i++) {
            ptr = (unsigned char*) (src->imageData + i*src->widthStep);
            for (int j = 0; j < src->width; j++)
                if (*(ptr++) != 0)
                    p_vector.push_back(cvPoint(j,i));
        }   
    }

    return p_vector.size();
}

// ######################################################################################
//  mvRect class
// ######################################################################################

mvRect::mvRect(const char* settings_file) : mvShape(settings_file) {
}

mvRect::~mvRect() {

}

int mvRect::find(IplImage* img, int target_brightness) {
    int result;
    
    if (img->width == ds_image->width && img->height == ds_image->height) { // no downsampling required
        result = find_internal (img, target_brightness);
    }
    else {
        downsample_from_image (img, target_brightness);
        result = find_internal (ds_image, target_brightness);
        upsample_to_image (img);
    }
    
    return result;
}

int mvRect::find_internal(IplImage* img, int target_brightness) {
    // algorithm description

    POINT_VECTOR point_vector;
    std::vector<ROW> row_vector;
    std::vector<int> stacked_rows;

    /// put appropriate pixels into point_vector
    int n_points = collect_pixels (img, point_vector, target_brightness);
    if (n_points < MIN_POINTS_IN_RESAMPLED_IMAGE) {
        DEBUG_PRINT ("mvRect: not enough points in resampled image\n");
        return -1;
    }

    /// go through the point_vector, identify long rows and put them in row_vector
    /// this is done simply by keeping track of continuous pixels on a row and putting the row into the
    /// vector if number of continuous pixels is greater than MIN_ROW_LENGTH 
    int contiguous_length = 0;
    for (int curr=1, first=-1; curr < n_points; curr++) {
        if (point_vector[curr].y == point_vector[curr-1].y && point_vector[curr].x-point_vector[curr-1].x < 3) { // contiguous point
            if (contiguous_length == 0) // start of new sequence
                first = curr-1;

            contiguous_length++;
        }
        else { // end of current sequence
            if (contiguous_length >= MIN_ROW_LENGTH)
                row_vector.push_back(make_row(point_vector[first].y, point_vector[first].x, point_vector[curr-1].x));

            contiguous_length = 0;
        }

        curr++;
    }
    
    /// look for a bunch of rows stacked on top of each other. the algorithm to do so is very similar to above!
    /// a bunch of stacked rows == a rectangle
    while (1) {
        int n_rows = row_vector.size();
        if (n_rows < MIN_STACKED_ROWS)
            break;

        printf ("mvRect: looping to look for stacked rows. %d rows total\n", n_rows);

        /// go thru each row. Look at next row and curr row. If they constitute a stack, push curr row
        /// when we find a non-stacking row, either empty the stacked_rows vector (if num of stacked rows is low)
        /// or (if num of stacked rows is high) we detected a rectangle => exit
        for (int curr=0; curr < n_rows-1; curr++) {
            if (row_vector[curr+1].y - row_vector[curr].y <= 2 && 
                abs(row_vector[curr+1].x1 - row_vector[curr].x1) <= 5 &&
                abs(row_vector[curr+1].x2 - row_vector[curr].x2) <= 5
                ) 
            {   // curr+1 is stacked on top of curr
                stacked_rows.push_back(curr);
            }
            else {
                if ((int)stacked_rows.size() < MIN_STACKED_ROWS) { 
                    // if number of stacked lines detected is small, delete all rows we just encountered
                    for (int i = (int)stacked_rows.size()-1; i >= 0; i--) {
                        row_vector.erase(row_vector.begin() + stacked_rows[i]);
                    }
                    stacked_rows.clear();
                    break;
                }
                else {
                    stacked_rows.push_back(curr+1);
                    break;
                }
            }
        }

        /// have to do this check here or we get errors (why?)
        int n_stacked = stacked_rows.size();
        if (n_stacked < MIN_STACKED_ROWS) {
            continue;
        }    
        printf ("mvRect: found stack of %d rows\n", n_stacked);

        /// next look at whether this stack of rows can be considered a rectangle
        int mean_left=0, mean_right=0;
        float var_left=0, var_right=0;

        for (int i = (int)n_stacked-1; i >= 0; i--) {
            int index = stacked_rows[i];
            printf ("\trow %d, %d-%d\n", row_vector[index].y,row_vector[index].x1,row_vector[index].x2);
        
            mean_left += row_vector[index].x1;
            mean_right += row_vector[index].x2;     
        }
        mean_left /= n_stacked;
        mean_right /= n_stacked;

        for (int i = (int)n_stacked-1; i >= 0; i--) {
            int index = stacked_rows[i];

            float delta_left = row_vector[index].x1 - mean_left;
            var_left += delta_left*delta_left;
            float delta_right = row_vector[index].x2 - mean_right;
            var_left += delta_right*delta_right;
        }
        var_left /= n_stacked;
        var_right /= n_stacked;

        printf ("mean = %d, %d\n", mean_left, mean_right);
        printf ("var = %f, %f\n", var_left, var_right);

        /// if variances are low, accept this rectangle
        if (var_left < 3 && var_right < 3) {
            m_rect[0].x = mean_left - 1;
            m_rect[0].y = row_vector[stacked_rows[0]].y - 1;
            m_rect[1].x = mean_right + 1;
            m_rect[1].y = row_vector[stacked_rows[n_stacked-1]].y + 1;
            break;
        }

        /// otherwise erase all the rows we examined
        for (int i = (int)n_stacked-1; i >= 0; i--) {
            row_vector.erase(row_vector.begin() + stacked_rows[i]);
        }
        stacked_rows.clear();
    }
    return 0;
}


bool m_circle_has_greater_count (CIRCLE_U_PAIR c1, CIRCLE_U_PAIR c2) { return (c1.second > c2.second); }

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
    read_mv_setting (settings_file, "N_CIRCLES_GENERATED", N_CIRCLES_GENERATED);
    read_mv_setting (settings_file, "N_CIRCLES_CUTOFF", N_CIRCLES_CUTOFF);

    assert (PIXELS_PER_GRID_POINT >= 1);

    grid_width = width / PIXELS_PER_GRID_POINT;
    grid_height = height / PIXELS_PER_GRID_POINT;
    grid = cvCreateImageHeader (cvSize(grid_width, grid_height), IPL_DEPTH_8U, 1);

    float angular_division = 2*CV_PI/N_POINTS_TO_CHECK;
    float angle = 0;
    for (int i = 0; i < N_POINTS_TO_CHECK; i++) {
        cos_sin_vector.push_back(std::make_pair(cos(angle),sin(angle)));
        angle += angular_division;
    }

    IplImage* temp = mvGetScratchImage3();
    grid->imageData = temp->imageData;
}

mvAdvancedCircles::~mvAdvancedCircles () {
    cvReleaseImageHeader(&grid);
    mvReleaseScratchImage3();
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
            }
            else {
            }
            ptr++;
        }
    }
    int n_points = point_vector.size();
    if (n_points < POINTS_NEEDED_IN_RESAMPLED_IMAGE) {
        DEBUG_PRINT ("Not enough points in resampled image\n");
        return;
    }

    /// main working loop
    int n_valid_circles = 0;
    for (int N = 0; N < 60*N_CIRCLES_GENERATED; N++) {
        // choose a point at random then choose a point with same Y, then a point with same X
        int c1, c2, c3;
        c1 = rand() % n_points;
        do {c2 = rand() % n_points;} while (c2 == c1);
        do {c3 = rand() % n_points;} while (c3 == c1 || c3 == c2);
        /*
        // now choose a point with same Y coord as c1, by looking near c1 first
        int i=c1, j=c1;
        while (1) {
            // search forward with index i until hit end of vector or hit a point that is not within 1 row of c1
            if (i < n_points-1 && point_vector[++i].y-point_vector[c1].y <= 1) { 
                if (std::abs(point_vector[i].x - point_vector[c1].x) > 3) {
                    c3 = i;
                    break;
                }
            }
            // search backward with index j until hit end of vector or hit a point that is not within 1 row of c1
            else if (j > 0 && point_vector[c1].y-point_vector[--j].y <= 1) {
                if (std::abs(point_vector[j].x - point_vector[c1].x) > 3) {
                    c3 = j;
                    break;
                }
            }
            // break if both iterators cannot advance 
            else {
                c3 = rand() % n_points;
                break;
            }
        }*/
    
        
        
        // get the circle center and radius
        MV_CIRCLE Circle;
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
        //DEBUG_PRINT("count: %d\n", count);
        if(count < _THRESHOLD_*N_POINTS_TO_CHECK)
            continue;

        // check each existing circle. avg with an existing circle or push new circle
        bool similar_circle_exists = false;
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

                // erase the circle from resampled image if cutoff is reached
                // this gets rid of a circle that is already found
                // this is done by removing points from point_vector
                if (accepted_circles[i].second == N_CIRCLES_CUTOFF) {
                    for (unsigned j = 0; j < point_vector.size(); j++) {
                        MV_CIRCLE temp_circle;
                        temp_circle.x = accepted_circles[i].first.x / PIXELS_PER_GRID_POINT;
                        temp_circle.y = accepted_circles[i].first.y / PIXELS_PER_GRID_POINT;
                        temp_circle.rad = accepted_circles[i].first.rad / PIXELS_PER_GRID_POINT;

                        int dx = point_vector[j].x - temp_circle.x;
                        int dy = point_vector[j].y - temp_circle.y;
                        float ratio = sqrt(dx*dx + dy*dy) / temp_circle.rad;

                        if (ratio < 1.2 && ratio > 0.83) {    // remove point if its approximately on temp_circle
                            point_vector.erase(point_vector.begin()+j);
                        }
                    }              
                }

                similar_circle_exists = true;
                break;
            }
        }
        if (!similar_circle_exists) {
            accepted_circles.push_back( std::make_pair(Circle, 1) );
        }

        n_valid_circles++;
        if (n_valid_circles >= N_CIRCLES_GENERATED) {
            break;
        }
    }

    /// remove each circle with less than N_CIRCLES_CUTOFF support
    std::vector<CIRCLE_U_PAIR>::iterator it = accepted_circles.begin();
    std::vector<CIRCLE_U_PAIR>::iterator end_it = accepted_circles.end();
    for (; it != end_it; ++it) {
        if (it->second < N_CIRCLES_CUTOFF) {
            accepted_circles.erase(it);
        } 
    }

    /// sort the circles by their counts
    std::sort (accepted_circles.begin(), accepted_circles.end(), m_circle_has_greater_count);

    /*for (unsigned i = 0; i < accepted_circles.size(); i++) {
        MV_CIRCLE circle = accepted_circles[i].first;
        printf ("(x=%d, y=%d, R=%f)  c=%d\n", circle.x,circle.y,circle.rad,accepted_circles[i].second);
    }*/

    bin_findcircles.stop();
}

void mvAdvancedCircles::drawOntoImage (IplImage* img) {
    std::vector<CIRCLE_U_PAIR>::iterator iter = accepted_circles.begin();
    std::vector<CIRCLE_U_PAIR>::iterator end_iter = accepted_circles.end();
    for (; iter != end_iter; ++iter) {
        cvCircle (img, cvPoint(iter->first.x,iter->first.y), iter->first.rad, CV_RGB(100,100,100), CIRCLE_THICKNESS);        
    }
}

int mvAdvancedCircles::ncircles() {
    return (int)(accepted_circles.size());
}

int mvAdvancedCircles::get_circle_from_3_points (CvPoint p1, CvPoint p2, CvPoint p3, MV_CIRCLE &Circle) {
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

int mvAdvancedCircles::check_circle_validity (IplImage* img, MV_CIRCLE Circle) {
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
        if(*(ptr) != 0){
            count++;
        }
    }

    return count;
}
