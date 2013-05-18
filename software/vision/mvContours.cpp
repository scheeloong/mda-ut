#include "mvContours.h"

mvContours::mvContours() {
    m_storage = cvCreateMemStorage(0);
    m_contours = NULL;
}

mvContours::~mvContours() {
    cvReleaseMemStorage (&m_storage);
}

void mvContours::get_ellipse_parameters (IplImage* img, CvSeq* contour1, CvPoint &centroid, float &angle) {
    assert (contour1->total > 0);

    CvBox2D ellipse = cvFitEllipse2(contour1);
    //cvEllipseBox(img, ellipse, cvScalar(200,200,200));
    //centroid.x = ellipse.centroid.x - img->width/2;
    //centroid.y = ellipse.centroid.y - img->height/2
    angle = ellipse.angle;

    // RZ - I think this can be removed after a bit more testing
    float Area = cvContourArea(contour1);
    CvMoments mom;
    cvMoments(contour1, &mom);

    centroid.x = static_cast<int>(mom.m10/mom.m00) - img->width*0.5;
    centroid.y = static_cast<int>(mom.m01/mom.m00) - img->height*0.5;

    printf("Angle is: %5.2f\n", angle);
    printf("Alt Center is: (%d, %d)\n",centroid.x, centroid.y);
    printf("Area is: %5.2fpxsq\n",Area);
}

/*void mvContours::get_hu_moments (CvSeq* contour1, std::vector<int> &hu_moments) {

}*/

float mvContours::match_contour (CvSeq* contour1, CvSeq* contour2) {
    return 1;
}

bool mvContours::find_rectangle (IplImage* img, CvPoint &centroid, float &angle) {
    assert (img != NULL);
    assert (img->nChannels == 1);

    // find the contours
    cvFindContours (
        img,
        m_storage,
        &m_contours,
        sizeof(CvContour),
        CV_RETR_EXTERNAL,
        CV_CHAIN_APPROX_SIMPLE
    );

    if (m_contours == NULL || m_contours->total == 0) {
        return false;
    }

    drawOntoImage (img);

    // do some kind of matching to ensure the contour is a rectangle

    // get the mathematical properties we want
    get_ellipse_parameters (img, m_contours, centroid, angle);
    return true;
}