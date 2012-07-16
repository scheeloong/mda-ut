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
#include "mvlines.h"

int main (int argc, char** argv) {
    assert (argc == 2);

    unsigned width, height;
    
    read_common_mv_setting ("IMG_WIDTH_COMMON", width);
    read_common_mv_setting ("IMG_HEIGHT_COMMON", height);
    printf ("Image Size: %dx%d\n", width,height);
    
    mvWindow win1("win1");
    mvWindow win2("win2");
    mvWindow win3("win3");
    
    //mvGradient gradient ("settings/test_settings.csv");
    mvHSVFilter HSVFilter ("settings/HSVFilter_settings.csv");

    IplImage * temp = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
    IplImage * img = mvCreateImage_Color (width, height);
    cvResize (temp, img);
    win1.showImage (img);
    
    IplImage * res = mvCreateImage (img); // res is greyscale 
    HSVFilter.filter (img, res);
    win2.showImage (res);
    
    mvLines lines;
    mvHoughLines HL ("settings/HoughLines_settings.csv");
    
    printf ("Before line finding: nlines = %d\n", lines.nlines());
    HL.findLines (res, &lines);
    
    printf ("After line finding: nlines = %d\n", lines.nlines());
    //lines.drawOntoImage (res);
    for (unsigned i = 0; i < lines.nlines(); i++) { 
        cvLine (res, lines[i][0], lines[i][1], CV_RGB(50,50,50), 1);
    }
    win3.showImage(res);
    
    cvWaitKey(0);
    return 0;
}
