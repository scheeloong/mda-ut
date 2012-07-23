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
unsigned Line_Difference_Metric (CvPoint p1[], CvPoint p2[]) {
// overloaded version used for two CvPoint*'s
    return Line_Difference_Metric (
        p1[0].x, p1[0].y,   p1[1].x, p1[1].y,
        p1[2].x, p1[2].y,   p1[3].x, p1[3].y
    );
}

inline
unsigned mvKMeans:: Get_Line_Cluster_Diff (unsigned cluster_index, unsigned line_index) {
// this function first reads the Cluster_Line_Diff value cached in the matrix, if it is -1
// that means the value wasnt calculated. So we calculate and store the value
    int cached_value = (*Cluster_Line_Diff_Matrix)(cluster_index, line_index);
    if (cached_value >= 0)
        return cached_value;
    
    unsigned new_value = Line_Difference_Metric (Clusters[cluster_index], (*Lines)[line_index]);
    Cluster_Line_Diff_Matrix->set(cluster_index,line_index, new_value);
    return new_value;
}

/** True methods of KMeans Algorithm */

mvKMeans:: mvKMeans () {
    Cluster_Line_Diff_Matrix = new Matrix<int> (MAX_CLUSTERS, 20);
    Lines = NULL;
    N_Lines = 0;
    N_Clusters = 0;
}

void mvKMeans:: KMeans_Init (unsigned _n_clusters, mvLines* _lines) {
// creates the required matrix, copied pointers, etc
    Lines = _lines; 
    N_Clusters = _n_clusters;
    N_Lines = Lines->nlines();
    assert (N_Clusters <= MAX_CLUSTERS);
    
    Lines->sortXY (); // I am not entirely sure we need this
    
    if (Cluster_Line_Diff_Matrix->getCols() < N_Lines) {
        delete Cluster_Line_Diff_Matrix;
        Cluster_Line_Diff_Matrix = new Matrix<int> (MAX_CLUSTERS, N_Lines);
    }
    Cluster_Line_Diff_Matrix->setAll (-1);
}

void mvKMeans:: KMeans_CreateStartingClusters () {
/* creates N starting clusters for the KMeans algorithm. A "cluster" really means the
 * start/endpoints of the line that other lines can bin to. The clusters are chosen thus:
 * The first cluster is just the strongest line (line[0] from HoughLines)
 * The n+1'th cluster is the line which is most different from existing clusters. This is
 * calculated as the line whose minimum difference wrt existing clusters is the greatest.
 */   
    printf ("N_Lines = %d\n", N_Lines);
    
    if (N_Clusters >= N_Lines) { /// this code should not be here
        for (unsigned i = 0; i < N_Lines; i++) {
            Clusters[i] = (*Lines)[i];
        }
        return;
    }

    /// choose first cluster, then 2nd, etc in this loop
    for (unsigned N = 0; N < N_Clusters; N++) {  
        Clusters[N] = new CvPoint[2];       // allocate starting and ending point
        
        printf ("Choosing cluster %d\n", N);
        
        /// For first cluster choose line[0];
        if (N == 0) {
            // I cant believe this is what happens when you pass in pointers like you are supposed to
            Clusters[N] = (*Lines)[0];     // pointer copy 
            continue;
        }
        
        
        /// For other clusters, loop over each line and find the line 
        /// whose min diff from existing clusters is the greatest among all lines
        unsigned next_cluster_index = 0;       
        unsigned next_cluster_min_diff = 0;
        
        for (unsigned line_index = 0; line_index < N_Lines; line_index++) { 
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
        Clusters[N] = (*Lines)[next_cluster_index];     // pointer copy starting point
    }
}

void mvKMeans:: drawClustersOntoImage (IplImage* img) {
    assert (img != NULL);
    assert (Clusters != NULL);
    assert (img->nChannels == 1);
    
    for (unsigned i = 0; i < N_Clusters; i++)
        cvLine (img, Clusters[i][0],Clusters[i][1], CV_RGB(50,50,50), 2*LINE_THICKNESS);
}