#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
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
	_window (mvWindow("Path Vision Module")),
	_HSVFilter (mvHSVFilter(MDA_VISION_PATH_SETTINGS)),
	_Morphology (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
	_HoughLines (mvHoughLines(MDA_VISION_PATH_SETTINGS)),
	_lines (mvLines())
{
    _filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_PATH:: ~MDA_VISION_MODULE_PATH () {
    mvReleaseScratchImage();
}

void MDA_VISION_MODULE_PATH:: primary_filter (IplImage* src) {
    _HSVFilter.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines.clearData ();
    _KMeans.clearData ();
    
    _Morphology.gradient(_filtered_img, _filtered_img);
    _HoughLines.findLines (_filtered_img, &_lines);
    _KMeans.cluster_auto (1, 4, &_lines, 1);

    _KMeans.drawOntoImage (_filtered_img);

    _window.showImage (_filtered_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_PATH:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
    unsigned nClusters = _KMeans.nClusters();

//============ 0 CLUSTERS ================================================================================================
    if (nClusters == 0) {
        printf ("Path: No clusters =(\n");
        return NO_TARGET;
    }
//============ 1 CLUSTER =================================================================================================
    else if (nClusters == 1) {
        /// single line, not much we can do but return its center and estimate range
        DEBUG_PRINT ("Path: 1 cluster =|\n");
        m_pixel_x = (_KMeans[0][0].x + _KMeans[0][1].x - _filtered_img->width)*0.5;       // centroid of line
        m_pixel_y = (_KMeans[0][0].y + _KMeans[0][1].y - _filtered_img->height)*0.5;
        unsigned length = line_sqr_length(_KMeans[0]);

        /// check that the line is near center of image
        
        /// calculate range if we pass sanity check
        m_range = (PATH_REAL_LENGTH * _filtered_img->width) / (sqrt(length) * TAN_FOV_X);
        m_angle = RAD_TO_DEG * line_angle_to_vertical(_KMeans[0]);
        DEBUG_PRINT ("Path Range = %d, PAngle = %5.2f\n", m_range, m_angle);

        retval = ONE_SEGMENT;       
        goto RETURN_CENTROID;
    }
//============ 2 CLUSTERS ================================================================================================
    else if (nClusters == 2) {
        DEBUG_PRINT ("Path: 2 clusters =)\n");
        int x00 = _KMeans[0][0].x,   y00 = _KMeans[0][0].y;
        int x01 = _KMeans[0][1].x,   y01 = _KMeans[0][1].y;
        int x10 = _KMeans[1][0].x,   y10 = _KMeans[1][0].y;
        int x11 = _KMeans[1][1].x,   y11 = _KMeans[1][1].y;
        m_pixel_x = (int)((x00+x01+x10+x11)*0.25 - _filtered_img->width*0.5);
        m_pixel_y = (int)((y00+y01+y10+y11)*0.25 - _filtered_img->height*0.5);

        float position_angle_0 = RAD_TO_DEG * line_angle_to_vertical(_KMeans[0]); 
        float position_angle_1 = RAD_TO_DEG * line_angle_to_vertical(_KMeans[1]);

        /// sanity checks
        unsigned length_0 = line_sqr_length(_KMeans[0]);
        unsigned length_1 = line_sqr_length(_KMeans[1]);
        DEBUG_PRINT ("angles = %f, %f\n",position_angle_0,position_angle_1);
        
        if (length_0 > 1.3*length_1 || 1.3*length_0 < length_1) {
            DEBUG_PRINT ("Path Sanity Failure: Lines too dissimilar\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }

        // Tragically, angles are a circular quantity, and we can't just subtract them.
        float normal_diff = absf(position_angle_0 - position_angle_1);
        float circular_diff = (float)180 - absf(position_angle_0) - absf(position_angle_1);
        float dA = (((normal_diff) < (circular_diff)) ? (normal_diff) : (circular_diff));

        if (dA> LINE_ANG_THRESH) {
            DEBUG_PRINT ("Path Sanity Failure: Line angles do not match\n");
            // set angle to be the smaller of the two angles (absolute)
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }

        // calculate values
        m_range = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(length_0)+sqrt(length_1))*0.5 * TAN_FOV_X);
        m_angle = (position_angle_0 + position_angle_1) * 0.5;
        DEBUG_PRINT ("Path Range = %d, PAngle = %5.2f\n", m_range, m_angle);

        retval = FULL_DETECT;
        goto RETURN_CENTROID;
    }
//============ 3 OR MORE CLUSTERS ========================================================================================
    else if (nClusters >= 3 && nClusters <= 4){
        //A and B are the indicies of paired lines, R holds rejected lines
        int A[2] = {-1,-1}, B[2] = {-1,-1}, R[nClusters];
        
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
                x[i][j] = _KMeans[i][j].x;
                y[i][j] = _KMeans[i][j].y;

                xSum += x[i][j];
                ySum += y[i][j];
            }
            R[i] = -1;

            //Zero out trivial self comparisons
            dis_sep[i][i] = len_sep[i][i] = ang_sep[i][i] = 0;
            len_bool[i][i] = sep_bool[i][i] = ang_bool[i][i] = false;
            pair_weight[i][i] = 9001;

            //Calculate frequently used quantities
            lengths[i] = line_sqr_length(_KMeans[i]);
            position_angles[i] = RAD_TO_DEG * line_angle_to_vertical(_KMeans[i]);  
        }

        //Fill out the lower half of commonly used properties
        for(int i = 0; i<(int)nClusters; i++){
            for (int j = 0; j<i; j++){
                //Scale observed pixel distances to real distances
                float scale_factor = (PATH_REAL_LENGTH / ((sqrt(lengths[i])+sqrt(lengths[j]))*0.5));

                //Pre calculate and scale the separation of important variables
                len_sep[i][j] = abs(lengths[i] - lengths[j]) * scale_factor;
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

                if(!ang_bool[i][j] || !len_bool[i][j] || !sep_bool[i][j]) pair_weight[i][j] = 9001;
                else pair_weight[i][j] = (float)(K_ANG * ang_sep[i][j] + K_LEN * len_sep[i][j] + K_DIS * dis_sep[i][j]);
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
        m_pixel_x = (int)(xSum*0.167 - _filtered_img->width*0.5);
        m_pixel_y = (int)(ySum*0.167 - _filtered_img->height*0.5);

        //Check the easily identifiable bad case first
        if(cross_sum == 0){
            //No lines meet all 3 similarity criteria
            DEBUG_PRINT ("Path Sanity Failure: Lines are too dissimilar\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }

        //We only have one possible pair; find it and determine if the remaining lines are
        //rejected, or counted as a partial match
        else if(cross_sum == 1){
            float best = 9000;

            DEBUG_PRINT("Path: %d Clusters =/\n", nClusters);
            //Find the lowest pairing score
            for(int i = 0; i<(int)nClusters; i++){
                for (int j = 0; j<i; j++){
                    if(pair_weight[i][j] < best){
                        A[0] = i; A[1] = j;
                        best = pair_weight[i][j];
                    }
                }
            }
            assert(A[0] != -1);  //at least one sane pair must exist, as cross_sum was not zero
            
            //Given the path pairing, determine the leftover line(s)
            unsigned temp = 0;
            for(int i=0; i<(int)nClusters; i++){
                if(i == A[0] || i == A[1]) continue;
                R[temp] = i;
                temp++;
                if(temp + 2 >= nClusters) break;
            }

            //Determine whether remaining lines are partial detects or rejects
            if(nClusters == 3 && dis_sep[R[0]][A[0]] > 10*LINE_DIS_THRESH && dis_sep[R[0]][A[1]] > 10*LINE_DIS_THRESH){
                //For nClusters = 3, count as a partial if it is far enough away from the main pair
                B[0] = B[1] = R[0];
                R[0] = -1;
            }
            else if(nClusters > 3){
                //Discard any lines that are too close to the main pair
                for(int i = 0; i<(int)nClusters-2; i++){
                    if(dis_sep[R[i]][A[0]] < 10*LINE_DIS_THRESH && dis_sep[R[i]][A[1]] < 10*LINE_DIS_THRESH) R[i] = 0;
                }

                //Of the remaining lines, pick the closest pair, or none if they all fail relaxed proximity test
                best = 2*LINE_DIS_THRESH;
                for(int i = 0; i<(int)nClusters-2; i++){
                    for (int j = 0; j<i; j++){
                        if(R[i] != -1 && R[j] != -1 && dis_sep[R[i]][R[j]] < best){
                            B[0] = i; B[1] = j;
                            best = dis_sep[R[i]][R[j]];
                        }
                    }
                }
            }

            //If we found a partial detect, return it
            if(B[0] != -1){
                m_pixel_x = (int)((x[A[0]][0] + x[A[0]][1] + x[A[1]][0] + x[A[1]][1])*0.25 - _filtered_img->width*0.5);
                m_pixel_y = (int)((y[A[0]][0] + y[A[0]][1] + y[A[1]][0] + y[A[1]][1])*0.25 - _filtered_img->height*0.5);
                m_pixel_x_alt = (int)((x[B[0]][0] + x[B[0]][1] + x[B[1]][1] + x[B[1]][1])*0.5 - _filtered_img->width*0.5);
                m_pixel_y_alt = (int)((y[B[0]][0] + y[B[0]][1] + x[B[1]][1] + x[B[1]][1])*0.5 - _filtered_img->height*0.5);

                m_range = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(lengths[A[0]])+sqrt(lengths[A[1]]))*0.5 * TAN_FOV_X);
                m_angle = (position_angles[A[0]] + position_angles[A[1]]) * 0.5;
                m_range_alt = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(lengths[B[0]])+sqrt(lengths[B[1]]))*0.5 * TAN_FOV_X);
                m_angle_alt = (position_angles[B[0]] + position_angles[B[1]]) * 0.5;

                DEBUG_PRINT ("Path Detect + Partial: Range = %d, PAngle = %5.2f\n", m_range, m_angle);
                retval = FULL_DETECT_PLUS;
                goto RETURN_TWO_CENTROIDS;
            }
            //Else, reject the other lines; we have only found one path
            else{
                m_pixel_x = (int)((xSum - x[R[0]][0] - x[R[0]][1])*0.25 - _filtered_img->width*0.5);
                m_pixel_y = (int)((ySum - y[R[0]][0] - y[R[0]][1])*0.25 - _filtered_img->height*0.5);
                m_range = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(lengths[A[0]])+sqrt(lengths[A[1]]))*0.5 * TAN_FOV_X);
                m_angle = (position_angles[A[0]] + position_angles[A[1]]) * 0.5;

                DEBUG_PRINT ("Path Detect: Range = %d, PAngle = %5.2f\n", m_range, m_angle);
                retval = FULL_DETECT;
                goto RETURN_CENTROID;
            }
        }
        else if (cross_sum > 1){
            int pair_ranks[cross_sum][2];
            float scores[cross_sum];

            //Rank pairs by score; insertion sort used as n is small
            for(int i = 0; i<(int)nClusters; i++){
                for(int j = 0; j<i; j++){
                    if(pair_weight[i][j] > 9000) continue;
                    int k = 0;
                    while(pair_weight[i][j] > scores[k]) k++;
                    for(int l=cross_sum-1; l>k; l--){
                        pair_ranks[l][0] = pair_ranks[l-1][0];
                        pair_ranks[l][1] = pair_ranks[l-1][1];
                    }
                    pair_ranks[k][0] = i;
                    pair_ranks[k][1] = j;
                    scores[k] = pair_weight[i][j];
                }
            }
            A[0] = pair_ranks[0][0]; A[1] = pair_ranks[0][1];

            for(int i = 1; i < (int)cross_sum; i++){
                //Skip ranked pairs that contain lines from the first pair
                if(pair_ranks[i][0] == A[0] || pair_ranks[i][0] == A[1] || pair_ranks[i][1] == A[0] || pair_ranks[i][1] == A[1]) continue;
                if(dis_sep[A[0]][pair_ranks[i][0]] < 6*LINE_DIS_THRESH || dis_sep[A[0]][pair_ranks[i][1]] < 6*LINE_DIS_THRESH ||
                    dis_sep[A[1]][pair_ranks[i][0]] < 6*LINE_DIS_THRESH || dis_sep[A[1]][pair_ranks[i][1]] < 6*LINE_DIS_THRESH) continue;
                
                B[0] = pair_ranks[i][0];
                B[1] = pair_ranks[i][1];
                break;
            }

            //If we have two independant pairs, far enough apart, discard everything else
            if(B[0] != -1){
                m_pixel_x = (int)((x[A[0]][0] + x[A[0]][1] + x[A[1]][0] + x[A[1]][1])*0.25 - _filtered_img->width*0.5);
                m_pixel_y = (int)((y[A[0]][0] + y[A[0]][1] + y[A[1]][0] + y[A[1]][1])*0.25 - _filtered_img->height*0.5);
                m_pixel_x_alt = (int)((x[B[0]][0] + x[B[0]][1] + x[B[1]][1] + x[B[1]][1])*0.5 - _filtered_img->width*0.5);
                m_pixel_y_alt = (int)((y[B[0]][0] + y[B[0]][1] + x[B[1]][1] + x[B[1]][1])*0.5 - _filtered_img->height*0.5);

                m_range = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(lengths[A[0]])+sqrt(lengths[A[1]]))*0.5 * TAN_FOV_X);
                m_angle = (position_angles[A[0]] + position_angles[A[1]]) * 0.5;
                m_range_alt = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(lengths[B[0]])+sqrt(lengths[B[1]]))*0.5 * TAN_FOV_X);
                m_angle_alt = (position_angles[B[0]] + position_angles[B[1]]) * 0.5;

                DEBUG_PRINT ("Double Path Detect: Range_A = %d, PAngle_A = %5.2f\n", m_range, m_angle);
                DEBUG_PRINT ("Double Path Detect: Range_B = %d, PAngle_B = %5.2f\n", m_range_alt, m_angle_alt);
                retval = DOUBLE_DETECT;
                goto RETURN_TWO_CENTROIDS;
            }

            //If not, given the path pairing, determine the leftover line(s)
            unsigned temp = 0;
            for(int i=0; i<(int)nClusters; i++){
                if(i == A[0] || i == A[1]) continue;
                R[temp] = i;
                temp++;
                if(temp + 2 >= nClusters) break;
            }

            //Determine whether remaining lines are partial detects or rejects
            if(nClusters == 3 && dis_sep[R[0]][A[0]] > 6*LINE_DIS_THRESH && dis_sep[R[0]][A[1]] > 6*LINE_DIS_THRESH){
                //For nClusters = 3, count as a partial if it is far enough away from the main pair
                B[0] = B[1] = R[0];
                R[0] = -1;
            }
            else if(nClusters > 3){
                //Discard any lines that are too close to the main pair
                for(int i = 0; i<(int)nClusters-2; i++){
                    if(dis_sep[R[i]][A[0]] < 6*LINE_DIS_THRESH && dis_sep[R[i]][A[1]] < 6*LINE_DIS_THRESH) R[i] = 0;
                }

                //Of the remaining lines, pick the closest pair, or none if they all fail relaxed proximity test
                float best = 2*LINE_DIS_THRESH;
                for(int i = 0; i<(int)nClusters-2; i++){
                    for (int j = 0; j<i; j++){
                        if(R[i] != -1 && R[j] != -1 && dis_sep[R[i]][R[j]] < best){
                            B[0] = i; B[1] = j;
                            best = dis_sep[R[i]][R[j]];
                        }
                    }
                }
            }

            //If we found a partial detect, return it
            if(B[0] != -1){
                m_pixel_x = (int)((x[A[0]][0] + x[A[0]][1] + x[A[1]][0] + x[A[1]][1])*0.25 - _filtered_img->width*0.5);
                m_pixel_y = (int)((y[A[0]][0] + y[A[0]][1] + y[A[1]][0] + y[A[1]][1])*0.25 - _filtered_img->height*0.5);
                m_pixel_x_alt = (int)((x[B[0]][0] + x[B[0]][1] + x[B[1]][1] + x[B[1]][1])*0.5 - _filtered_img->width*0.5);
                m_pixel_y_alt = (int)((y[B[0]][0] + y[B[0]][1] + x[B[1]][1] + x[B[1]][1])*0.5 - _filtered_img->height*0.5);

                m_range = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(lengths[A[0]])+sqrt(lengths[A[1]]))*0.5 * TAN_FOV_X);
                m_angle = (position_angles[A[0]] + position_angles[A[1]]) * 0.5;
                m_range_alt = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(lengths[B[0]])+sqrt(lengths[B[1]]))*0.5 * TAN_FOV_X);
                m_angle_alt = (position_angles[B[0]] + position_angles[B[1]]) * 0.5;

                DEBUG_PRINT ("Path Detect + Partial: Range = %d, PAngle = %5.2f\n", m_range, m_angle);
                retval = FULL_DETECT_PLUS;
                goto RETURN_TWO_CENTROIDS;
            }
            //Else, reject the other lines; we have only found one path
            else{
                m_pixel_x = (int)((xSum - x[R[0]][0] - x[R[0]][1])*0.25 - _filtered_img->width*0.5);
                m_pixel_y = (int)((ySum - y[R[0]][0] - y[R[0]][1])*0.25 - _filtered_img->height*0.5);
                m_range = (PATH_REAL_LENGTH * _filtered_img->width) / ((sqrt(lengths[A[0]])+sqrt(lengths[A[1]]))*0.5 * TAN_FOV_X);
                m_angle = (position_angles[A[0]] + position_angles[A[1]]) * 0.5;

                DEBUG_PRINT ("Path Detect: Range = %d, PAngle = %5.2f\n", m_range, m_angle);
                retval = FULL_DETECT;
                goto RETURN_CENTROID;
            }
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
}
