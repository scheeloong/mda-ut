#include "mvLines.h"
#include <cv.h>

#define CREATE_STARTING_CLUSTERS_APPROX

#define KMEANS_ITERATIONS 1

#define CLUSTERING_DEBUG 1
#define DEBUG_MSG (str, ...) \
    if (CLUSTERING_DEBUG) \
        printf (str, ...); 

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
unsigned mvKMeans:: Get_Line_ClusterSeed_Diff (unsigned cluster_index, unsigned line_index) {
// this function first reads the Cluster_Line_Diff value cached in the matrix, if it is -1
// that means the value wasnt calculated. So we calculate and store the value
    int cached_value = (*_ClusterSeed_Line_Diff_Matrix)(cluster_index, line_index);
    if (cached_value >= 0)
        return cached_value;
    
    unsigned new_value = Line_Difference_Metric (_Clusters_Seed[cluster_index], (*_Lines)[line_index]);
    _ClusterSeed_Line_Diff_Matrix->set(cluster_index,line_index, new_value);
    return new_value;
}

/** True methods of KMeans Algorithm */

mvKMeans:: mvKMeans () {
    _ClusterSeed_Line_Diff_Matrix = new Matrix<int> (MAX_CLUSTERS, 20);
    
    for (unsigned i = 0; i < MAX_CLUSTERS; i++) {
        _Clusters_Seed[i] = NULL;
        _Clusters_Temp[i] = new CvPoint[2];
        _Clusters_Best[i] = NULL;
    }
    
    _Lines = NULL;
    _nLines = 0;
}

mvKMeans:: ~mvKMeans () {
    delete _ClusterSeed_Line_Diff_Matrix;
}

void mvKMeans:: KMeans_CreateStartingClusters (unsigned nClusters) {
/* creates N starting clusters for the KMeans algorithm. A "cluster" really means the
 * start/endpoints of the line that other lines can bin to. The clusters are chosen thus:
 * The first cluster is just the strongest line (line[0] from HoughLines)
 * The n+1'th cluster is the line which is most different from existing clusters. This is
 * calculated as the line whose minimum difference wrt existing clusters is the greatest.
 */   
    /// initialize all seed clusters to NULL
    for (unsigned i = 0; i < MAX_CLUSTERS; i++)
        _Clusters_Seed[i] = NULL;

    /// choose first seed cluster, then 2nd, etc in this loop
    for (unsigned N = 0; N < nClusters; N++) {  
        printf ("Choosing cluster %d\n", N);
        
        /// For first cluster choose line[0];
        if (N == 0) {
            // I cant believe this is what happens when you pass in pointers like you are supposed to
            _Clusters_Seed[N] = (*_Lines)[0];     // pointer copy 
            continue;
        }
        
        /// For other clusters, loop over each line and chose a line based on the method specified
        
        unsigned next_cluster_index = 0;       
        unsigned next_cluster_max_diff = 0;
        
        #ifdef CREATE_STARTING_CLUSTERS_APPROX   /// find the line most different from the last line chosen as a cluster
        
        for (unsigned line_index = 0; line_index < _nLines; line_index++) { 
            if (line_already_chosen[line_index] == true) continue;
            
            // calculate the diff between this line and last selected cluster
            unsigned diff_from_last_cluster = Get_Line_ClusterSeed_Diff (N-1, line_index);
            
            if (diff_from_last_cluster > next_cluster_max_diff) {
                next_cluster_max_diff = diff_from_last_cluster;
                next_cluster_index = line_index;
            }
        }
        
        #else                                            /// find the line whose min diff from any of the prev clusters is greatest
        
        for (unsigned line_index = 0; line_index < _nLines; line_index++) { 
            if (line_already_chosen[line_index] == true) continue;
            
            unsigned min_diff_from_cluster = 1E8;       // init this to a huge number
            // loop to find min_diff_from_cluster
            for (unsigned cluster_index = 0; cluster_index < N; cluster_index++) {   
                unsigned diff_from_cluster_i = Get_Line_ClusterSeed_Diff (cluster_index, line_index);
                printf ("      Cluster %d - line %d: diff = %d\n", cluster_index,line_index,diff_from_cluster_i);
                
                if (diff_from_cluster_i < min_diff_from_cluster)
                    min_diff_from_cluster = diff_from_cluster_i;
            }
            
            if (min_diff_from_cluster > next_cluster_max_diff) {
                next_cluster_max_diff = min_diff_from_cluster;
                next_cluster_index = line_index;
            }
            
            printf ("    Examining line %d. next_cluster_max_diff=%d. next_cluster_index=%d\n", 
                    line_index,next_cluster_max_diff,next_cluster_index);;
        }
        
        #endif
        
        /// now copy the data for the line chosen to be the next cluster
        printf ("  Using line %d for cluster %d\n", next_cluster_index, N);
        _Clusters_Seed[N] = (*_Lines)[next_cluster_index];     // pointer copy starting point
        line_already_chosen[next_cluster_index] = true;
    }
}

int mvKMeans:: KMeans_Cluster (unsigned nClusters, unsigned iterations) {
    
    
    for (unsigned iter = 0; iter < iterations; iter++) {
        
        /// loop over each line and bin that line to a cluster
        for (unsigned line_index = 0; line_index < _nLines; line_index++) {
            unsigned closest_cluster_index = 0;
            unsigned closest_cluster_diff = 1E9;
            unsigned cluster_diff_i;
            
            /// go thru each cluster and find the one that is closest (least diff) wrt this line
            for (unsigned cluster_index = 0; cluster_index < nClusters; cluster_index++) {
                if (iter == 0)  // for first iteration we can use cached diff values
                    cluster_diff_i = Get_Line_ClusterSeed_Diff (cluster_index, line_index);
                else            // otherwise we have to calculate the diff value
                    cluster_diff_i = Line_Difference_Metric (_Clusters_Seed[cluster_index], (*_Lines)[line_index]);
                
                if (cluster_diff_i < closest_cluster_diff) {
                    closest_cluster_diff = cluster_diff_i;
                    closest_cluster_index = cluster_index;
                }
            }
            
            /// add the line to the cluster
        }
        
    }
    
    return 0; 
}

int mvKMeans:: cluster_auto (unsigned nclusters_min, unsigned nclusters_max, mvLines* lines) {
    _Lines = lines; 
    _nLines = _Lines->nlines();
    assert (nclusters_min > 0 && nclusters_max <= MAX_CLUSTERS);
    assert (nclusters_min <= nclusters_max);
    
    /// if the nlines < nclusters then quit (with the clusters being NULL)
    printf ("_nLines = %d\n", _nLines);
    if (nclusters_min >= _nLines) { 
        printf ("Not enough lines.\n");
        return 1;
    }
    
    /// initialize line_already_chosen array
    line_already_chosen = new bool[_nLines];
    for (unsigned i = 0; i < _nLines; i++)
        line_already_chosen[i] = false;
    
    //_Lines->sortXY (); // I am not entirely sure we need this
    
    /// initialize the matrix cache
    unsigned cols = _ClusterSeed_Line_Diff_Matrix->getCols();    
    if (cols < _nLines || cols > 3*_nLines) {
        delete _ClusterSeed_Line_Diff_Matrix;
        _ClusterSeed_Line_Diff_Matrix = new Matrix<int> (MAX_CLUSTERS, _nLines);
    }
    _ClusterSeed_Line_Diff_Matrix->setAll (-1);
    
    /// we now run the algorithm
    for (unsigned N = nclusters_min; N <= nclusters_max; N++) {
        if (_nLines < N) {
            printf ("Cannot create %d clusters. Not enough lines.\n", N);
            return 1;
        }
        
        // create required num of clusters
        KMeans_CreateStartingClusters (N);
        
        // run the clustering algorithm through the desired num of iterations
        KMeans_Cluster (N, KMEANS_ITERATIONS);
        
        // check validity. If better than current validity pointer copy temp to best and
        // reallocate temp. Otherwise leave temp and it will be overwritten next iteration
        for (unsigned i = 0; i < MAX_CLUSTERS; i++)
            _Clusters_Best[i] = _Clusters_Seed[i];
    }
    
    /// cleanup
    delete [] line_already_chosen;
    return 0;
}

void mvKMeans:: drawOntoImage (IplImage* img) {
    assert (img != NULL);
    assert (img->nChannels == 1);
    
    for (unsigned i = 0; i < MAX_CLUSTERS; i++)
        if (_Clusters_Best[i] != NULL)
            cvLine (img, _Clusters_Best[i][0],_Clusters_Best[i][1], CV_RGB(50,50,50), 2*LINE_THICKNESS);
}
