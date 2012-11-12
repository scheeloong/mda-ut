#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

const char MDA_VISION_MODULE_GATE::MDA_VISION_GATE_SETTINGS[] = "vision_gate_settings.csv";

/// ########################################################################
/// MODULE_GATE methods
/// ########################################################################
MDA_VISION_MODULE_GATE:: MDA_VISION_MODULE_GATE () :
	_window (mvWindow("Gate Vision Module")),
	_HSVFilter (mvHSVFilter(MDA_VISION_GATE_SETTINGS)),
	_HoughLines (mvHoughLines(MDA_VISION_GATE_SETTINGS)),
	_lines (mvLines())
{
    _filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_GATE:: ~MDA_VISION_MODULE_GATE () {
    mvReleaseScratchImage();
}

void MDA_VISION_MODULE_GATE:: primary_filter (const IplImage* src) {
    _HSVFilter.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines.clearData ();
    _KMeans.clearData ();
    
    _HoughLines.findLines (_filtered_img, &_lines);
    _KMeans.cluster_auto (1, 2, &_lines);
    _KMeans.drawOntoImage (_filtered_img);

    _window.showImage (_filtered_img);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_GATE:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
    unsigned nClusters = _KMeans.nClusters();

    if (nClusters == 0) {
        printf ("Gate: No clusters =(\n");
        return NO_TARGET;
    }
    else if (nClusters == 1) {
        /// single line, not much we can do but return its center and estimate range
        DEBUG_PRINT ("Gate: 1 cluster =|\n");
        int x00 = _KMeans[0][0].x,   y00 = _KMeans[0][0].y;
        int x01 = _KMeans[0][1].x,   y01 = _KMeans[0][1].y;
        m_pixel_x = (x00 + x01 - _filtered_img->width)*0.5;       // centroid of line
        m_pixel_y = (y00 + y01 - _filtered_img->height)*0.5;
        unsigned line_height = abs(y01 - y00);
        unsigned line_width = abs(x01 - x00);

        /// check that the line is at least vertical
        if (line_height < 9.5*line_width) { // this is like +- 6 degrees
            DEBUG_PRINT ("Gate Sanity Failure: Single line not vertical enough\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;   
        }
    
        /// calculate range if we pass sanity check
        m_range = (GATE_REAL_HEIGHT * _filtered_img->height) / (line_height * TAN_FOV_Y);
        DEBUG_PRINT ("Gate Range: %d\n", m_range);

        retval = ONE_SEGMENT;       
        goto RETURN_CENTROID;
    }
    else {
        assert (nClusters == 2);
        DEBUG_PRINT ("Gate: 2 clusters =)\n");
        int x00 = _KMeans[0][0].x,   y00 = _KMeans[0][0].y;
        int x01 = _KMeans[0][1].x,   y01 = _KMeans[0][1].y;
        int x10 = _KMeans[1][0].x,   y10 = _KMeans[1][0].y;
        int x11 = _KMeans[1][1].x,   y11 = _KMeans[1][1].y;
        m_pixel_x = (int)((x00+x01+x10+x11)*0.25 - _filtered_img->width*0.5);
        m_pixel_y = (int)((y00+y01+y10+y11)*0.25 - _filtered_img->height*0.5); 
        
        /// sanity checks
        float gate_height_0 = abs(y01 - y00); // height of first 1ine
        float gate_height_1 = abs(y11 - y10); // height of second line
        float gate_width = (abs(x01 - x00) + abs(x11 - x10)) / 2;
        
        if (gate_height_0 > 1.3*gate_height_1 || 1.3*gate_height_0 < gate_height_1) {
            DEBUG_PRINT ("Gate Sanity Failure: Lines too dissimilar\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }
        if (gate_height_0 < 9.5*gate_width) { // this is like +- 6 degrees
            DEBUG_PRINT ("Gate Sanity Failure: Lines not vertical enough\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }
        
        /// calculations, treat center of image as 0,0   
        int gate_pixel_width = (int)( abs(x00+x01-x10-x11) * 0.5);
        int gate_pixel_height = (int)( (abs(y00-y01) + abs(y10-y11)) * 0.5);
        float gate_width_to_height_ratio = abs((float)gate_pixel_width / gate_pixel_height);
        if (gate_width_to_height_ratio > 1.3*GATE_WIDTH_TO_HEIGHT_RATIO || 1.3*gate_width_to_height_ratio < GATE_WIDTH_TO_HEIGHT_RATIO) {
            DEBUG_PRINT ("Gate Sanity Failure: Gate dimensions inconsistent with data\n");
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }

        // calculate real distances
        m_range = (GATE_REAL_WIDTH * _filtered_img->width) / (gate_pixel_width * TAN_FOV_X);
        DEBUG_PRINT ("Gate Range: %d\n", m_range);
        
        retval = FULL_DETECT;
        goto RETURN_CENTROID;
    }

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / _filtered_img->width);
        m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / _filtered_img->height);
        DEBUG_PRINT ("Gate: (%d,%d) (%5.2f,%5.2f)\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y); 
        return retval;
}
