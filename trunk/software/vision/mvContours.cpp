#include "mvContours.h"

//#define MATCH_CONTOURS_DEBUG
//#define M_DEBUG
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

#define CONTOUR_IMG_PREFIX "../vision/contour_images/"
#define MEAN2(a,b) ((fabs((a) + (b)))/2.0)

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
    /*CONTOUR_IMG_PREFIX "circ03.png", dont use the half circle for now*/
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

void mvContours::get_rect_parameters (IplImage* img, CvSeq* contour1, CvPoint &centroid, float &length, float &angle) {
    assert (contour1->total > 6); // needed by cvFitEllipse2

    //CvBox2D Rect = cvFitEllipse2(contour1);
    CvBox2D Rect = cvMinAreaRect2(contour1, m_storage);
    angle = Rect.angle;
    int height = Rect.size.height;
    int width = Rect.size.width;

    // depending on which is the long side we assign the angle differently    
    if (height > width) {
        length = height;
    } else {
        length = width;
        angle += 90;
    }

    int x = Rect.center.x;
    int y = Rect.center.y;
    centroid.x = x - img->width*0.5;
    centroid.y = -y + img->height*0.5; // the centroid y is measured to the bottom of the image

    // draw a line to indicate the angle
    CvPoint p0, p1;
    int delta_x = length/2 * -sin(angle*CV_PI/180.f);
    int delta_y = length/2 * cos(angle*CV_PI/180.f);
    p0.x = x - delta_x;  p0.y = y - delta_y;
    p1.x = x + delta_x;  p1.y = y + delta_y;
    cvLine (img, p0, p1, CV_RGB(50,50,50), 2);
}

void mvContours::get_circle_parameters (IplImage* img, CvSeq* contour1, CvPoint &centroid, float &radius) {
    assert (contour1->total > 3);

    CvPoint2D32f centroid32f;
    cvMinEnclosingCircle(contour1, &centroid32f, &radius);
    int x = static_cast<int>(centroid32f.x);
    int y = static_cast<int>(centroid32f.y);

    if (radius > img->height/2) {
        radius = -1;
        return;
    }

    centroid.x = x - img->width*0.5;
    centroid.y = y - img->height*0.5;
    
    cvCircle (img, cvPoint(x,y), static_cast<int>(radius), CV_RGB(50,50,50), 2);
}

void mvContours::get_hu_moments (CvSeq* contour1, HU_MOMENTS &hu_moments) {
    CvMoments moments;
    double hus[7];

    cvContourMoments(contour1, &moments);
    cvGetHuMoments(&moments, (CvHuMoments*)hus);
    hu_moments = std::vector<double>(hus, hus+sizeof(hus)/sizeof(double));
}

void mvContours::find_contour_and_check_errors(IplImage* img) {
    if (m_contours != NULL) {
        cvClearSeq(m_contours);
        m_contours = NULL;
    }

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

    int last_x=-1, last_y=-1;
    double area;
    if (m_contours == NULL || m_contours->total <= 6) {
        DEBUG_PRINT ("find_countour: contour too short, returning.\n");
        goto FIND_CONTOUR_ERROR;
    }
    
    // check the contour's area to make sure it isnt too small
    area = cvContourArea(m_contours);
    if (area < img->width*img->height/600) {
        DEBUG_PRINT ("find_countour: contour area too small, returning.\n");
        goto FIND_CONTOUR_ERROR;
    }

    // check that the contour does not coincide with the sides of the image for more than 20% of its perimeter
    for (int i = 0; i < m_contours->total; i++) {
        CvPoint* p = CV_GET_SEQ_ELEM (CvPoint, m_contours, i);
        if (p->x == last_x && abs(p->y-last_y) > img->height/3) {
            DEBUG_PRINT ("find_contour: contour shares vertical side with image. Discarding.\n");
            goto FIND_CONTOUR_ERROR;
        }
        if (p->y == last_y && abs(p->x-last_x) > img->width/3) {
            DEBUG_PRINT ("find_contour: contour shares horizontal side with image. Discarding.\n");
            goto FIND_CONTOUR_ERROR;
        }

        last_x = p->x;
        last_y = p->y;
    }
    bin_contours.stop();
    return;

    FIND_CONTOUR_ERROR:
    if (m_contours != NULL) {
        cvClearSeq(m_contours);
        m_contours = NULL;
    }
    bin_contours.stop();
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

    DEBUG_PRINT ("Best Match Diff = %9.6lf\n", best_match_diff);
}

int mvContours::match_rectangle (IplImage* img, MvRBoxVector* RBoxVector, int method) {
    assert (img != NULL);
    assert (img->nChannels == 1);

    find_contour_and_check_errors (img);
    if (m_contours == NULL)
        return -1;

    bin_calc.start();

    double perimeter = cvArcLength (m_contours, CV_WHOLE_SEQ, 1);
    double area = cvContourArea (m_contours);

    CvBox2D Rect = cvMinAreaRect2(m_contours, m_storage);
    float angle = Rect.angle;
    int length = Rect.size.height;
    int width = Rect.size.width;
    // depending on which is the long side we assign the sides and angle differently    
    if (length < width) {
        length = Rect.size.width;
        width = Rect.size.height;
        angle += 90;
    }

    double perimeter_ratio = perimeter / (2*length+2*width);
    double area_ratio = area / (length*width);
    printf ("Rect: Area / Peri:    %6.2lf / %6.2lf\n", area_ratio, perimeter_ratio);
    if (area_ratio < 0.6 || perimeter_ratio > 1.1) {
        return -1;
    }

    MvRotatedBox box;
    box.center.x = Rect.center.x;
    box.center.y = Rect.center.y;
    box.length = length;
    box.width = width;
    box.angle = angle;
    RBoxVector->push_back(box);

    // draw a line to indicate the angle
    /*CvPoint p0, p1;
    int delta_x = length/2 * -sin(angle*CV_PI/180.f);
    int delta_y = length/2 * cos(angle*CV_PI/180.f);
    p0.x = x - delta_x;  p0.y = y - delta_y;
    p1.x = x + delta_x;  p1.y = y + delta_y;
    cvLine (img, p0, p1, CV_RGB(50,50,50), 2);
    */
    bin_calc.stop();

    return 1;
}

int mvContours::match_circle (IplImage* img, MvCircleVector* circle_vector, int method) {
    assert (img != NULL);
    assert (img->nChannels == 1);

    find_contour_and_check_errors(img);
    if (m_contours == NULL)
        return -1;

    bin_calc.start();

    // do some kind of matching to ensure the contour is a circle
    CvMoments moments;
    cvContourMoments (m_contours, &moments);
    cv::Moments cvmoments(moments);

    double nu11 = cvmoments.nu11;
    double nu20 = cvmoments.nu02;
    double nu02 = cvmoments.nu20;
    double nu21 = cvmoments.nu21;
    double nu12 = cvmoments.nu12;
    double nu03 = cvmoments.nu03;
    double nu30 = cvmoments.nu30;

    double r03 = fabs(nu30 / nu03);
    r03 = (r03 > 1) ? r03 : 1.0/r03;
    double r12 = fabs(nu12 / nu21);
    r12 = (r12 > 1) ? r12 : 1.0/r12;
    double r02 = fabs(nu02 / nu20);
    r02 = (r02 > 1) ? r02 : 1.0/r02;

    double r11 = fabs( MEAN2(nu02,nu20) / nu11);
    double R = MEAN2(nu20,nu02) / std::max((MEAN2(nu21,nu12)), (MEAN2(nu30,nu03)));
    bool pass = (r03 <= 25.0) && (r12 <= 8.0) && (r02 <= 8.0) && (r11 > 2.5) && (R > 25);

    //printf ("Circle Contours: nu11=%lf, nu20=%lf, nu02=%lf, nu21=%lf, nu12=%lf, nu30=%lf, nu03=%lf\n",
    //    nu11, nu20, nu02, nu21, nu12, nu30, nu03);
    printf ("Circle Contours: \tn30/n03=%3.1lf, n21/n12=%3.1lf, nu20/nu02=%3.1lf, r11=%3.1f, R=%3.1f. %s\n", r03, r12, r02, r11, R, pass?"PASS!":"FAIL!");
    if (!pass) {
        return -1;
    }
    
    // get area and perimeter of the contour
    //double perimeter = cvArcLength (m_contours, CV_WHOLE_SEQ, 1);
    double area = cvContourArea (m_contours);

    // get min enclosing circle and radius
    CvPoint2D32f centroid32f;
    float radius;
    cvMinEnclosingCircle(m_contours, &centroid32f, &radius);
    if (radius > img->width/2 || radius < 0) {
        return -1;
    }

    // do checks on area and perimeter
    double area_ratio = area / (CV_PI*radius*radius);
    //double perimeter_ratio = perimeter / (2*CV_PI*radius);
    //printf ("Circle: Area/Peri:    %6.2lf / %6.2lf\n", area_ratio, perimeter_ratio);
    if (area_ratio < 0.7) {
        return -1;
    }
    
    MvCircle circle;
    circle.center.x = static_cast<int>(centroid32f.x);
    circle.center.y = static_cast<int>(centroid32f.y);

    circle.radius = radius;
    circle_vector->push_back(circle);
    
    //cvCircle (img, cvPoint(x,y), static_cast<int>(radius), CV_RGB(50,50,50), 2);

    bin_calc.stop();

    return 1;
}
