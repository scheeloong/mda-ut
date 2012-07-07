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
    char TEST_SETTINGS[] = "settings/test_settings.mda";
    unsigned width, height;
    
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    printf ("Image Size: %dx%d\n", width,height);
    
    mvWindow win1("win1");
    
    mvGradient gradient (TEST_SETTINGS);
    
    IplImage * temp = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    IplImage * img = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
    cvResize (temp, img);
    
    win1.showImage (img);
    
    IplImage * res = cvCreateImage (cvGetSize(img), IPL_DEPTH_8U, 1); 
    gradient.filter (img, res);
    
    
    
    cvWaitKey(0);
    return 0;
}
