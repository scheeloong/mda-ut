#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

/// ########################################################################
/// MODULE_PATH methods
/// ########################################################################
MDA_VISION_MODULE_PATH:: MDA_VISION_MODULE_PATH () :
	_window (mvWindow("Path Vision Module")),
	_HSVFilter (mvHSVFilter(MDA_VISION_MODULE_TEST_SETTINGS)),
	_Morphology (mvBinaryMorphology(5, 5, MV_KERN_RECT)),
	_HoughLines (mvHoughLines(MDA_VISION_MODULE_TEST_SETTINGS)),
	_lines (mvLines())
{
    _filtered_img = mvCreateImage (); // common size
}

MDA_VISION_MODULE_PATH:: ~MDA_VISION_MODULE_PATH () {
    cvReleaseImage (&_filtered_img);
}

void MDA_VISION_MODULE_PATH:: primary_filter (const IplImage* src) {
    _HSVFilter.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines.clearData ();
    _KMeans.clearData ();
    
    _Morphology.gradient(_filtered_img, _filtered_img);
    mvGradient (_filtered_img, _filtered_img, 5, 5);
    _HoughLines.findLines (_filtered_img, &_lines);
    _KMeans.cluster_auto (1, 4, &_lines, 1);

    _lines.drawOntoImage (_filtered_img);
    _KMeans.drawOntoImage (_filtered_img);

    _window.showImage (_filtered_img);
}

int MDA_VISION_MODULE_PATH:: calc_vci (VCI* interface) {
    int retval = 0;
    int pixel_x = 0, pixel_y = 0;

    if (_KMeans.nClusters() == 0) {
        /// NO CLUSTERS
        printf ("Path: No clusters =(\n");
        return -1;
    }
    else if (_KMeans.nClusters() == 1) {
        /// single line, not much we can do but return its center and estimate range
        DEBUG_PRINT ("Path: 1 cluster =|\n");
        pixel_x = (_KMeans[0][0].x + _KMeans[0][1].x - _filtered_img->width)*0.5;       // centroid of line
        pixel_y = (_KMeans[0][0].y + _KMeans[0][1].y - _filtered_img->height)*0.5;
        unsigned length = line_sqr_length(_KMeans[0]);

        /// check that the line is near center of image
        
        /// calculate range if we pass sanity check
        interface->range = (PATH_REAL_LENGTH * _filtered_img->width) / (length * TAN_FOV_X);
        interface->angle = RAD_TO_DEG * line_angle_to_vertical(_KMeans[0]);
        DEBUG_PRINT ("Path Range = %d, Angle = %5.2f\n", interface->range, interface->angle);
        retval = 0;       
        goto RETURN_CENTROID;
    }
    else if (_KMeans.nClusters() == 2) {
        DEBUG_PRINT ("Path: 2 clusters =)\n");
        int x00 = _KMeans[0][0].x,   y00 = _KMeans[0][0].y;
        int x01 = _KMeans[0][1].x,   y01 = _KMeans[0][1].y;
        int x10 = _KMeans[1][0].x,   y10 = _KMeans[1][0].y;
        int x11 = _KMeans[1][1].x,   y11 = _KMeans[1][1].y;
        pixel_x = (int)((x00+x01+x10+x11)*0.25 - _filtered_img->width*0.5);
        pixel_y = (int)((y00+y01+y10+y11)*0.25 - _filtered_img->height*0.5);

        float position_angle_0 = RAD_TO_DEG * line_angle_to_vertical(_KMeans[0]); 
        float position_angle_1 = RAD_TO_DEG * line_angle_to_vertical(_KMeans[1]);

        /// sanity checks
        unsigned length_0 = line_sqr_length(_KMeans[0]);
        unsigned length_1 = line_sqr_length(_KMeans[1]);
        
        if (length_0 > 1.3*length_1 || 1.3*length_0 < length_1) {
            DEBUG_PRINT ("Path Sanity Failure: Lines too dissimilar\n");
            retval = 2;
            goto RETURN_CENTROID;
        }
        if (position_angle_0 > 1.2*position_angle_1 || 1.2*position_angle_0 < position_angle_1) {
            DEBUG_PRINT ("Path Sanity Failure: Line angles do not match\n");
            retval = 2;
            goto RETURN_CENTROID;
        }

        // calculate values
        interface->range = (PATH_REAL_LENGTH * _filtered_img->width) / ((length_0+length_1)*0.5 * TAN_FOV_X);
        interface->angle = (position_angle_0 + position_angle_1) * 0.5;
        DEBUG_PRINT ("Path Range = %d, Angle = %5.2f\n", interface->range, interface->angle);
        retval = 0;
        goto RETURN_CENTROID;
    }
    else {
    	printf ("nClusters > 2 in Path! This is not implemented yet!");
    }

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        interface->angle_x = RAD_TO_DEG * atan(TAN_FOV_X * pixel_x / _filtered_img->width);
        interface->angle_y = RAD_TO_DEG * atan(TAN_FOV_Y * pixel_y / _filtered_img->height);
        DEBUG_PRINT ("Path (pixels, degrees): <%d,%d> <%5.2f,%5.2f>\n", pixel_x, pixel_y, 
            interface->angle_x, interface->angle_y); 
        return retval;
}
