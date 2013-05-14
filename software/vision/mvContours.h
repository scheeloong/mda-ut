#ifndef __MDA_VISION_MVCONTOURS__
#define __MDA_VISION_MVCONTOURS__

#include <cv.h>
#include "mv.h"
#include "mgui.h"

class mvContours {
    CvMemStorage* m_storage;
    CvSeq* m_contours;

private:
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
    void get_ellipse_parameters (IplImage* img, CvSeq* contour1, CvPoint &centroid, float &angle);
    float match_contour (CvSeq* contour1, CvSeq* contour2);
    void draw_contours (CvSeq* contours_to_draw, IplImage* img) {
        cvDrawContours (
                    img,
                    contours_to_draw,
                    cvScalar(200,200,200),  // contour color
                    cvScalar(0,0,0),        // background color
                    0                       // max contours level
            );
    }

public:
    mvContours ();
    ~mvContours ();

    bool find_rectangle (IplImage* img, CvPoint &centroid, float &angle);

    void drawOntoImage (IplImage* img) { draw_contours (m_contours, img); }        

};

#endif