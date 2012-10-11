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
#include "mvCircles.h"
#include "mda_vision.h"

int main (int argc, char** argv) {
    // We want to do HSV color filter, take the gradient of result,
    // and run line finding on the gradient img
    assert (argc == 2); // need image as first argument
    
    /// Grab the filters and data structs we need
    mvHSVFilter HSVFilter ("HSVFilter_settings.csv"); // color filter
    mvHoughLines HoughLines ("HoughLines_settings.csv");
    mvLines lines; // data struct to store lines
    mvHoughCircles HoughCircles ("HoughCircles_settings.csv");
    mvCircles circles;

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
    
    //HoughLines.findLines (res, &lines);
    //lines.drawOntoImage (res);
    
    HoughCircles.findCircles (res, &circles);
    //circles.drawOntoImage (res);
    for (unsigned i = 0; i < circles.ncircles(); i++) { 
        CvPoint center = cvPoint(circles[i].x, circles[i].y); // x and y coordinate of circle 
        cvCircle (res, center, circles[i].rad, CV_RGB(50,50,50), 2);
    }
    
    
    win3.showImage(res);
        
    cvWaitKey(0);
    
    return 0;
}
