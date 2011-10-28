#include "highgui.h"
int main( int argc, char** argv ) {
cvNamedWindow( "Example2", CV_WINDOW_AUTOSIZE );

CvCapture* capture = cvCreateCameraCapture(0) ;
IplImage* frame = cvQueryFrame( capture );

CvVideoWriter * vid1 = cvCreateVideoWriter (
    "webcam1.avi", 
    CV_FOURCC('P','I','M','1'), 
    25, 
    cvGetSize(frame),
    1 );

while(1) {
    frame = cvQueryFrame( capture );
    if( !frame ) break;
    
    cvWriteFrame( vid1, frame );
    cvShowImage( "Example2", frame );
    
    char c = cvWaitKey(20);
    if( c == 'q' ) break;

}
cvReleaseCapture( &capture );
cvDestroyWindow( "Example2" );
cvReleaseVideoWriter (&vid1);
return 0;
}
