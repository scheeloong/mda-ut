#ifndef __MDA_VISION_MVCONTOURS__
#define __MDA_VISION_MVCONTOURS__

#include <cv.h>
#include "mv.h"
#include "mgui.h"

typedef std::vector<double> HU_MOMENTS;
static const int CONTOURS_MATCH_NORMAL = 0;
static const int CONTOURS_MATCH_RECIP = 1;
static const int CONTOURS_MATCH_FRACTION = 2;

class mvContours {
    // list of images that contain objects to match against
    static const int NUM_CONTOUR_RECT_IMAGES = 5;
    static const char* contour_rect_images[];
    std::vector<HU_MOMENTS> hu_moments_rect_vector;

    static const int NUM_CONTOUR_CIRC_IMAGES = 2;
    static const char* contour_circ_images[];
    std::vector<HU_MOMENTS> hu_moments_circ_vector;

    CvMemStorage* m_storage;
    CvSeq* m_contours;

    PROFILE_BIN bin_contours;
    PROFILE_BIN bin_match;
    PROFILE_BIN bin_calc;

private:
    // approximates a rounded contour with a polygon
    CvSeq* approx_poly (CvSeq* contour_to_approx, int accuracy) { 
        return cvApproxPoly(
                contour_to_approx,
                sizeof(CvContour),
                m_storage,
                CV_POLY_APPROX_DP,
                accuracy,               // accuracy in pixels
                0                       // contour level. 0 = first only, 1 = all contours level with and below the first
            ); 
    }

    void find_contour_and_check_errors (IplImage* img);
    void get_rect_parameters (IplImage* img, CvSeq* contour1, CvPoint &centroid, float &length, float &angle);
    void get_circle_parameters (IplImage* img, CvSeq* contour1, CvPoint &centroid, float &radius);

    void get_hu_moments (CvSeq* contour1, HU_MOMENTS& hu_moments);
    
    // initialize the database of hu moments for a given shape database
    void init_contour_template_database (const char** image_database_vector, int num_images, std::vector<HU_MOMENTS> &output_moments);

    // match the hu moments of the contour against all the ones in hu_moments_vector
    void match_contour_with_database (CvSeq* contour1, int &best_match_index, double &best_match_diff, 
                                    int method, std::vector<HU_MOMENTS> hu_moments_vector);

    void draw_contours (CvSeq* contours_to_draw, IplImage* img) {
        cvDrawContours (
                    img,
                    contours_to_draw,
                    cvScalar(200,200,200),  // contour color
                    cvScalar(100,100,100),        // background color
                    0                       // max contours level
            );
    }

public:
    mvContours ();
    ~mvContours ();

    double match_rectangle (IplImage* img, CvPoint &centroid, float &length, float &angle, int method=CONTOURS_MATCH_RECIP);
    double match_circle (IplImage* img, CvPoint &centroid, float &radius, int method=CONTOURS_MATCH_RECIP);

    void drawOntoImage (IplImage* img) { draw_contours (m_contours, img); }        

};

#endif
