#include "mvLines.h"
#include "TriangularMatrix.h"
#include <cv.h>

//#define CLUSTERING_DEBUG

#define ABS(X) (((X) > 0) ? (X) : (-(X)))
/** Helper Functions */
inline
unsigned Line_Difference_Metric (int x1,int y1, int x2,int y2, int x3,int y3, int x4,int y4) {
// this is 8x the area of the quadrilateral
    int a123 = (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1);
    int a134 = (x3-x1)*(y4-y1) - (x4-x1)*(y3-y1);
    int a124 = (x2-x1)*(y4-y1) - (x4-x1)*(y2-y1);
    int a234 = (x3-x2)*(y4-y2) - (x4-x2)*(y3-y2);
    
    return ABS(a123) + ABS(a134) + ABS(a124) + ABS(a234);
}

inline 
unsigned Line_Difference_Metric (CvPoint line1[], CvPoint line2[]) {
// overloaded version used for two CvPoint*'s
    return Line_Difference_Metric (
        line1[0].x, line1[0].y,   line1[1].x, line1[1].y,
        line2[0].x, line2[0].y,   line2[1].x, line2[1].y
    );
}

inline
unsigned mvKMeans:: Get_Line_Cluster_Diff (unsigned cluster_index, unsigned line_index) {
// this function first reads the Cluster_Line_Diff value cached in the matrix, if it is -1
// that means the value wasnt calculated. So we calculate and store the value
    int cached_value = (*_Cluster_Line_Diff_Matrix)(cluster_index, line_index);
    if (cached_value >= 0)
        return cached_value;
    
    unsigned new_value = Line_Difference_Metric (_Clusters[cluster_index], (*_Lines)[line_index]);
    _Cluster_Line_Diff_Matrix->set(cluster_index,line_index, new_value);
    return new_value;
}

/** True methods of KMeans Algorithm */

mvKMeans:: mvKMeans () {
    _Cluster_Line_Diff_Matrix = new Matrix<int> (MAX_CLUSTERS, 20);
    _Lines = NULL;
    _nLines = 0;
    _nClusters = 0;
}

void mvKMeans:: KMeans_Init (unsigned n_clusters, mvLines* lines) {
// creates the required matrix, copied pointers, etc
    _Lines = lines; 
    _nClusters = n_clusters;
    _nLines = _Lines->nlines();
    assert (_nClusters <= MAX_CLUSTERS);
    
    _Lines->sortXY (); // I am not entirely sure we need this
    
    for (unsigned i = 0; i < _nClusters; i++)
        _Clusters[i] = NULL;
    
    if (_Cluster_Line_Diff_Matrix->getCols() < _nLines) {
        delete _Cluster_Line_Diff_Matrix;
        _Cluster_Line_Diff_Matrix = new Matrix<int> (MAX_CLUSTERS, _nLines);
    }
    _Cluster_Line_Diff_Matrix->setAll (-1);
}

void mvKMeans:: KMeans_CreateStartingClusters () {
/* creates N starting clusters for the KMeans algorithm. A "cluster" really means the
 * start/endpoints of the line that other lines can bin to. The clusters are chosen thus:
 * The first cluster is just the strongest line (line[0] from HoughLines)
 * The n+1'th cluster is the line which is most different from existing clusters. This is
 * calculated as the line whose minimum difference wrt existing clusters is the greatest.
 */   
    printf ("_nLines = %d\n", _nLines);
        
    if (_nClusters >= _nLines) { /// this code should not be here
        printf ("Not enough lines.\n");
        return;
    }

    /// choose first cluster, then 2nd, etc in this loop
    for (unsigned N = 0; N < _nClusters; N++) {  
        _Clusters[N] = new CvPoint[2];       // allocate starting and ending point
        
        printf ("Choosing cluster %d\n", N);
        
        /// For first cluster choose line[0];
        if (N == 0) {
            // I cant believe this is what happens when you pass in pointers like you are supposed to
            _Clusters[N] = (*_Lines)[0];     // pointer copy 
            continue;
        }
        
        
        /// For other clusters, loop over each line and find the line 
        /// whose min diff from existing clusters is the greatest among all lines
        unsigned next_cluster_index = 0;       
        unsigned next_cluster_min_diff = 0;
        
        for (unsigned line_index = 0; line_index < _nLines; line_index++) { 
            unsigned min_diff_from_cluster = 1E8;       // init this to a huge number
            
            // loop to find min_diff_from_cluster
            for (unsigned cluster_index = 0; cluster_index < N; cluster_index++) {   
                unsigned diff_from_cluster_i = Get_Line_Cluster_Diff (cluster_index, line_index);
                
                printf ("      Cluster %d - line %d: diff = %d\n", cluster_index,line_index,diff_from_cluster_i);
                
                if (diff_from_cluster_i < min_diff_from_cluster)
                    min_diff_from_cluster = diff_from_cluster_i;
            }
            
            if (min_diff_from_cluster > next_cluster_min_diff) {
                next_cluster_min_diff = min_diff_from_cluster;
                next_cluster_index = line_index;
            }
            
            printf ("  Examining line %d. next_cluster_min_diff=%d. next_cluster_index=%d\n", 
                    line_index,next_cluster_min_diff,next_cluster_index);;
        }
        
        /// now copy the data for the line chosen to be the next cluster
        printf ("Using line %d for cluster %d\n", next_cluster_index, N);
        _Clusters[N] = (*_Lines)[next_cluster_index];     // pointer copy starting point
    }
}

void mvKMeans:: KMeans_Cleanup () {
}

void mvKMeans:: drawClustersOntoImage (IplImage* img) {
    assert (img != NULL);
    assert (_Clusters != NULL);
    assert (img->nChannels == 1);
    
    for (unsigned i = 0; i < _nClusters; i++)
        if (_Clusters[i] != NULL)
            cvLine (img, _Clusters[i][0],_Clusters[i][1], CV_RGB(50,50,50), 2*LINE_THICKNESS);
}