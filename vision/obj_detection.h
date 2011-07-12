#ifndef OBJECT_DETECTION_
#define OBJECT_DETECTION_

// holds functions to identify and track objects

// NAME: cvCornerST_Custom
//
// PURPOSE: Finds corners in the image. Outputs them in an array of CvPoint2D32f
//      Will not output 2 corners closer than min_dist
//
// ARGUMENTS: 
//      img - 1 channel. treated as binary (every nonzero pix is the same)
//      eigenImg - pass a pointer and the function returns the eigenvalues at each
//              pixel. Set to NULL if dont want to.
//      corners - output array of corners. Sorted in decreasing eigenvalue. 
//      corner_count - will not find more than this many corners. Function will overwrite
//              this with actual number of corners found.
//      eigenThreshold - minimum relative eigenvalue to consitute a corner. (relative eVal is
//              eVal at a pixel / largest lower eVal in image).
//      min_dist - will not output 2 corners closer than this
//      block_size kernel size for the sobel derivative filter (1,3,5,7 only)
//
// METHOD: Calls cvGoodFeaturesToTrack. Thats about it. The principle has to do with applying
//      a matrix transform to region around each pixel, and calculating eigenvalue of the result.
//      Bigger eigenvalue = stron derivative in both x and y = a corner.
//
void cvCornerST_Custom (IplImage* img, IplImage* eigenImg,
                    CvPoint2D32f* &corners, int* corner_count=NULL,//def 4 
                    double eigenThreshold=0.1, double min_dist=20,
                    int block_size=7)
{
    //assert (img != NULL);
    //assert (img->nChannels == 1);
    
    int noreturn = 0, nocount = 0;
    if (eigenImg == NULL) // do not return the eigenImage if it was NULL'd
        noreturn = 1;
    
    IplImage* temp = cvCreateImage ( // image for scratch space
        cvSize(img->width, img->height),
        img->depth, 1);
    eigenImg = cvCreateImage ( // scratch space, contains eVals after
        cvSize(img->width, img->height),
        img->depth, 1);
        
    if (corner_count == NULL) { // if user didnt specify corner_count
        nocount = 1;
        corner_count = new int; // allocate and set to 4
        *corner_count = 4;      
    }
    
    corners = new CvPoint2D32f[*corner_count]; // allocate space for corners

    // call the function. Some features like use_harris is removed
    cvGoodFeaturesToTrack(img, eigenImg, temp,
                corners, corner_count, eigenThreshold, min_dist, NULL,
                block_size);
           
    if (noreturn) cvReleaseImage (&eigenImg);
    if (nocount) delete corner_count;
    cvReleaseImage (&temp);
}
/*
void detect_LLane (IplImage* img, CvPoint* &endPoints, int flags=0) {
    assert (img != NULL);
    
    CvMat* lines = cvCreateMat (3, 1, CV_32SC4); // 3 elem array of 4chan *cough* signed integers
    
    cvHoughLines2(img,
        lines,
        CV_HOUGH_PROBABILISTIC,     // looks for line segments, not just lines
        1,              // res in R in pixels
        CV_PI/128.0,     // res in theta in rad
        10,             // pixel threshold to make a line
        int (img->height/16), // minimum length of line segment 
        int (img->height/16)  // min distance between parallel lines        
    );
    
    endPoints = new CvPoint[6];                 // a pair of points for each line segment

    double* dataPtr = (double*)(lines->data.db);  // points to an array with the line data
    for (int i = 0; i < 3; i++) {                   // transfer the line endpoints to CvPoint structures
        endPoints[2*i] = cvPoint(dataPtr[4*i],dataPtr[4*i+1]);    // x,y of line starting point
        endPoints[2*i+1] = cvPoint(dataPtr[4*i+2],dataPtr[4*i+3]);  // x,y of line end point
    }    
    
    if (flags == 0) {
        IplImage* img_Line = cvCreateImage ( // create second image with 1 channel
            cvSize(img->width, img->height),
            IPL_DEPTH_8U,
            img->nChannels);
        
        for (int i = 0; i < 3; i++)                      // draw lines on the image
            cvLine(img, endPoints[2*i], endPoints[2*i+1], cvScalar (100,100,0,0));
        
        cvNamedWindow ("Detect_LLane", CV_WINDOW_AUTOSIZE);
        cvShowImage("Detect_LLane", img_Line);
        cvWaitKey(0);
        cvDestroyWindow ("Detect_LLane");
        cvReleaseImage (&img_Line);
    }
}
*/


#endif
