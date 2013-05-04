#include "mda_vision.h"

#define ABS(X) (((X)>0) ? (X) : (-(X)))

const char MDA_VISION_MODULE_TEST::MDA_VISION_TEST_SETTINGS[] = "vision_test_settings.csv";

/// ########################################################################
/// MODULE_TEST methods
/// ########################################################################
MDA_VISION_MODULE_TEST:: MDA_VISION_MODULE_TEST () :
	window (mvWindow("Test Vision Module")),
	HSVFilter (mvHSVFilter(MDA_VISION_TEST_SETTINGS)),
	AdvancedColorFilter (MDA_VISION_TEST_SETTINGS),
	HoughLines (mvHoughLines(MDA_VISION_TEST_SETTINGS)),
	lines (mvLines()),
    histogram_filter ("vision_gate_settings.csv"),
    bin_test ("Test Module")
{
    color_img = mvCreateImage_Color();
    filtered_img = mvGetScratchImage();
    //_filtered_img->origin = 1;
}

MDA_VISION_MODULE_TEST:: ~MDA_VISION_MODULE_TEST () {
    mvReleaseScratchImage();
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

    AdvancedColorFilter.mean_shift(src, color_img);
    //_HSVFilter.filter (src, filtered_img);
    filtered_img->origin = src->origin;

    bin_test.stop();

    // this line displays the img in a window
    window.showImage (color_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_TEST:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = NO_TARGET;
    return retval;
}
