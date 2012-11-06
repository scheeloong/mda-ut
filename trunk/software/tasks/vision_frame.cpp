#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

const char MDA_VISION_MODULE_FRAME::MDA_VISION_FRAME_SETTINGS[] = "vision_frame_settings.csv";

/// #########################################################################
/// MODULE_FRAME methods
/// #########################################################################
MDA_VISION_MODULE_FRAME:: MDA_VISION_MODULE_FRAME () :
    _window (mvWindow("Frame Vision Module")),
    _HSVFilter (mvHSVFilter(MDA_VISION_FRAME_SETTINGS)),
    _HoughLines (mvHoughLines(MDA_VISION_FRAME_SETTINGS)),
    _lines (mvLines())
{
    _filtered_img = mvCreateImage (); // common size
}

MDA_VISION_MODULE_FRAME:: ~MDA_VISION_MODULE_FRAME () {
    cvReleaseImage (&_filtered_img);
}

void MDA_VISION_MODULE_FRAME:: primary_filter (const IplImage* src) {
    _HSVFilter.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines.clearData ();
    _KMeans.clearData ();
    
    _HoughLines.findLines (_filtered_img, &_lines);
    _KMeans.cluster_auto (1, 3, &_lines, 1);

    _lines.drawOntoImage (_filtered_img);
    _KMeans.drawOntoImage (_filtered_img);

    _window.showImage (_filtered_img);
    cvWaitKey(3);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_FRAME:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
    unsigned nClusters = _KMeans.nClusters();

    if(nClusters == 0) {
        printf ("Frame: No clusters =(\n");
        return NO_TARGET;
    }
    if(nClusters == 1){
        int x1 = _KMeans[0][0].x, x2 = _KMeans[0][1].x;
        int y1 = _KMeans[0][0].y, y2 = _KMeans[0][1].y;    

        float denom = (x1==x2) ? 1 : abs(x2-x1);
        double slope = ((double)y1-y2)/denom;

        if (abs(slope) < 0.2) { // single horizontal line
            m_pixel_x = (x1-x2)/2;
            m_pixel_y = (y1-y2)/2;
            m_range = ((float)(FRAME_REAL_WIDTH) * _filtered_img->width) / ((x2-x1) * TAN_FOV_X);
            printf("Frame Range: %d\n", m_range);

            retval = ONE_SEGMENT;
            goto RETURN_CENTROID;
        }
        else if (abs(slope) > 6) { // single vertical line
            m_pixel_x = (x1-x2)/2;
            m_pixel_y = (y1-y2)/2;

            retval = ONE_SEGMENT;
            goto RETURN_CENTROID;
        }
        else { // unknown - raise error and return centroid
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }
    }
    else{
        printf("nClusters = %d unhandled\n", nClusters);
        return NO_TARGET;
    }

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / _filtered_img->width);
        m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / _filtered_img->height);
        DEBUG_PRINT ("Frame: (%d,%d) (%5.2f,%5.2f)\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y); 
        return retval;
}
