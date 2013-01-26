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

void MDA_VISION_MODULE_TEST:: primary_filter (const IplImage* src) {   
    //_HSVFilter->filter (src, _filtered_img);

    /** src is an image that is passed in from the simulator. It is 3 channel
     *  Because it is const you may need to deep copy (not pointer copy) it 
     *  to your own IplImage first before you can modify it.
     *  I've done this with the variable img
     */

    /** YOUR CODE HERE. DO STUFF TO img */
    bin_test.start();

    //_adaptive.filter (src, _filtered_img);
    mvMeanShift(src, _color_img, 5, 15, 25, 30);

    bin_test.stop();

    // this line displays the img in a window
    _window.showImage (_color_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_TEST:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = NO_TARGET;
    return retval;
}
