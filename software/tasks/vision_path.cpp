#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

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

void MDA_VISION_MODULE_PATH:: primary_filter (const IplImage* src) {
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

    if (nClusters == 0) {
        printf ("Path: No clusters =(\n");
        return NO_TARGET;
    }
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
            m_angle = (position_angle_0 + position_angle_1) * 0.5;
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }
        if (abs(position_angle_0) > 1.3*abs(position_angle_1) || 1.3*abs(position_angle_0) < abs(position_angle_1)) {
            DEBUG_PRINT ("Path Sanity Failure: Line angles do not match\n");
            // set angle to be the smaller of the two angles (absolute)
            m_angle = (abs(position_angle_0) <= abs(position_angle_1)) ? position_angle_0 : position_angle_1;
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
    else {
    	printf ("nClusters > 2 in Path! This is not implemented yet!\n");
        return NO_TARGET;
    }

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        m_angular_x = RAD_TO_DEG * atan((float)m_pixel_x / m_pixel_y);
        DEBUG_PRINT ("Path: (%d,%d) (%5.2f,?)\n", m_pixel_x, m_pixel_y, 
            m_angular_x); 
        return retval;
}
