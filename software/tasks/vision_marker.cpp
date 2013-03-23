#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

#define ABS(X) (((X)>0) ? (X) : (-(X)))

const char MDA_VISION_MODULE_MARKER::MDA_VISION_MARKER_SETTINGS[] = "vision_marker_settings.csv";

/// ########################################################################
/// MODULE_MARKER methods
/// ########################################################################
MDA_VISION_MODULE_MARKER:: MDA_VISION_MODULE_MARKER () :
	_window (mvWindow("Marker Vision Module")),
	_HSVFilter (mvHSVFilter(MDA_VISION_MARKER_SETTINGS))
{
    _filtered_img = mvGetScratchImage();
    //_filtered_img->origin = 1;
}

MDA_VISION_MODULE_MARKER:: ~MDA_VISION_MODULE_MARKER () {
    mvReleaseScratchImage();
}

void MDA_VISION_MODULE_MARKER:: primary_filter (IplImage* src) {   
    /** src is an image that is passed in from the simulator. It is 3 channel
     *  Because it is const you may need to deep copy (not pointer copy) it 
     *  to your own IplImage first before you can modify it.
     *  I've done this with the variable img
     */

    /** YOUR CODE HERE. DO STUFF TO img */

    _HSVFilter.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;

    // this line displays the img in a window
    _window.showImage (_filtered_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_MARKER:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = NO_TARGET;
    double target[2][7] = {
                           {0.256755, 0.001068, 0.006488, 0.001148, 0.000003, 0.000037, -0.000000}, //the plane
                           {0.256755, 0.001068, 0.006488, 0.001148, 0.000003, 0.000037, -0.000000}
                          };
    
    double hu[7];
    mvHuMoments(_filtered_img, hu);

    double dist = 0;

    for (int j = 0; j < 2; ++j)
    {
        printf("{");
        for (int i = 0; i < 7; ++i)
        {
            printf("%lf, ", hu[i]);
            dist += fabs(hu[i] - target[j][i]);
        }
        printf("\b\b}: %lf\n",dist);
    
        if (dist < HU_THRESH){
            retval = FULL_DETECT;
            printf("Match found!! :)\n");
        }
        dist = 0;
    }

    return retval;
}
