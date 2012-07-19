/** The main function is just used to test various parts of vision. Rewrite
 *  as needed 
 */
#include <stdlib.h>
#include <stdio.h>
#include <highgui.h>
#include <cv.h>
//#include <highgui.h>

#include "mv.h"
#include "mgui.h"
#include "mvLines.h"

int main (int argc, char** argv) {
    // We want to do HSV color filter, take the gradient of result,
    // and run line finding on the gradient img
    assert (argc == 2);
    
    /// Grab the filters and data structs we need
    mvHSVFilter HSVFilter ("settings/HSVFilter_settings.csv"); // color filter
    mvGradient gradient ("settings/test_settings.csv");
    mvHoughLines HoughLines ("settings/HoughLines_settings.csv");
    mvLines lines; // data struct to store lines

    // windows to display stuff
    mvWindow win1("img");
    mvWindow win2("filtered");
    mvWindow win3("gradient & lines");
    
    unsigned width, height;
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    printf ("Image Size: %dx%d\n", width,height);
    
    // process the img
    IplImage * temp = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
    IplImage * img = mvCreateImage_Color (); // img will be common width and height
    cvResize (temp, img);
    win1.showImage (img);
    
    IplImage * res = mvCreateImage (img); // res is greyscale 
    HSVFilter.filter (img, res);
    win2.showImage (res);
    
    HoughLines.findLines (res, &lines);
    lines.drawOntoImage (res);
    /*for (unsigned i = 0; i < lines.nlines(); i++) { 
        cvLine (res, lines[i][0], lines[i][1], CV_RGB(50,50,50), 1);
    }*/
    win3.showImage(res);
    
    cvWaitKey(0);
    return 0;
}
