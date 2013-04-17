#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

#define ABS(X) (((X)>0) ? (X) : (-(X)))

const char MDA_VISION_MODULE_TEST::MDA_VISION_TEST_SETTINGS[] = "vision_test_settings.csv";

/// ########################################################################
/// MODULE_TEST methods
/// ########################################################################
MDA_VISION_MODULE_TEST:: MDA_VISION_MODULE_TEST () :
	_window (mvWindow("Test Vision Module")),
	_HSVFilter (mvHSVFilter(MDA_VISION_TEST_SETTINGS)),
	_MeanShift (mvMeanShift(MDA_VISION_TEST_SETTINGS)),
	_HoughLines (mvHoughLines(MDA_VISION_TEST_SETTINGS)),
	_lines (mvLines()),
    _adaptive ("vision_gate_settings.csv"),
    bin_test ("Test Module")
{
    _color_img = mvCreateImage_Color();
    _filtered_img = mvGetScratchImage();
    //_filtered_img->origin = 1;
}

MDA_VISION_MODULE_TEST:: ~MDA_VISION_MODULE_TEST () {
    mvReleaseScratchImage();
    cvReleaseImage(&_color_img);
}

void MDA_VISION_MODULE_TEST:: primary_filter (IplImage* src) {   
    /** src is an image that is passed in from the simulator. It is 3 channel
     *  Because it is const you may need to deep copy (not pointer copy) it 
     *  to your own IplImage first before you can modify it.
     *  I've done this with the variable img
     */

    /** YOUR CODE HERE. DO STUFF TO img */
    bin_test.start();

    _MeanShift.mean_shift(src, _color_img);
    //_HSVFilter.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
 /*   
    double target[7] = {0.256755, 0.001068, 0.006488, 0.001148, 0.000003, 0.000037, -0.000000};//the plane

    double hu[7];
    mvHuMoments(_filtered_img, hu);

    double dist = 0;

    for (int i = 0; i < 7; ++i)
    {
        printf("%lf, ", hu[i]);
        dist += fabs(hu[i] - target[i]);
    }
    printf("\n");

    printf("%lf\n", dist);

    if (dist < 0.005){
        printf("it's a plane!\n");
    }

*/
    bin_test.stop();

    // this line displays the img in a window
    _window.showImage (_color_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_TEST:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = NO_TARGET;
    return retval;
}
