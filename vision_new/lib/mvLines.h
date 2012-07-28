/** mvlines - mda Line Finding algorithm library
 *  Author - Ritchie Zhao
 *  July 2012
 */ 
#ifndef __MDA_VISION_MVLINES__
#define __MDA_VISION_MVLINES__ 

#include <cv.h>

#define LINE_THICKNESS 1
    
/** mvLines - object representing a collection of lines using the two endpoints */
/* When created, the class automatically allocates 6400 bytes of memory that it uses to 
 * store data. This can be recycled by calling clearData(); This memory cannot be reclaimed
 * without destroying the class instance.
 * 
 * Use nlines() to get the number of lines currently stored.
 * 
 * To access line data: lines[i] returns a 2 element array of CvPoint* => the 2 endpoints.
 * So lines[i][0] and lines[i][1] are the endpoints of ith line.
 * Use lines[i][0].x and lines[i][0].y to get x and y coordinates of an endpoint. 
 * 
 * Use drawOntoImage (img) to draw the lines onto an image.
 */
class mvLines {
    CvSeq* _data;
    CvMemStorage* _storage; // this stores actual line data in opencv
    
    friend class mvHoughLines;
    friend class mvKmeans;
    
    public:
    // the constructor allocates 6400 bytes of storage space, which is like 400 lines...
    mvLines () { _data=NULL; _storage=cvCreateMemStorage(6400); } 
    ~mvLines () {} // opencv will clean up the memory used (i hope)
    
    unsigned nlines () { return (_data != NULL) ? unsigned(_data->total) : 0; }
    void removeHoriz ();
    void removeVert ();
    void sortXY (); // each Horiz line has smaller X value first, each Vert smaller Y first
    void drawOntoImage (IplImage* img);
    
    CvPoint* operator [] (unsigned index) { return (CvPoint*)cvGetSeqElem(_data,index); }
    
    // note clearData does NOT deallocate memory, it only allows recycling of used memory. 
    void clearData () { 
        if (_data) {
            cvClearSeq(_data); 
            _data=NULL; 
            cvClearMemStorage(_storage);
        }
    } 
};

/** mvHoughLines - Hough Line finding filter */
// you must provide an mvLines object for it to work 
class mvHoughLines {
    unsigned PIX_RESOLUTION;
    float ANG_RESOLUTION; // ang_resolution is in radians
    float _ACCUMULATOR_THRESHOLD_;
    float _MIN_LINE_LENGTH_, _MIN_COLINEAR_LINE_DIST_;
    
    public:
    mvHoughLines (const char* settings_file);
    /* use default destructor */
    void findLines (IplImage *img, mvLines* lines);
};



/** The following code deal with K-Means clustering */
#include "Matrix.h"
#define MAX_CLUSTERS 6

class mvKMeans {
    unsigned _nClusters;
    unsigned _nLines;
    //unsigned _iterations;
    
    // The below is an array of CvPoint* (maximum 10 elements). Each CvPoint* 
    // represents a line using starting and ending point.
    CvPoint* _Clusters[MAX_CLUSTERS];  
    mvLines* _Lines;
    Matrix<int>* _Cluster_Line_Diff_Matrix;
    
    private:    
    // helper functions
    unsigned Get_Line_Cluster_Diff (unsigned cluster_index, unsigned line_index);
    
    // steps in the algorithm
    void KMeans_Init (unsigned n_clusters, mvLines* lines);
    
    
    public:
    mvKMeans ();
    void init (unsigned n_clusters, mvLines* lines) { KMeans_Init (n_clusters, lines); }
    void KMeans_CreateStartingClusters ();
    void KMeans_Cleanup ();
    void drawClustersOntoImage (IplImage* img);
};

#endif
