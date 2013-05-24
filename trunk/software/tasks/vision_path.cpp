#include "mda_vision.h"

#define M_DEBUG
#ifdef M_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

float absf(float f){
    if(f < 0) return f*-1;
    else return f;
}

const char MDA_VISION_MODULE_PATH::MDA_VISION_PATH_SETTINGS[] = "vision_path_settings.csv";

/// ########################################################################
/// MODULE_PATH methods
/// ########################################################################
MDA_VISION_MODULE_PATH:: MDA_VISION_MODULE_PATH () :
	window (mvWindow("Path Vision 1")),
    window2 (mvWindow("Path Vision 2")),
    HSVFilter (mvHSVFilter(MDA_VISION_PATH_SETTINGS)),
    Morphology (mvBinaryMorphology(19, 19, MV_KERN_RECT)),
    Morphology2 (mvBinaryMorphology(7, 7, MV_KERN_RECT)),
    advanced_filter (mvAdvancedColorFilter("test_settings.csv")),
    HoughLines (mvHoughLines(MDA_VISION_PATH_SETTINGS))
{
    color_img = mvGetScratchImage_Color();
    gray_img = mvGetScratchImage();
    gray_img_2 = mvGetScratchImage2();
}

MDA_VISION_MODULE_PATH:: ~MDA_VISION_MODULE_PATH () {
    mvReleaseScratchImage_Color();
    mvReleaseScratchImage();
    mvReleaseScratchImage2();
}

void MDA_VISION_MODULE_PATH:: primary_filter (IplImage* src) {
    //_HSVFilter.filter (src, gray_image);
    /*AdvancedColorFilter.flood_image(src, gray_image);
    gray_image->origin = src->origin;
    lines.clearData ();
    KMeans.clearData ();
    
    Morphology2.open(gray_image, gray_image);
    Morphology.close(gray_image, gray_image);
    Morphology2.gradient(gray_image, gray_image);
    HoughLines.findLines (gray_image, &lines);
    KMeans.cluster_auto (1, 4, &lines, 1);

    KMeans.drawOntoImage (gray_image);
    */

    watershed_filter.watershed(src, gray_img);
    window.showImage (gray_img);

    int seg = 0;
    const double COLOR_DIVISION_FACTOR = 1.0;
    COLOR_TRIPLE color;
    COLOR_TRIPLE color_template (160,95,157,0);

    CvPoint best_centroid = cvPoint(MV_UNDEFINED_VALUE, MV_UNDEFINED_VALUE);
    float best_length = MV_UNDEFINED_VALUE;
    float best_angle = MV_UNDEFINED_VALUE;
    double best_diff = 1000000;
    
    while ( watershed_filter.get_next_watershed_segment(gray_img_2, color) ) {
        //DEBUG_PRINT ("\nSegment %d\n", ++seg);
        //DEBUG_PRINT ("\tColor (%3d,%3d,%3d)\n", color.m1, color.m2, color.m3);

        // calculate color diff
        double color_diff = 0;//static_cast<double>(color.diff(color_template)) / COLOR_DIVISION_FACTOR;

        // calculate shape diff
        CvPoint centroid;
        float length, angle;
        double shape_diff = contour_filter.match_rectangle(gray_img_2, centroid, length, angle);
        if (shape_diff < 0) // error from the shape matching
            continue;

        double diff = color_diff + shape_diff;
        //DEBUG_PRINT ("\tColor_Diff=%6.4f  Shape_Diff=%6.4f\n\tFinal_Diff=%6.4f\n", color_diff, shape_diff, diff);

        if (seg == 1 || diff < best_diff) {
            best_diff = diff;
            best_centroid = centroid;
            best_length = length;
            best_angle = angle;
            cvCopy (gray_img_2, gray_img);
        }
    }

    DEBUG_PRINT ("Best Diff = %5.2lf\n", best_diff);
    window2.showImage (gray_img);
    
    m_pixel_x = best_centroid.x;
    m_pixel_y = best_centroid.y;
    m_range = (PATH_REAL_LENGTH * gray_img->width) / (sqrt(best_length) * TAN_FOV_X);
    m_angle = best_angle;
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_PATH:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
    /*unsigned nClusters = KMeans.nClusters();

//============ 0 CLUSTERS ================================================================================================
    if (nClusters == 0) {
        printf ("Path: No clusters =(\n");
        return NO_TARGET;
    }
//============ 1 CLUSTER =================================================================================================
    else if (nClusters == 1) {
        /// single line, not much we can do but return its center and estimate range
        DEBUG_PRINT ("Path: 1 cluster =|\n");
        m_pixel_x = (KMeans[0][0].x + KMeans[0][1].x - gray_image->width)*0.5;       // centroid of line
        m_pixel_y = (KMeans[0][0].y + KMeans[0][1].y - gray_image->height)*0.5;
        unsigned length = line_sqr_length(KMeans[0]);

        /// check that the line is near center of image
        
        /// calculate range if we pass sanity check
        m_range = (PATH_REAL_LENGTH * gray_image->width) / (sqrt(length) * TAN_FOV_X);
        m_angle = RAD_TO_DEG * line_angle_to_vertical(KMeans[0]);
        DEBUG_PRINT ("Path Range = %d, PAngle = %5.2f\n", m_range, m_angle);

        retval = ONE_SEGMENT;       
        goto RETURN_CENTROID;
    }
//============ 2 CLUSTERS ================================================================================================
    else if (nClusters == 2) {
        DEBUG_PRINT ("Path: 2 clusters =)\n");
        int x00 = KMeans[0][0].x,   y00 = KMeans[0][0].y;
        int x01 = KMeans[0][1].x,   y01 = KMeans[0][1].y;
        int x10 = KMeans[1][0].x,   y10 = KMeans[1][0].y;
        int x11 = KMeans[1][1].x,   y11 = KMeans[1][1].y;
        m_pixel_x = (int)((x00+x01+x10+x11)*0.25 - gray_image->width*0.5);
        m_pixel_y = (int)((y00+y01+y10+y11)*0.25 - gray_image->height*0.5);

        float position_angle_0 = RAD_TO_DEG * line_angle_to_vertical(KMeans[0]); 
        float position_angle_1 = RAD_TO_DEG * line_angle_to_vertical(KMeans[1]);

        /// sanity checks
        unsigned length_0 = line_sqr_length(KMeans[0]);
        unsigned length_1 = line_sqr_length(KMeans[1]);
        DEBUG_PRINT ("angles = %f, %f\n",position_angle_0,position_angle_1);

        // Tragically, angles are a circular quantity, and we can't just subtract them.
        float normal_diff = absf(position_angle_0 - position_angle_1);
        float circular_diff = (float)180 - absf(position_angle_0) - absf(position_angle_1);
        float dA = (((normal_diff) < (circular_diff)) ? (normal_diff) : (circular_diff));
        
        if (dA > LINE_ANG_THRESH) {
            //Detecting one line from each of two paths
            if(length_0 > LINE_LEN_THRESH && length_0 > LINE_LEN_THRESH){    
                m_pixel_x = (int)((x00+x01)*0.5 - gray_image->width*0.5);
                m_pixel_y = (int)((y00+y01)*0.5 - gray_image->height*0.5);
                m_pixel_x_alt = (int)((x10+x11)*0.5 - gray_image->width*0.5);
                m_pixel_y_alt = (int)((y10+y11)*0.5 - gray_image->height*0.5);

                m_range = (PATH_REAL_LENGTH * gray_image->width) / (sqrt(length_0) * TAN_FOV_X);
                m_angle = position_angle_0;
                m_range_alt = (PATH_REAL_LENGTH * gray_image->width) / (sqrt(length_1) * TAN_FOV_X);
                m_angle_alt = position_angle_1;

                DEBUG_PRINT ("Double Segment Detect: Range_A = %d, PAngle_A = %5.2f\n", m_range, m_angle);
                DEBUG_PRINT ("Double Segment Detect: Range_B = %d, PAngle_B = %5.2f\n", m_range_alt, m_angle_alt);

                retval = TWO_SEGMENT;
                goto RETURN_TWO_CENTROIDS;
            }
            //Return one line or the other if one is too short
            else if (length_0 > LINE_LEN_THRESH){
                m_pixel_x = (int)((x00+x01)*0.5 - gray_image->width*0.5);
                m_pixel_y = (int)((y00+y01)*0.5 - gray_image->height*0.5);
                m_range = (PATH_REAL_LENGTH * gray_image->width) / (sqrt(length_0) * TAN_FOV_X);
                m_angle = position_angle_0;

                DEBUG_PRINT ("One Segment Range = %d, PAngle = %5.2f\n", m_range, m_angle);

                retval = ONE_SEGMENT;
                goto RETURN_CENTROID;
            }
            else if (length_1 > LINE_LEN_THRESH){
                m_pixel_x = (int)((x10+x11)*0.5 - gray_image->width*0.5);
                m_pixel_y = (int)((y10+y11)*0.5 - gray_image->height*0.5);
                m_range = (PATH_REAL_LENGTH * gray_image->width) / (sqrt(length_1) * TAN_FOV_X);
                m_angle = position_angle_1;

                DEBUG_PRINT ("One Segment Range = %d, PAngle = %5.2f\n", m_range, m_angle);

                retval = ONE_SEGMENT;
                goto RETURN_CENTROID;
            }

            DEBUG_PRINT ("Path Sanity Failure: Line angles do not match\n");
            // set angle to be the smaller of the two angles (absolute)
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }
        else if (length_0 > 1.3*length_1 || 1.3*length_0 < length_1) {
            DEBUG_PRINT ("Path Sanity Failure: Lines too dissimilar\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }

        // calculate values
        m_range = (PATH_REAL_LENGTH * gray_image->width) / ((sqrt(length_0)+sqrt(length_1))*0.5 * TAN_FOV_X);
        m_angle = (position_angle_0 + position_angle_1) * 0.5;
        DEBUG_PRINT ("Path Range = %d, PAngle = %5.2f\n", m_range, m_angle);

        retval = FULL_DETECT;
        goto RETURN_CENTROID;
    }
//============ 3 OR MORE CLUSTERS ========================================================================================
    else if (nClusters >= 3 && nClusters <= 4){
        DEBUG_PRINT("Path: %d Clusters =/\n", nClusters);
        //A and B are the indicies of paired lines, R holds rejected lines
        int A[2] = {-1,-1}, B[2] = {-1,-1}, R[nClusters];
        bool partial = false;
        
        float    position_angles [nClusters];
        unsigned lengths         [nClusters];

        //Pre calculate separations
        float dis_sep[nClusters][nClusters];     //Euclidean distance apart
        float len_sep[nClusters][nClusters];     //Length difference
        float ang_sep[nClusters][nClusters];     //Angular separation

        unsigned xSum = 0;
        unsigned ySum = 0;

        //Nastyness: we add up the boolean conditions and rely on bool = {0|1}
        unsigned cross_sum = 0;

        //Precalculate frequently used logic conditions
        bool len_bool[nClusters][nClusters];
        bool sep_bool[nClusters][nClusters];
        bool ang_bool[nClusters][nClusters];

        float pair_weight[nClusters][nClusters];

        int x[nClusters][2], y[nClusters][2];
        for (int i = 0; i < (int)nClusters; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                x[i][j] = KMeans[i][j].x;
                y[i][j] = KMeans[i][j].y;

                xSum += x[i][j];
                ySum += y[i][j];
            }
            R[i] = -1;

            //Zero out trivial self comparisons
            dis_sep[i][i] = len_sep[i][i] = ang_sep[i][i] = 0;
            len_bool[i][i] = sep_bool[i][i] = ang_bool[i][i] = false;
            pair_weight[i][i] = -1;

            //Calculate frequently used quantities
            int dx = KMeans[i][0].x - KMeans[i][1].x;
            int dy = KMeans[i][0].y - KMeans[i][1].y;
            //DEBUG_PRINT("Line %d: (%d,%d):(%d,%d)   (dx,dy): (%d,%d) ==> %d\n",i,_KMeans[i][0].x, KMeans[i][0].y,_KMeans[i][1].x, KMeans[i][1].y,dx,dy,dx*dx+dy*dy); 

            lengths[i] = (unsigned)(dx*dx + dy*dy);
            position_angles[i] = RAD_TO_DEG * line_angle_to_vertical(KMeans[i]);
            //DEBUG_PRINT("Length[%d]: %d\n",i,dx*dx + dy*dy);  
        }

        //Fill out the lower half of commonly used properties
        for(int i = 0; i<(int)nClusters; i++){
            for (int j = 0; j<i; j++){
                //Scale observed pixel distances to real distances
                float scale_factor = (PATH_REAL_LENGTH / ((sqrt(lengths[i])+sqrt(lengths[j]))*0.5));

                //Pre calculate and scale the separation of important variables
                len_sep[i][j] = absf(sqrt(lengths[i]) - sqrt(lengths[j])) * scale_factor;
                dis_sep[i][j] = sqrt(pow((x[i][0] + x[i][1])/2 - (x[j][0] + x[j][1])/2, 2) 
                    + pow((y[i][0] + y[i][1])/2 - (y[j][0] + y[j][1])/2, 2)) * scale_factor;

                // Tragically, angles are a circular quantity, and we can't just subtract them.
                float normal_diff = absf(position_angles[i] - position_angles[j]);
                float circular_diff = (float)180 - absf(position_angles[i]) - absf(position_angles[j]);
                ang_sep[i][j] = (((normal_diff) < (circular_diff)) ? (normal_diff) : (circular_diff));

                //Calculate frequently used conditions
                len_bool[i][j] = len_sep[i][j] < LINE_LEN_THRESH;
                ang_bool[i][j] = ang_sep[i][j] < LINE_ANG_THRESH;
                sep_bool[i][j] = dis_sep[i][j] < LINE_DIS_THRESH;

                cross_sum += (unsigned)(len_bool[i][j] && ang_bool[i][j] && sep_bool[i][j]);
                pair_weight[i][j] = (float)(K_ANG * ang_sep[i][j] + K_LEN * len_sep[i][j] + K_DIS * dis_sep[i][j]);
            }
        }

        //Fill out other half by symmetry
        for(int i = 0; i<(int)nClusters; i++){
            for (unsigned int j = i+1; j<nClusters; j++){
                len_sep[i][j] = len_sep[j][i];
                ang_sep[i][j] = ang_sep[j][i];
                dis_sep[i][j] = dis_sep[j][i];

                len_bool[i][j] = len_bool[j][i];
                ang_bool[i][j] = ang_bool[j][i];
                sep_bool[i][j] = sep_bool[j][i];

                pair_weight[i][j] = pair_weight[j][i];
            }
        }

        //If we can't determine a grouping, default to the centroid
        m_pixel_x = (int)(xSum*0.167 - gray_image->width*0.5);
        m_pixel_y = (int)(ySum*0.167 - gray_image->height*0.5);

        int lHalf = (int)((nClusters*(nClusters-1))/2);    //(n*(n-1))/2; the # of elements in the lower half of an n*n matrix
        int ranked[lHalf][2];
        float scores[lHalf];
        std::fill_n(scores, lHalf, 9001);   //Fill with an arbitrary large number
        bool flag;

        //sort each possible pair in descending order by pair weight
        for(int i=0; i<(int)nClusters; i++){
            for(int j=0; j<i; j++){

                //Insertion sort; easy to implement and good enough for n = 6
                int k = -1; flag = false;
                while(!flag && k < lHalf){
                    k++;
                    if(pair_weight[i][j] < scores[k]) flag = true;
                }

                //shift everything down, erasing last element
                for(int l = lHalf-1; l > k; l--){
                    scores[l] = scores[l-1];
                    ranked[l][0] = ranked[l-1][0];
                    ranked[l][1] = ranked[l-1][1];
                }
                scores[k] = pair_weight[i][j];
                ranked[k][0] = i; ranked[k][1] = j;
            }
        }
*/
/*
        for(int i=0; i<lHalf; i++){
            printf("Pair[%d]: (%d,%d) ==> %5.2f\n",i,ranked[i][0],ranked[i][1],scores[i]);
        }
        fflush(stdout);
*/
/*
        //Best pair goes into A, next best pair containing no lines from A goes into B
        A[0] = ranked[0][0]; A[1] = ranked[0][1];
        for(int i = 1; i < lHalf; i++){
            if(ranked[i][0] == A[0] || ranked[i][0] == A[1] || ranked[i][1] == A[0] || ranked[i][1] == A[1]) continue;
            
            B[0] = ranked[i][0];
            B[1] = ranked[i][1];
            break;
        }

        //No viable second pair; only occurrs when nClusters = 3
        if(B[0] == -1){
            int temp = 0;
            for(int i=0; i<(int)nClusters; i++){
                if(i != A[0] && i != A[1]){
                    R[temp] = i;
                    temp++;
                }
            }
            B[0] = B[1] = R[0];
        }

        //Always calculare both points of interest; it is up to control code to use/ignore based on return code
        m_pixel_x = (int)((x[A[0]][0] + x[A[0]][1] + x[A[1]][0] + x[A[1]][1])*0.25 - gray_image->width*0.5);
        m_pixel_y = (int)((y[A[0]][0] + y[A[0]][1] + y[A[1]][0] + y[A[1]][1])*0.25 - gray_image->height*0.5);
        m_pixel_x_alt = (int)((x[B[0]][0] + x[B[0]][1] + x[B[1]][0] + x[B[1]][1])*0.5 - gray_image->width*0.5);
        m_pixel_y_alt = (int)((y[B[0]][0] + y[B[0]][1] + x[B[1]][0] + x[B[1]][1])*0.5 - gray_image->height*0.5);

        m_range = (PATH_REAL_LENGTH * gray_image->width) / ((sqrt(lengths[A[0]])+sqrt(lengths[A[1]]))*0.5 * TAN_FOV_X);
        m_angle = (position_angles[A[0]] + position_angles[A[1]]) * 0.5;
        m_range_alt = (PATH_REAL_LENGTH * gray_image->width) / ((sqrt(lengths[B[0]])+sqrt(lengths[B[1]]))*0.5 * TAN_FOV_X);
        m_angle_alt = (position_angles[B[0]] + position_angles[B[1]]) * 0.5;


        if(cross_sum == 0){
            //No lines meet all 3 similarity criteria
            DEBUG_PRINT ("Path Sanity Failure: Lines are too dissimilar\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }
        else if(cross_sum == 1){
            //Only one pair, but we know there are 3 or 4 lines
            DEBUG_PRINT("One pair plus lines\n");
            retval = FULL_DETECT_PLUS;
            goto RETURN_CENTROID;
        }
        else if (cross_sum > 1){
            //Two full paths
            DEBUG_PRINT("Two full pairs\n");
            retval = DOUBLE_DETECT;
            goto RETURN_TWO_CENTROIDS;
        }
    }

//============ ELSE ======================================================================================================
    else {
    	printf ("nClusters > 4 in Path! This is not implemented!\n");
        return NO_TARGET;
    }

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        m_angular_x = RAD_TO_DEG * atan((float)m_pixel_x / m_pixel_y);
        DEBUG_PRINT ("Path: (%d,%d) (%5.2f,?)\n", m_pixel_x, m_pixel_y, 
            m_angular_x); 
        return retval;

    RETURN_TWO_CENTROIDS:
        m_angular_x = RAD_TO_DEG * atan((float)m_pixel_x / m_pixel_y);
        m_angular_x_alt = RAD_TO_DEG * atan((float)m_pixel_x_alt / m_pixel_y_alt);
        DEBUG_PRINT ("Path A: (%d,%d) (%5.2f,?)\n", m_pixel_x, m_pixel_y, 
            m_angular_x); 
        DEBUG_PRINT ("Path B: (%d,%d) (%5.2f,?)\n", m_pixel_x_alt, m_pixel_y_alt, 
            m_angular_x_alt); 
        return retval;
*/
        if (m_pixel_x == MV_UNDEFINED_VALUE || m_pixel_y == MV_UNDEFINED_VALUE || m_angle == MV_UNDEFINED_VALUE)
            return NO_TARGET;

        retval = TWO_SEGMENT;
        m_angular_x = RAD_TO_DEG * atan((float)m_pixel_x / m_pixel_y);
        DEBUG_PRINT ("Path: (%d,%d) (%5.2f,?)\n", m_pixel_x, m_pixel_y, 
            m_angular_x);
        
        return retval;
}
