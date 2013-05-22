#include "mvContours.h"

//#define MATCH_CONTOURS_DEBUG

#define CONTOUR_IMG_PREFIX "../vision/contour_images/"

const char* mvContours::contour_rect_images[] = {
    CONTOUR_IMG_PREFIX "Rect01.png",
    CONTOUR_IMG_PREFIX "Rect02.png",
    CONTOUR_IMG_PREFIX "Rect03.png",
    CONTOUR_IMG_PREFIX "Rect04.png",
    CONTOUR_IMG_PREFIX "Rect05.png",
    CONTOUR_IMG_PREFIX "Rect06.png",
    CONTOUR_IMG_PREFIX "Rect07.png",
    CONTOUR_IMG_PREFIX "Rect08.png",
    CONTOUR_IMG_PREFIX "Rect09.png",
    CONTOUR_IMG_PREFIX "Rect10.png"
};

double dslog (double x) { // double signed log
    double sign = static_cast<double>(x>0) - static_cast<double>(x<0);
    return (sign * log(fabs(x)));
}

mvContours::mvContours() :
    bin_contours(PROFILE_BIN("mvContours - Contour Finding")),
    bin_match(PROFILE_BIN("mvContours - Matching")),
    bin_calc(PROFILE_BIN("mvContours - Calculation"))
{
    m_storage = cvCreateMemStorage(0);
    m_contours = NULL;

    // iterate in reverse dir because we push back onto the hu_moments_vector
    for (int i = NUM_CONTOUR_IMAGES-1; i >= 0; i--) {
        IplImage* img = cvLoadImage(contour_rect_images[i], CV_LOAD_IMAGE_GRAYSCALE);
        cvFindContours (
            img,
            m_storage,
            &m_contours,
            sizeof(CvContour),
            CV_RETR_EXTERNAL,
            CV_CHAIN_APPROX_SIMPLE
        );

        if (m_contours == NULL || m_contours->total == 0) {
            printf ("ERROR: No contours found when loading contour_image %s\n", contour_rect_images[i]);
            exit(1);
        }
        else {
            // get the hu moments and add it to the vector
            HU_MOMENTS h;
            get_hu_moments (m_contours, h);
            hu_moments_vector.push_back(h);
            // clear sequence for next loop
            cvClearSeq(m_contours);
        }
        cvReleaseImage(&img);
    }

    // return used memory to storage
    cvClearMemStorage(m_storage);
}

mvContours::~mvContours() {
    cvReleaseMemStorage (&m_storage);
}

void mvContours::get_ellipse_parameters (IplImage* img, CvSeq* contour1, CvPoint &centroid, float &angle) {
    assert (contour1->total > 6); // needed by cvFitEllipse2

    CvBox2D ellipse = cvFitEllipse2(contour1);
    angle = ellipse.angle;
    if (static_cast<int>(angle) > 180) angle -= 180;

    if (static_cast<int>(angle) > 90) angle -= 180;
    else if (static_cast<int>(angle) < -90) angle += 180;


    // RZ - I think this can be removed after a bit more testing
    float Area = cvContourArea(contour1);
    CvMoments mom;
    cvMoments(contour1, &mom);

    int x = static_cast<int>(mom.m10/mom.m00);
    int y = static_cast<int>(mom.m01/mom.m00);
    centroid.x = x - img->width*0.5;
    centroid.y = y - img->height*0.5;

    printf("Angle is: %5.2f\n", angle);
    printf("Alt Center is: (%d, %d)\n", x, y);

    // draw a line to indicate the angle
    CvPoint p0, p1;
    int delta_x = 100 * -sin(angle*CV_PI/180.f);
    int delta_y = 100 * cos(angle*CV_PI/180.f);
    p0.x = x - delta_x;  p0.y = y - delta_y;
    p1.x = x + delta_x;  p1.y = y + delta_y;
    cvLine (img, p0, p1, CV_RGB(50,50,50), 2);
    //printf("Area is: %5.2fpxsq\n",Area);
}

void mvContours::get_hu_moments (CvSeq* contour1, HU_MOMENTS &hu_moments) {
    CvMoments moments;
    double hus[7];

    cvContourMoments(contour1, &moments);
    cvGetHuMoments(&moments, (CvHuMoments*)hus);
    hu_moments = std::vector<double>(hus, hus+sizeof(hus)/sizeof(double));
}

void mvContours::match_contour_with_database (CvSeq* contour1, int &best_match_index, double &best_match_diff, int method) {
    HU_MOMENTS hus_to_match;
    get_hu_moments (contour1, hus_to_match);
    best_match_diff = 1000000;

#ifdef MATCH_CONTOURS_DEBUG
    printf ("Matching Contours:\n");
#endif
    for (int i = 0; i < NUM_CONTOUR_IMAGES; i++) {
        HU_MOMENTS hus_template = hu_moments_vector[i];
        double curr_diff = 0;

        // calculate curr_diff
        for (int j = 0; j < 7; j++) {
            double m1 = dslog(hus_to_match[j]);
            double m2 = dslog(hus_template[j]);
            if (isnan(m1))
                m1 = -100000000;
            if (isnan(m2))
                m2 = -100000000;

            if (method == CONTOURS_MATCH_NORMAL)
                curr_diff += fabs(m1 - m2);
            else if (method == CONTOURS_MATCH_RECIP)
                curr_diff += fabs(1.0/m1 - 1.0/m2);
            else if (method == CONTOURS_MATCH_RECIP)
                curr_diff += fabs((m1-m2)/m1);
        }
#ifdef MATCH_CONTOURS_DEBUG
        printf ("contour %d: %9.6lf\n", i, curr_diff);
#endif
        if (i == 0 || curr_diff < best_match_diff) {
            best_match_index = i;
            best_match_diff = curr_diff;
        }
    }

    printf ("Best Match Diff = %9.6lf\n", best_match_diff);
}

double mvContours::match_rectangle (IplImage* img, CvPoint &centroid, float &angle, int method) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    
    // find the contours
    bin_contours.start();
    cvFindContours (
        img,
        m_storage,
        &m_contours,
        sizeof(CvContour),
        CV_RETR_EXTERNAL,
        CV_CHAIN_APPROX_SIMPLE
    );
    bin_contours.stop();
    
    if (m_contours == NULL || m_contours->total <= 6) {
        printf ("match_rectangle: contour too short, returning.\n");
        return -1;
    }

    drawOntoImage (img);

    // check the contour's area to make sure it isnt too small
    double area = cvContourArea(m_contours);
    if (area < img->width*img->height/10000) {
        printf ("match_rectangle: contour area too small, returning.\n");
        return -1;
    }

    // do some kind of matching to ensure the contour is a rectangle
    int match_index;
    double best_match_diff;
    bin_match.start();
    match_contour_with_database (m_contours, match_index, best_match_diff, method);
    bin_match.stop();

    // get the mathematical properties we want
    bin_calc.start();
    get_ellipse_parameters (img, m_contours, centroid, angle);
    bin_calc.stop();

    cvClearSeq(m_contours);
    return best_match_diff;
}