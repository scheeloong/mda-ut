#include "highgui.h"

int main( int argc, char** argv ) {
    cvNamedWindow( "webcam1", CV_WINDOW_AUTOSIZE );

    CvCapture* capture = cvCreateCameraCapture(0) ;    // create a webcam video capture
    IplImage* frame = cvQueryFrame( capture );         // read a single frame from the cam

    CvVideoWriter * vid1 = cvCreateVideoWriter (       // create a video file to store video
        "webcam1.avi",              // name of the video file
        CV_FOURCC('P','I','M','1'), // video codec (don't worry about this one)
        25,                         // framerate that gets stored along with the video
        cvGetSize(frame),           // the resolution. 
        1);                         // 1 here means color video, 0 means not color

    while(1) {                      // play the video like before     
        frame = cvQueryFrame( capture );
        if( !frame ) break;
    
        cvWriteFrame( vid1, frame );      // write the frame to the video writer
        cvShowImage( "webcam1", frame );
    
        char c = cvWaitKey(40);
        if( c == 'q' ) break;
    }

    cvReleaseCapture( &capture );
    cvDestroyWindow( "webcam1" );
    cvReleaseVideoWriter (&vid1);         // finishes writing to file 
}
