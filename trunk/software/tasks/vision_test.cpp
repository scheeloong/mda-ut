#include "mda_vision.h"

#define ABS(X) (((X)>0) ? (X) : (-(X)))
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

const char MDA_VISION_MODULE_TEST::MDA_VISION_TEST_SETTINGS[] = "vision_test_settings.csv";

/// ########################################################################
/// MODULE_TEST methods
/// ########################################################################
MDA_VISION_MODULE_TEST:: MDA_VISION_MODULE_TEST () :
	window (mvWindow("Test Vision Module")),
    window2 (mvWindow("Test Vision Module 2")),
	HSVFilter (mvHSVFilter(MDA_VISION_TEST_SETTINGS)),
	AdvancedColorFilter (MDA_VISION_TEST_SETTINGS),
	Contours (mvContours()),
    HoughLines (mvHoughLines(MDA_VISION_TEST_SETTINGS)),
	lines (mvLines()),
    histogram_filter ("vision_gate_settings.csv"),
    bin_test ("Test Module")
{
    color_img = mvCreateImage_Color();
    gray_img = mvCreateImage();
    gray_img_2 = mvCreateImage();
    //_gray_img->origin = 1;
}

MDA_VISION_MODULE_TEST:: ~MDA_VISION_MODULE_TEST () {
    cvReleaseImage(&gray_img);
    cvReleaseImage(&gray_img_2);
    cvReleaseImage(&color_img);
}

void MDA_VISION_MODULE_TEST:: primary_filter (IplImage* src) {   
    /** src is an image that is passed in from the simulator. It is 3 channel
     *  Because it is const you may need to deep copy (not pointer copy) it 
     *  to your own IplImage first before you can modify it.
     *  I've done this with the variable img
     */

    /** YOUR CODE HERE. DO STUFF TO img */
    bin_test.start();
    
    // do the filter - easy!
    AdvancedColorFilter.watershed(src, gray_img);
    window.showImage (gray_img);

    // variables for color matching
    int seg = 0;
    const double COLOR_DIVISION_FACTOR = 180;
    COLOR_TRIPLE color;
    //COLOR_TRIPLE color_template (160,95,157,0);
    COLOR_TRIPLE color_template (155,120,60,0);

    // variables for shape matching
    CvPoint best_centroid;
    float best_angle;
    double best_diff = 1000000;
    
    // get each segment from the filter and try to match
    while ( AdvancedColorFilter.get_next_watershed_segment(gray_img_2, color) ) {
        printf ("\nSegment %d\n", ++seg);
        printf ("\tColor (%3d,%3d,%3d)\n", color.m1, color.m2, color.m3);

        // calculate color diff
        double color_diff = 0;//static_cast<double>(color.diff(color_template)) / COLOR_DIVISION_FACTOR;

        // calculate shape diff
        CvPoint centroid;
        float angle;
        double shape_diff = Contours.match_rectangle(gray_img_2, centroid, angle);
        if (shape_diff < 0) // i still dont know how this is possible
            continue;

        double diff = color_diff + shape_diff;
        //double diff = shape_diff;
        printf ("\tColor_Diff=%5.2f  Shape_Diff=%5.2f\n\tFinal_Diff=%5.2f\n", color_diff, shape_diff, diff);

        if (seg == 1 || diff < best_diff) {
            best_diff = diff;
            best_centroid = centroid;
            best_angle = angle;
            cvCopy (gray_img_2, gray_img);
        }
        
        window2.showImage(gray_img_2);
        //cvWaitKey(0);
    }

    bin_test.stop();

    // this line displays the img in a window
    window2.showImage (gray_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_TEST:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = NO_TARGET;
    return retval;
}
