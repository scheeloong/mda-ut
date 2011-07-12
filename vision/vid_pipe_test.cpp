#include <cv.h>
#include <highgui.h>

#define DIM_RATIO 1.5
#define GRAD_ROWS 5  // kernal rows for gradient
#define GRAD_COLS 5

main( int argc, char* argv[] ) {

    CvCapture* capture = NULL;
    capture = cvCreateCameraCapture(0); // start getting video

    IplImage *img = cvQueryFrame(capture); // grad 1 frame to look at img size

   /* CvSize size = cvSize ( // get size
        (int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),
        (int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT)
    );
*/
cvNamedWindow( "Vid_Grey", CV_WINDOW_AUTOSIZE );
cvNamedWindow( "Vid_Grad", CV_WINDOW_AUTOSIZE );
cvNamedWindow( "Vid_Thresh", CV_WINDOW_AUTOSIZE );

    IplImage* img_Resize = cvCreateImage ( // create second image with diff size
        cvSize(img->width/DIM_RATIO, img->height/DIM_RATIO),
        IPL_DEPTH_8U,
        img->nChannels);
    IplImage* img_Grey= cvCreateImage ( // create second image with 1 channel
        cvSize(img_Resize->width, img_Resize->height),
        IPL_DEPTH_8U,
        1);
    IplImage* img_Grad = cvCreateImage ( // create second image with 1 channel
        cvSize(img_Resize->width, img_Resize->height),
        IPL_DEPTH_8U,
        1);
    IplImage* temp = cvCreateImage ( // create temp with 1 channel
        cvSize(img_Resize->width, img_Resize->height),
        IPL_DEPTH_8U,
        1);
    IplImage* img_Thresh = cvCreateImage ( // create second image with 1 channel
        cvSize(img_Grad->width, img_Grad->height),
        IPL_DEPTH_8U,
        1);
    CvMat* lines; 
    float* dataPtr;
    float degs;
    
while(1) {

    img = cvQueryFrame( capture );
    if(!img) break;
    
    cvResize (img, img_Resize, CV_INTER_LINEAR); // resize first img to second
   
// greyscale 
        
    cvConvertImage (img_Resize, img_Grey, 0); // convert to img_Grey
     
    cvShowImage("Vid_Grey", img_Grey );
// gradient
    
    cvMorphologyEx (img_Grey, //src
        img_Grad, // dst
        temp, // temp
        cvCreateStructuringElementEx (GRAD_ROWS, GRAD_COLS, (GRAD_ROWS+1)/2, (GRAD_COLS+1)/2, CV_SHAPE_RECT),
        CV_MOP_GRADIENT,
        1
    );   
    
    cvShowImage("Vid_Grad", img_Grad );
    
// threshold
    
    cvThreshold (img_Grad, img_Thresh, 
        25, // threshold
        255, // max_value that pix above thresh are set to
        CV_THRESH_BINARY//CV_THRESH_TOZERO
    ); 
    
    cvShowImage("Vid_Thresh", img_Thresh);

    
// Hough Line Transform: R = x*cosT + y*sinT gives normal to line from origin. R = rad to line, T = ang of normal
    
    lines = cvCreateMat (3, 1, CV_32FC2); // two channel 32 bit float

    cvHoughLines2(img_Thresh,
        lines,
        CV_HOUGH_STANDARD,
        5, // res in R value in pixels
        CV_PI/32.0, // res in theta in rad
        75, // pixel threshold to make a line
        0,0 // unused        
    );

    dataPtr = (float*)(lines->data.db);
    degs = *(dataPtr+1) * 180.0 / CV_PI;
    if (degs > 90) degs -= 90;    
    printf ("Angle: %f degrees\n", degs);
 
 char c = cvWaitKey(33);
   if( c == 27 ) break;
}

cvReleaseCapture ( &capture );

return 0;

}