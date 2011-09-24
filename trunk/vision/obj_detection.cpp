#include "obj_detection.h"

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
                    CvPoint2D32f* &corners, int* corner_count,//def 4 
                    double eigenThreshold, double min_dist,
                    int block_size)
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

// NAME: areaQuad
//
// PURPOSE: given 4 points in x,y coords, calculates the area of the quadrilateral formed by them.
//      This is used as a distance metric in the lines clustering algorithm.
//      
// ARGUMENTS: x1,y1 and etc are the points, which are the input.
//      Returns the area.
//
// METHOD: take any 3 of the 4 points. They form a triangle. The area of this triangle can be
//      found via cross products. Repeat this for every unique combination of 3 points among the 4.
//      Then divide to find the area of the quadrilateral.
float areaQuad (float x1,float y1, float x2,float y2, float x3,float y3, float x4,float y4) {
    float a123 = (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1);   // this is parallelogram area not triangle
    float a134 = (x3-x1)*(y4-y1) - (x4-x1)*(y3-y1);
    float a124 = (x2-x1)*(y4-y1) - (x4-x1)*(y2-y1);
    float a234 = (x3-x2)*(y4-y2) - (x4-x2)*(y3-y2);
    
    a123=ABS(a123); a134=ABS(a134); a124=ABS(a124); a234=ABS(a234);
    return (a123+a134+a124+a234)/8.0; // not divide by 4.0
}

/** SDmatrix routines */
    // NAME: createSDMatrix (create seed distance matrix)
    //
    // PURPOSE: given an array of cluster seeds (cseed) and array of lines (lines), calculates a
    //      matrix whose elements SDmat[i][j] measures the difference (distance) between the ith
    //      seed and jth line. This is used in the lines clustering algorithm.
    //
    // ARGUMENTS: SDmat - points to output matrix
    //          cseed - X by 2 array holding seeds. cseed[i][0], cseed[i][1] are endpoints of ith seed
    //          nseeds - number of seeds
    //          lines - output of cvHoughLines2, probabilistic mode.
    //          nlines - number of lines. Can be accessed thru lines->total
    //
    // METHOD: Simply steps thru each combionation of seed and line and uses areaQuad to measure difference
    void createSDMatrix (float** &SDmat, CvPoint** cseed, int nseeds, CvSeq* lines, int nlines) {
        SDmat = new float*[nseeds]; // allocate the matrx
        for (int i = 0; i < nseeds; i++) SDmat[i] = new float[nlines];

        CvPoint* temp1, * temp2;
        for (int i = 0; i < nseeds; i++) 
            for (int j = 0; j < nlines; j++) {
                temp1 = cseed[i];                           // read seed data into CvPoint*
                temp2 = (CvPoint*)cvGetSeqElem(lines, j);   // read sequence data into CvPoint*
                // now calculate difference between the seed and line
                SDmat[i][j] = areaQuad(temp1[0].x,temp1[0].y, temp1[1].x,temp1[1].y
                                        ,temp2[0].x,temp2[0].y, temp2[1].x,temp2[1].y);
            }
    }

    void destroySDMatrix (float** SDmat, int nseeds) {
    // simple destructor for SDmatrix
        for (int i = 0; i < nseeds; i++) {
            
            delete[] SDmat[i];
            
        }
        delete[] SDmat;
    }
/** END SDmatrix routines */

/** Clusterseed routines */
    // NAME: addClusterSeed
    //
    // PURPOSE: 
    void addClusterSeed (CvPoint** &cseed, int &nseeds, CvSeq* lines, int nlines) {
    // looks at seed array (which is a CvPoint[i][2], with each i holding the 2 endpoints of
    // the ith seed line. Picks out
        if (nseeds == 0) { // no seeds yet. Just take strongest line as seed
            cseed = new CvPoint*[N_CLUSTERS_MAX]; // allocate 10 x 2 matrix. Allows up to 10 cluster seeds
            for (int i = 0; i < N_CLUSTERS_MAX; i++) cseed[i] = new CvPoint[2];
            
            CvPoint* temp = (CvPoint*) cvGetSeqElem(lines,0); // get the strongest line
            cseed[0][0].x = temp[0].x;   cseed[0][0].y = temp[0].y; // set the seed to that line
            cseed[0][1].x = temp[1].x;   cseed[0][1].y = temp[1].y;
            nseeds = 1;
        }
        else {
            float** SDmat;
            createSDMatrix (SDmat, cseed, nseeds, lines, nlines); // create SDmatrix
                
            // now look for the line whose minimum diff from any seed is the greatest (aka the line
            // is most different from any of the seeds)
            int newSeedIndex = 0; float newSeedDiff = 0;
            float Diffi;
            for (int j = 0; j < nlines; j++) { // look at each line
                Diffi = 1E12;   
                for (int i = 0; i < nseeds; i++) // get the minimum difference between that line and a seed
                    if (Diffi > SDmat[i][j]) 
                        Diffi = SDmat[i][j];

                if (Diffi > newSeedDiff) { // if ith line is more different than newSeedIndex line
                    newSeedDiff = Diffi;
                    newSeedIndex = j;
                }
            }

            CvPoint* temp = (CvPoint*) cvGetSeqElem(lines,newSeedIndex); 
            cseed[nseeds][0].x = temp[0].x;   cseed[nseeds][0].y = temp[0].y; // set the seed to that line
            cseed[nseeds][1].x = temp[1].x;   cseed[nseeds][1].y = temp[1].y;
            
            destroySDMatrix (SDmat, nseeds);
            nseeds++;
        }
    }

    void destroyClusters (CvPoint** cseed) {
        for (int i = 0; i < N_CLUSTERS_MAX; i++) delete[] cseed[i];
        delete[] cseed;
    }
/** END Clusterseed routines */


float KMcluster (CvPoint** &cseed, int nseeds, CvSeq* lines, int nlines, const int kmeans_iterations) {
// cseed is uninitialized pointer. Will be initialized in function
// nseeds is specified number of clusters
/** generate cseed matrix */
    int temp = 0; // placeholder for number of seeds, not used
    for (int i = 0; i < nseeds+1; i++) 
        addClusterSeed (cseed, temp, lines, nlines); 
    
 /** perform clustering */
    float average_intra;        // avg intra cluster distance, avged amongst all clusters
    float total_intra_1;        // avg intra cluster distance, for one cluster
    // create arrays to hold line indices, and SDmatrix
    int indexArray[nseeds][nlines];  // indexArray[i] is array of lines that belong to cluster i
    int indexArraySize[nseeds];      // size of indexArray[i], size of each cluster
    float** SDmat;

/** loop for each clustering iteration */
    for (int km_iter = 0; km_iter < kmeans_iterations; km_iter++) { // for each iteration
        createSDMatrix (SDmat, cseed, nseeds, lines, nlines); // create SDmatrix
        // step thru each line, accumulate to closest seed
        
        for (int i = 0; i < nseeds; i++) indexArraySize[i] = 0;
        for (int i = 0; i < nlines; i++) {
            int nearestSeed=0; float nearestSeedDiff = 1e8;
            for (int s = 0; s < nseeds; s++) // find closest seed to ith line
                if (nearestSeedDiff > SDmat[s][i]) {
                    nearestSeedDiff = SDmat[s][i];
                    nearestSeed = s;
                }

            indexArray[nearestSeed][indexArraySize[nearestSeed]] = i; // add line to that cluster
            indexArraySize[nearestSeed]++;                  // increment cluster size
        }
        
        average_intra = 0;

        for (int s = 0; s < nseeds; s++) {
            total_intra_1 = 0;
            
            float x1=0,y1=0,x2=0,y2=0; // cluster mean
            float len, weight, totalweight=0;
            
            for (int i = 0; i < indexArraySize[s]; i++) { // step thru lines in cluster s
                CvPoint* temp = (CvPoint*)cvGetSeqElem(lines, indexArray[s][i]);   
                
                // do a weighted average of lines in cluster s. Weight = line length^2
                float dy = temp[1].y-temp[0].y, dx = temp[1].x-temp[0].x;
                len = sqrt(dy*dy + dx*dx);
                
                if (SDmat[s][indexArray[s][i]]/len > 300) weight = 0.0;
                else weight = len + 0.01;               
                
                x1+=temp[0].x * weight;      y1+=temp[0].y * weight;
                x2+=temp[1].x * weight;      y2+=temp[1].y * weight;
                totalweight += weight; // number of lines in this cluster 
                
                total_intra_1 += SDmat[s][indexArray[s][i]]; 
            }       
            cseed[s][0].x=x1/totalweight; cseed[s][0].y=y1/totalweight; // average of cluster is new seed
            cseed[s][1].x=x2/totalweight; cseed[s][1].y=y2/totalweight;
            
            average_intra += total_intra_1 / indexArraySize[s]; // add (total intra cluster dist) / (cluster size)
        }
        destroySDMatrix (SDmat, nseeds);
    } // end clustering
    
/** calculate validity score of clusters */ 
    float validity;
    float min_inter;                // minimum inter cluster distance
    if (nseeds == 1) {
        // Denotes min dist needed between 2 clusters of equal validity to justify 2 clusters over 1
        // currently uses Length of Cluster Seed * 20
        int dx = cseed[0][1].x-cseed[0][0].x,  dy = cseed[0][1].y-cseed[0][0].y;
        min_inter = 10.0 * sqrt(dx*dx + dy*dy);      // the 10 is arbitrary and can be adjusted
    }
    else {
        min_inter = 1E12;
        for (int s = 0; s < nseeds; s++) // loop over all combinations of clusters and find the min intercluster dist
            for (int s2 = s+1; s2 < nseeds; s2++) {
                float temp_dist = areaQuad (cseed[s][0].x,cseed[s][0].y,cseed[s][1].x,cseed[s][1].y,
                                            cseed[s2][0].x,cseed[s2][0].y,cseed[s2][1].x,cseed[s2][1].y);
                if (temp_dist < min_inter) min_inter = temp_dist;
            }
    }
    validity = average_intra / min_inter; // validity is defined as intracluster dist / min intercluster dist
    printf ("Clusters: %d  Validity: %f = %f / %f\n", nseeds, validity, average_intra, min_inter);
    return validity;
}

void KMcluster_auto_K (CvPoint** &cseed, int &nseeds, int K_MIN, int K_MAX, 
                       CvSeq* lines, int nlines, int kmeans_iterations) {   
/* tries every value of nseeds between K_min and K_max, returns the clustered set with the lowest
 * validity number */
    int n = K_MAX-K_MIN+1;
    CvPoint **clusters=0, **min_valid_clusters=0;      // stores the clusted sets
    float valid, min_valid = 1E12;   // to find the minimum validity and the associated clustered set
    
    for (int i = 0; i < n; i++) { // for each possible value of nseeds
        // perform clustering with KMIN + i clusters
        valid = KMcluster (clusters, K_MIN+i, lines, nlines, kmeans_iterations);
        
        if (valid < min_valid) { // if validity is smaller than the min, swap min with current clusters
            nseeds = K_MIN+i;
            min_valid = valid;
            if (min_valid_clusters != 0) destroyClusters (min_valid_clusters);
            min_valid_clusters = clusters;
        }
        else
            destroyClusters (clusters);
    }    
    cseed = min_valid_clusters;
    printf ("  Final: %d clusters, %f validity\n", nseeds, min_valid);
}

/*

void KMcluster (CvPoint** cseed, int nseeds, CvSeq* lines, int nlines, int iterations) {
    // create arrays to hold line indices, and SDmatrix
    int indexArray[nseeds][nlines];  // indexArray[i] is array of lines that belong to cluster i
    int indexArraySize[nseeds];      // size of indexArray[i]
    for (int i = 0; i < nseeds; i++) indexArraySize[i] = 0;

    float** SDmat;
    createSDMatrix (SDmat, cseed, nseeds, lines, nlines); // create SDmatrix
    
    // step thru each line, accumulate to closest seed
    for (int i = 0; i < nlines; i++) {
        int nearestSeed=0; float nearestSeedDiff = 1e8;
        for (int s = 0; s < nseeds; s++) // find closest seed to ith line
            if (nearestSeedDiff > SDmat[s][i]) {
                nearestSeedDiff = SDmat[s][i];
                nearestSeed = s;
            }

        indexArray[nearestSeed][indexArraySize[nearestSeed]] = i; // add line to that cluster
        indexArraySize[nearestSeed]++;                  // increment cluster size
    }
                
    // average each cluster to form new seeds // this loop can be removed for faster
    for (int s = 0; s < nseeds; s++) {
        float x1=0,y1=0,x2=0,y2=0; // cluster mean
        float len, weight, totalweight=0;
        
        for (int i = 0; i < indexArraySize[s]; i++) { // step thru lines in cluster s
            CvPoint* temp = (CvPoint*)cvGetSeqElem(lines, indexArray[s][i]);   
            
            // do a weighted average of lines in cluster s. Weight = line length^2
            float dy = temp[1].y-temp[0].y, dx = temp[1].x-temp[0].x;
            len = sqrt(dy*dy + dx*dx);
            
            if (SDmat[s][i]/len > 300) weight = 0.0;
            else weight = len + 0.01;               
            
            x1+=temp[0].x * weight;      y1+=temp[0].y * weight;
            x2+=temp[1].x * weight;      y2+=temp[1].y * weight;
            totalweight += weight; // number of lines in this cluster            
//printf ("%d %d %d %d\n", temp[0].x,temp[0].y,temp[1].x,temp[1].y);
        }
//printf ("\n");
        
        cseed[s][0].x=x1/totalweight; cseed[s][0].y=y1/totalweight; // average of cluster is new seed
        cseed[s][1].x=x2/totalweight; cseed[s][1].y=y2/totalweight;
    }
    destroySDMatrix (SDmat, nseeds);
     
    iterations--;
    if (iterations > 0) // call recursively if needed
        KMcluster (cseed, nseeds, lines, nlines, iterations);
}

*/
