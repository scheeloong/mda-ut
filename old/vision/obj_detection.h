#ifndef OBJECT_DETECTION_
#define OBJECT_DETECTION_

#include <cv.h>
#include <highgui.h>

#define N_CLUSTERS_MAX 20
#define _DISPLAY 1
#define _QUIET 4

// holds functions to identify and track objects

// PURPOSE: Finds corners in the image. Outputs them in an array of CvPoint2D32f
//      Will not output 2 corners closer than min_dist
void cvCornerST_Custom (IplImage* img, IplImage* eigenImg,
                    CvPoint2D32f* &corners, int* corner_count=NULL,//def 4 
                    double eigenThreshold=0.1, double min_dist=20,
                    int block_size=7);

void lines_binning (CvMat* lines, int nlines, CvMat* &out, int nout, // input line matrix, nlines, output line mat 
                       int rbounds[], float abounds[], // min/max bound for rad and ang
                       int radbins, int angbins);         // num of bins

float KMcluster (CvPoint** &cseed, int nseeds, CvSeq* lines, int nlines, const int iterations=1, char flags=0);
float KMcluster_auto_K (CvPoint** &cseed, int &nseeds, int K_MIN, int K_MAX, 
                       CvSeq* lines, int nlines, const int kmeans_iterations=1, char flags=0);
void addClusterSeed (CvPoint** &cseed, int &nseeds, CvSeq* lines, int nlines);
void createSDMatrix (float** &SDmat, CvPoint** cseed, int nseeds, CvSeq* lines, int nlines);
float areaQuad (float x1,float y1, float x2,float y2, float x3,float y3, float x4,float y4);
int lineSegment_intersects (CvPoint** lines, int nlines, CvSize imgSize, CvPoint* &points);
    
#endif
