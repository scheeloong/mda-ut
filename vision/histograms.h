#ifndef HISTOGRAMS_
#define HISTOGRAMS_

#define _DISPLAY 1

// NAME: displayHistogram2D
//
// PURPOSE: displays a histogram with squares. Intensity of each square is the bin value, 
//      normalized so highest bin = 255.
//
// ARGUMENTS:
//      hist - the histogram to display, Must be 2D
//      scale - how big each bin is in pixels
//
// METHOD:
//      creates appropriately sized image, then steps thru each bin and draws its sqaure
//      on the image. 
void displayHistogram2D (CvHistogram* hist, int scale=10) { 
// displays a 2D histogram by creating an image, drawing rectangles of varying intensity
// and displaying it
    // note that dim[0] is NOT WIDTH. it is height. This is different from, say, cvSize
    int height = hist->mat.dim[0].size, width = hist->mat.dim[1].size;

    IplImage* histImage = cvCreateImage ( // image to hold 
        cvSize (width*scale, height*scale),
        IPL_DEPTH_8U,
        1);
    cvZero (histImage);
    
    float max, binval;
    int intensity;
    cvGetMinMaxHistValue (hist, 0, &max, 0, 0);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            binval = cvQueryHistValue_2D(hist, i,j);
            intensity = cvRound (binval/max*255);

            cvRectangle (histImage,
                cvPoint (j*scale, i*scale), // x coord first
                cvPoint ((j+1)*scale, (i+1)*scale),
                CV_RGB(intensity,intensity,intensity),
                CV_FILLED);
        }
    }
 
    cvNamedWindow("Histogram", CV_WINDOW_AUTOSIZE);
    cvShowImage("Histogram", histImage);
    cvWaitKey(0);
    cvDestroyWindow("Histogram");
    cvReleaseImage(&histImage);
}

// NAME: histogram_HS
//
// PURPOSE: calculates a Hue-Saturation histogram. Hue is first dimension.
//
// ARGUMENTS:
//      img - the image. 3 channel. May be BGR or HSV
//      hist - the output histogram. It will be allocated and computed.
//      convert_HSV - set this to have function convert to HSV
//      h_bins, s_bins - number of bins in H and S
//      flags = bit1 = display the histogram
//
// METHOD:
//      seperates out the Hue and Sat planes with cvSplit, defines and calculates
//      histogram with cvCreateHist and cvCalcHist
void histogram_HS (IplImage* img, CvHistogram* &hist, int convert_HSV, 
                   int h_bins=90, int s_bins=50, int flags=0) {
    IplImage* img_Hue = cvCreateImage ( // image to store the Hue values
        cvSize(img->width, img->height),
        IPL_DEPTH_8U,
        1);
    IplImage* img_Sat = cvCreateImage ( // image to store the Saturation values
        cvSize(img->width, img->height),
        IPL_DEPTH_8U,
        1);
    
    if (convert_HSV)  // convert if needed
        cvCvtColor (img, img, CV_BGR2HSV);            
        
    cvSplit (img, img_Hue, img_Sat, NULL,NULL);   // extract Hue, Saturation
    IplImage* planes[] = {img_Hue, img_Sat};

    int size[] = {h_bins, s_bins};
    float h_range[] = {0, 180}, s_range[] = {0,255};
    float* ranges[] = {h_range, s_range};
    
    hist = cvCreateHist (2, //dims
        size, // num of bins per dims
        CV_HIST_ARRAY, // dense matrix
        ranges, // upper & lower bound for bins
        1); // uniform
    
    cvCalcHist (planes,
        hist,
        0,  // accumulate
        NULL); // possible boolean mask image
    
    if (flags & _DISPLAY)
        displayHistogram2D (hist);
}


#endif