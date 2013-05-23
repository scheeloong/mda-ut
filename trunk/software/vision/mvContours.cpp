#include "mvContours.h"

//#define MATCH_CONTOURS_DEBUG

#define CONTOUR_IMG_PREFIX "../vision/contour_images/"

const char* mvContours::contour_rect_images[] = {
    CONTOUR_IMG_PREFIX "Rect01.png",
    CONTOUR_IMG_PREFIX "Rect02.png",
    CONTOUR_IMG_PREFIX "Rect03.png",
    CONTOUR_IMG_PREFIX "Rect04.png",
    CONTOUR_IMG_PREFIX "Rect05.png",
};

const char* mvContours::contour_circ_images[] = {
    CONTOUR_IMG_PREFIX "circ01.png",
    CONTOUR_IMG_PREFIX "circ02.png",
    CONTOUR_IMG_PREFIX "circ03.png",
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

    init_contour_template_database (contour_circ_images, NUM_CONTOUR_CIRC_IMAGES, hu_moments_circ_vector);
    init_contour_template_database (contour_rect_images, NUM_CONTOUR_RECT_IMAGES, hu_moments_rect_vector);

    // return used memory to storage
    cvClearMemStorage(m_storage);
}

mvContours::~mvContours() {
    cvReleaseMemStorage (&m_storage);
}

void mvContours::init_contour_template_database (const char** image_database_vector, int num_images, std::vector<HU_MOMENTS> &output_moments) {
    // iterate in reverse dir because we push back onto the output_moments
    for (int i = num_images-1; i >= 0; i--) {
        IplImage* img = cvLoadImage(image_database_vector[i], CV_LOAD_IMAGE_GRAYSCALE);
        cvFindContours (img, m_storage, &m_contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        if (m_contours == NULL || m_contours->total == 0) {
            printf ("ERROR: No contours found when loading contour_image %s\n", image_database_vector[i]);
            exit(1);
        }
        else {
            // get the hu moments and add it to the vector
            HU_MOMENTS h;
            get_hu_moments (m_contours, h);
            output_moments.push_back(h);
            // clear sequence for next loop
            cvClearSeq(m_contours);
        }
        cvReleaseImage(&img);
    }
}

void mvContours::get_ellipse_parameters_for_rect (IplImage* img, CvSeq* contour1, CvPoint &centroid, float &length, float &angle) {
    assert (contour1->total > 6); // needed by cvFitEllipse2

    CvBox2D ellipse = cvFitEllipse2(contour1);
    angle = ellipse.angle;
    if (static_cast<int>(angle) > 180) angle -= 180;

    if (static_cast<int>(angle) > 90) angle -= 180;
    else if (static_cast<int>(angle) < -90) angle += 180;

    // RZ - I think this can be removed after a bit more testing
    CvMoments mom;
    cvMoments(contour1, &mom);

    int x = static_cast<int>(mom.m10/mom.m00);
    int y = static_cast<int>(mom.m01/mom.m00);
    centroid.x = x - img->width*0.5;
    centroid.y = y - img->height*0.5;
    length = 0.6 * ellipse.size.height;

    printf("Angle is: %5.2f\n", angle);
    printf("Alt Center is: (%d, %d)\n", x, y);

    // draw a line to indicate the angle
    CvPoint p0, p1;
    int delta_x = length/2 * -sin(angle*CV_PI/180.f);
    int delta_y = length/2 * cos(angle*CV_PI/180.f);
    p0.x = x - delta_x;  p0.y = y - delta_y;
    p1.x = x + delta_x;  p1.y = y + delta_y;
    cvLine (img, p0, p1, CV_RGB(50,50,50), 2);
}

void mvContours::get_circle_parameters (IplImage* img, CvSeq* contour1, CvPoint &centroid, float radius) {
    assert (contour1->total > 3);

    CvMoments mom;
    cvMoments (contour1, &mom);
    int x = static_cast<int>(mom.m10/mom.m00);
    int y = static_cast<int>(mom.m01/mom.m00);
    centroid.x = x - img->width*0.5;
    centroid.y = y - img->height*0.5;
    
    radius = static_cast<float>(std::sqrt(mom.m00 / CV_PI)); // m00 is just the area
    cvCircle (img, cvPoint(x,y), static_cast<int>(radius), CV_RGB(50,50,50), 2);
}

void mvContours::get_hu_moments (CvSeq* contour1, HU_MOMENTS &hu_moments) {
    CvMoments moments;
    double hus[7];

    cvContourMoments(contour1, &moments);
    cvGetHuMoments(&moments, (CvHuMoments*)hus);
    hu_moments = std::vector<double>(hus, hus+sizeof(hus)/sizeof(double));
}

void mvContours::match_contour_with_database (CvSeq* contour1, int &best_match_index, double &best_match_diff, int method, std::vector<HU_MOMENTS> hu_moments_vector) {
    HU_MOMENTS hus_to_match;
    get_hu_moments (contour1, hus_to_match);
    best_match_diff = 1000000;

#ifdef MATCH_CONTOURS_DEBUG
    printf ("Matching Contours:\n");
#endif
    for (unsigned i = 0; i < hu_moments_vector.size(); i++) {
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

double mvContours::match_rectangle (IplImage* img, CvPoint &centroid, float &length, float &angle, int method) {
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

    // do matching to ensure the contour is a rectangle
    int match_index;
    double best_match_diff;
    bin_match.start();
    match_contour_with_database (m_contours, match_index, best_match_diff, method, hu_moments_rect_vector);
    bin_match.stop();

    // get the mathematical properties we want
    bin_calc.start();
    get_ellipse_parameters_for_rect (img, m_contours, centroid, length, angle);
    bin_calc.stop();

    cvClearSeq(m_contours);
    return best_match_diff;
}

double mvContours::match_circle (IplImage* img, CvPoint &centroid, float &radius, int method) {
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
    
    if (m_contours == NULL || m_contours->total <= 3) {
        printf ("match_circle: contour too short, returning.\n");
        return -1;
    }

    drawOntoImage (img);

    // check the contour's area to make sure it isnt too small
    double area = cvContourArea(m_contours);
    if (area < img->width*img->height/1000) {
        printf ("match_circle: contour area too small, returning.\n");
        return -1;
    }

    // do some kind of matching to ensure the contour is a circle
    int match_index;
    double best_match_diff;
    bin_match.start();
    match_contour_with_database (m_contours, match_index, best_match_diff, method, hu_moments_circ_vector);
    bin_match.stop();

    // get the mathematical properties we want
    bin_calc.start();
    get_circle_parameters (img, m_contours, centroid, radius);
    bin_calc.stop();

    cvClearSeq(m_contours);
    return best_match_diff;
}
