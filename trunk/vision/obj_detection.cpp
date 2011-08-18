#include <cstdio>

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
/*
int lines_accumulate (float rad[], float ang[], int nlines, // line values, nlines
                      float radt, float angt,               // target rad/ang
                      float &radf, float &angf, float diffThresh=30) { // final rad/ang
// looks thru list of ang and rad values. Finds the line with ang closest to angt (angle target).
// Then looks thru list again, picks out all lines similar to the line before (similarity measured thru
// abs(ang and rad) < diffThresh) and averages their rad and and values.
// radf and angf (final) will contain the averaged values
    float temp, diff = 20.0/0.175*abs(ang[0]-angt) + abs(rad[0]-radt);
    int it = 0;
    for (int i = 1; i < nlines; i++) {              // identify line closest to target angle value
        temp = 20.0/0.175*abs(ang[i]-angt) + abs(rad[i]-radt);
        if (temp < diff) { diff = temp;  it = i;}
    }
    
    if (diff > 1.5*diffThresh) return 0;              // if actual values too far from target
    radt = rad[it];     angt = ang[it];             // replace target with actual values
    
    int navg = 0;   
    radf = 0;  angf = 0;                            // stores the averaged line values
    for (int i = 0; i < nlines; i++) {              // now go thru lines and average similar ones
        // sum abs diff in rad and ang. And weighed so 10deg diff == 10pix diff
        diff = 10.0/0.175*abs(ang[i]-angt) + abs(rad[i]-radt); 
        if (diff < diffThresh) {
            radf += rad[i];  angf += ang[i];        // if similar accumulate line
            rad[i] = 5000;   ang[i] = 5000;
            navg++;
        }
    }
    
    radf /= navg;   angf /= navg;
    return navg;
}


void lines_binning (CvMat* lines, int nlines, CvMat* &out, int nout, // input line matrix, nlines, output line mat 
                       int rbounds[], float abounds[], // min/max bound for rad and ang
                       int radbins, int angbins)         // num of bins
// Unlinke line_accumulate, this thing is basically histograms for lines. It creates a 2D histogram
// with rad and ang as the variables. Then for each line, it classifies the line into a bin based on
// rad and ang.
// Finally, each bin's rad and ang values are averaged, resulting in a set of accumulated lines, which
// are sorted based on the number of lines used to create the average.
// The algorithm is to create a matrix of radbins x angbins, each bin stores a value, and a rad/ang.
// Run through the lines and put them into the right bin, then look thru the bins to find nonzero
// vals and sort them.
// FOR VERTICAL LINES set middle of abounds to 0, use EVEN angbins
// FOR HORIZONTAL LINES set middle to PI/2, use EVEN angbins
{
struct ltuple {     // line tuple
    int val;        // number of lines in this bins
    float rad, ang; // accumulated rad/ang values of the lines
};
    //float rstep = (float)(rbounds[1]-rbounds[0])/radbins;  
  //  float astep = (float)(abounds[1]-abounds[0])/angbins;
    float *dataPtr = (float*) (lines->data.ptr);
    float rad, ang;     // temp storage for r,a
    float rbin, abin;     // stores the bin indices
   
    ltuple lmat[radbins][angbins];  // histogram matrix
    memset (lmat, 0, radbins*angbins*sizeof(ltuple)); // zero everything
    
    // FOR EVERY LINE, ADD THAT LINE TO A BIN
    for (int i = 0; i < nlines; i++) { // for each line
        rad = *dataPtr++;   if (rad < 0) rad *= -1;     
        ang = *dataPtr++;   if (ang > CV_PI/2) ang -= CV_PI;
        // calculate which bin to put the line in
        rbin = (rad-rbounds[0])/(rbounds[1]-rbounds[0]) * (radbins-1);     
        abin = (ang-abounds[0])/(abounds[1]-abounds[0]) * (angbins-1); 
            
    //printf ("%f %f\n%f %f\n\n", rad, ang, rbin, abin);
//std::cin.get();
        if ((rbin >= 0) && (rbin < radbins) && (abin >= 0) && (abin < angbins)) {
            lmat[(int)rbin][(int)abin].val++;         // bin the line if it is within range
            lmat[(int)rbin][(int)abin].rad += rad;
            lmat[(int)rbin][(int)abin].ang += ang;
        }
    }
    
    // PUT THE NONZERO BINNED LINES INTO AN ARRAY
    int nfinal = 0;                      // final number of lines
    ltuple larray[radbins*angbins];      // array to hold the nonzero bins
    
    for (int i = 0; i < radbins; i++)    // for every bin
        for (int j = 0; j < angbins; j++)
            if (lmat[i][j].val > 0) {// if bin value > 0
                larray[nfinal].val = lmat[i][j].val;
                larray[nfinal].rad = lmat[i][j].rad / lmat[i][j].val;
                larray[nfinal].ang = lmat[i][j].ang / lmat[i][j].val;
                nfinal++;
            }
       
    // SORT THE ARRAY BY VAL
    float temp;
    for (int i = 0; i < nfinal-1; i++)    // bubble sort the arrays from largest
        for (int j = 0; j < nfinal-1-i; j++) // value of ABS(ang)
            if (larray[j].val < larray[j+1].val) {
                temp=larray[j].val; larray[j].val=larray[j+1].val; larray[j+1].val=temp;
                temp=larray[j].rad; larray[j].rad=larray[j+1].rad; larray[j+1].rad=temp;
                temp=larray[j].ang; larray[j].ang=larray[j+1].ang; larray[j+1].ang=temp;
            }
    
    for (int i = 0; i < nfinal; i++)
        printf ("%d %f %f\n", larray[i].val,larray[i].rad,larray[i].ang);
    
    // PUT DATA INTO CVMAT LINE STRUCTURE
    out = cvCreateMat (nout, 1, CV_32FC2);   // single column of 2 channel
    float* datPtr = (float*) (out->data.ptr);    
    for (int i = 0; i < nout; i++) {      
        *datPtr++ = larray[i].rad;
        *datPtr++ = larray[i].ang;
    }
}

*/

// NAME: areaQuad
//
// PURPOSE: given 4 points in x,y coords, calculates the area of the quadrilateral formed
//      
// ARGUMENTS: x1,y1 and etc are the points.
//      Returns the area
//
// METHOD: take any 3 of the 4 points. They form a triangle. The area of this triangle can be
//      found via cross products. Repeat this for every unique combination of 3 points among the 4.
//      Then divide to find the area of the quadrilateral.
float areaQuad (float x1,float y1, float x2,float y2, float x3,float y3, float x4,float y4) {
    float a123 = (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1);   // this is parallelogram area not triangle
    float a134 = (x3-x1)*(y4-y1) - (x4-x1)*(y3-y1);
    float a124 = (x2-x1)*(y4-y1) - (x4-x1)*(y2-y1);
    float a234 = (x3-x2)*(y4-y2) - (x4-x2)*(y3-y2);
    
    a123=abs(a123); a134=abs(a134); a124=abs(a124); a234=abs(a234);
    return (a123+a134+a124+a234)/8.0; // not divide by 4.0
}

// NAME: createSDMatrix (create seed distance matrix)
//
// PURPOSE: given an array of cluster seeds (cseed) and array of lines (lines), calculates a
//      matrix whose elements SDmat[i][j] measures the difference (distance) between the ith
//      seed and jth line.
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
    for (int i = 0; i < nseeds; i++)
        SDmat[i] = new float[nlines];
    
    CvPoint* temp1, * temp2;
    for (int i = 0; i < nseeds; i++) 
        for (int j = 0; j < nlines; j++) {
            temp1 = cseed[i];                           // read seed data into CvPoint*
            temp2 = (CvPoint*)cvGetSeqElem(lines, j);   // read sequence data into CvPoint*
            // now calculate difference
            SDmat[i][j] = areaQuad(temp1[0].x,temp1[0].y, temp1[1].x,temp1[1].y
                                    ,temp2[0].x,temp2[0].y, temp2[1].x,temp2[1].y);
        }
}


// NAME: addClusterSeed
//
void addClusterSeed (CvPoint** &cseed, int &nseeds, CvSeq* lines, int nlines) {
// looks at seed array (which is a CvPoint[i][2], with each i holding the 2 endpoints of
// the ith seed line. Picks out
    if (nseeds == 0) { // no seeds yet. Just take strongest line as seed
        cseed = new CvPoint*[10]; // allocate 10 x 2 matrix. Allows up to 10 cluster seeds
        for (int i = 0; i < 10; i++) cseed[i] = new CvPoint[2];
        
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
        nseeds++;
    }
}

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
        float x1=0,y1=0,x2=0,y2=0; int nl=0;
        
        for (int i = 0; i < indexArraySize[s]; i++) { // step thru lines in cluster s
            CvPoint* temp = (CvPoint*)cvGetSeqElem(lines, indexArray[s][i]);   
            x1+=temp[0].x; y1+=temp[0].y;
            x2+=temp[1].x; y2+=temp[1].y;
            nl++; // number of lines in this cluster
            
printf ("%d %d %d %d\n", temp[0].x,temp[0].y,temp[1].x,temp[1].y);
        }
printf ("\n");
        
        cseed[s][0].x = x1/nl; cseed[s][0].y = y1/nl; // average of cluster is new seed
        cseed[s][1].x = x2/nl; cseed[s][1].y = y2/nl;
    }
     
    iterations--;
    if (iterations > 0) // call recursively if needed
        KMcluster (cseed, nseeds, lines, nlines, iterations);
}
