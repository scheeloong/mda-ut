/** The main function is just used to test various parts of vision. Rewrite
 *  as needed 
 */
#include <stdlib.h>
#include <stdio.h>
#include <highgui.h>
#include <cv.h>

#include "mv.h"
#include "mgui.h"

int main (int argc, char** argv) {
    assert (argc == 2);

    unsigned width, height;
    
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    printf ("Image Size: %dx%d\n", width,height);
    
    mvWindow win1("win1");
    
    //mvGradient gradient ("settings/test_settings.mda");
    mvHSVFilter HSVFilter ("settings/HSVFilter_settings.mda");

    IplImage * temp = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
    IplImage * img = mvCreateImage_Color (width, height);
    cvResize (temp, img);
    
    win1.showImage (img);
    
    IplImage * res = mvCreateImage (img); // res is greyscale 
    HSVFilter.filter (img, res);
    
    cvWaitKey(0);
    return 0;
}
