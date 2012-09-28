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
#define MAX_CLUSTERS 10
#define _MIN_DIST_BETWEEN_PARALLEL_LINES_MULTIPLIER_ 0.05
#define _OUTLIER_CUTOFF_MULTIPLIER 40

class mvKMeans {
    unsigned MIN_DIST_BETWEEN_PARALLEL_LINES;
    
    // The below _Clusters_**** are arrays of CvPoint[2]. Each CvPoint[2] which store the beginning 
    // and end points of a line. The lines are the "clusters" we are trying to get and each array
    // is an intermediate variable to get _Clusters_Best
    //
    // _Seed stores the starting cluster, which we use to bin the lines to. Note each element
    //      will be used to point to an existing line, so no need to allocate this
    // _Temp store the clusters and they grow during the binning process. 
    //
    // _Best stores the most suitable cluster configuration (based on nClusters). It will point
    //      to _Temp if _Temp is deemed the best so far.
    CvPoint* _Clusters_Seed[MAX_CLUSTERS];  
    CvPoint* _Clusters_Temp[MAX_CLUSTERS];
    CvPoint* _Clusters_Best[MAX_CLUSTERS];  
    
    mvLines* _Lines;                        // points to lines we are try to cluster
    unsigned _nLines;
    unsigned _nClusters_Final; // only used for storing the number of clusters after the algorithm
    
    private:    
    // helper functions
    unsigned Get_Line_ClusterSeed_Diff (unsigned cluster_index, unsigned line_index);
    
    // steps in the algorithm
    void KMeans_CreateStartingClusters (unsigned nClusters);
    float KMeans_Cluster (unsigned nClusters, unsigned iterations);
    
    
    public:
    mvKMeans ();
    ~mvKMeans ();

    unsigned nClusters () { return _nClusters_Final; }
    void printClusters ();

    int cluster_auto (unsigned nclusters_min, unsigned nclusters_max, mvLines* lines, unsigned iterations=1);
    //void clearData () { for (unsigned i = 0; i < MAX_CLUSTERS; i++) _Clusters_Best[i] = NULL; }
    void drawOntoImage (IplImage* img);
};

#endif