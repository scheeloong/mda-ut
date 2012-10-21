#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

/// ########################################################################
/// MODULE_TEST methods
/// ########################################################################
MDA_VISION_MODULE_TEST:: MDA_VISION_MODULE_TEST () {
    // these images are common size
    _filtered_img = mvCreateImage_Color ();
    _window = new mvWindow ("Testing Vision Module");
    _HSVFilter = new mvHSVFilter (MDA_VISION_MODULE_TEST_SETTINGS);
    _HoughLines = new mvHoughLines (MDA_VISION_MODULE_TEST_SETTINGS);
    _lines = new mvLines ();
}

MDA_VISION_MODULE_TEST:: ~MDA_VISION_MODULE_TEST () {
    cvReleaseImage (&_filtered_img);
    delete _window;
    delete _HSVFilter;
    delete _HoughLines;
    delete _lines;
}

void MDA_VISION_MODULE_TEST:: primary_filter (const IplImage* src) {   
    //_HSVFilter->filter (src, _filtered_img);

    /** src is an image that is passed in from the simulator. It is 3 channel
     *  Because it is const you may need to deep copy (not pointer copy) it 
     *  to your own IplImage first before you can modify it.
     *  I've done this with the variable img
     */
    IplImage* img = mvCreateImage_Color();
    cvCopy (src, img);
    img->origin = src->origin;

    /** YOUR CODE HERE. DO STUFF TO img */




    // this line displays the img in a window
    _window->showImage (img);
}

int MDA_VISION_MODULE_TEST:: calc_vci (VCI* interface) {
    return 0;
}


/// ########################################################################
/// MODULE_GATE methods
/// ########################################################################
MDA_VISION_MODULE_GATE:: MDA_VISION_MODULE_GATE () {
    _filtered_img = mvCreateImage (); // common size
    _window = new mvWindow ("Gate Vision Module");
    _HSVFilter = new mvHSVFilter (MDA_VISION_GATE_SETTINGS);
    _HoughLines = new mvHoughLines (MDA_VISION_GATE_SETTINGS);
    _KMeans = new mvKMeans ();
    _lines = new mvLines ();
}

MDA_VISION_MODULE_GATE:: ~MDA_VISION_MODULE_GATE () {
    cvReleaseImage (&_filtered_img);
    delete _window;
    delete _HSVFilter;
    delete _HoughLines;
    delete _KMeans;
    delete _lines;
}

void MDA_VISION_MODULE_GATE:: primary_filter (const IplImage* src) {
    _HSVFilter->filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines->clearData ();
    _KMeans->clearData ();
    
    _HoughLines->findLines (_filtered_img, _lines);
    _KMeans->cluster_auto (1, 2, _lines);
    _KMeans->drawOntoImage (_filtered_img);

    _window->showImage (_filtered_img);
}

int MDA_VISION_MODULE_GATE:: calc_vci (VCI* interface) {
    int retval = 0;
    int pixel_x = 0, pixel_y = 0;

    if (_KMeans->nClusters() == 0) {
        /// NO CLUSTERS
        printf ("Gate: No clusters =(\n");
        return -1;
    }
    else if (_KMeans->nClusters() == 1) {
        /// single line, not much we can do but return its center and estimate range
        DEBUG_PRINT ("Gate: 1 cluster =|\n");
        int x00 = (*_KMeans)[0][0].x,   y00 = (*_KMeans)[0][0].y;
        int x01 = (*_KMeans)[0][1].x,   y01 = (*_KMeans)[0][1].y;
        pixel_x = (x00 + x01 - _filtered_img->width)*0.5;       // centroid of line
        pixel_y = (y00 + y01 - _filtered_img->height)*0.5;
        unsigned line_height = abs(y01 - y00);
        unsigned line_width = abs(x01 - x00);

        /// check that the line is at least vertical
        if (line_height < 9.5*line_width) { // this is like +- 6 degrees
            DEBUG_PRINT ("Gate Sanity Failure: Single line not vertical enough\n");
            retval = 2;
            goto RETURN_CENTROID;   
        }
    
        /// calculate range if we pass sanity check
        interface->range = (GATE_REAL_HEIGHT * _filtered_img->height) / (line_height * TAN_FOV_Y);
        DEBUG_PRINT ("Gate Range: %d\n", interface->range);
        retval = 0;       
        goto RETURN_CENTROID;
    }
    else {
        assert (_KMeans->nClusters() == 2);
        DEBUG_PRINT ("Gate: 2 clusters =)\n");
        int x00 = (*_KMeans)[0][0].x,   y00 = (*_KMeans)[0][0].y;
        int x01 = (*_KMeans)[0][1].x,   y01 = (*_KMeans)[0][1].y;
        int x10 = (*_KMeans)[1][0].x,   y10 = (*_KMeans)[1][0].y;
        int x11 = (*_KMeans)[1][1].x,   y11 = (*_KMeans)[1][1].y;
        pixel_x = (int)((x00+x01+x10+x11)*0.25 - _filtered_img->width*0.5);
        pixel_y = (int)((y00+y01+y10+y11)*0.25 - _filtered_img->height*0.5); 
        
        /// sanity checks
        float gate_height_0 = abs(y01 - y00); // height of first 1ine
        float gate_height_1 = abs(y11 - y10); // height of second line
        float gate_width = (abs(x01 - x00) + abs(x11 - x10)) / 2;
        
        if (gate_height_0 > 1.3*gate_height_1 || 1.3*gate_height_0 < gate_height_1) {
            DEBUG_PRINT ("Gate Sanity Failure: Lines too dissimilar\n");
            retval = 2;
            goto RETURN_CENTROID;
        }
        if (gate_height_0 < 9.5*gate_width) { // this is like +- 6 degrees
            DEBUG_PRINT ("Gate Sanity Failure: Lines not vertical enough\n");
            retval = 2;
            goto RETURN_CENTROID;
        }
        
        /// calculations, treat center of image as 0,0   
        int gate_pixel_width = (int)( abs(x00+x01-x10-x11) * 0.5);
        int gate_pixel_height = (int)( (abs(y00-y01) + abs(y10-y11)) * 0.5);
        float gate_width_to_height_ratio = abs((float)gate_pixel_width / gate_pixel_height);
        if (gate_width_to_height_ratio > 1.3*GATE_WIDTH_TO_HEIGHT_RATIO || 1.3*gate_width_to_height_ratio < GATE_WIDTH_TO_HEIGHT_RATIO) {
            DEBUG_PRINT ("Gate Sanity Failure: Gate dimensions inconsistent with data\n");
            retval = 2;
            goto RETURN_CENTROID;
        }

        // calculate real distances
        interface->range = (GATE_REAL_WIDTH * _filtered_img->width) / (gate_pixel_width * TAN_FOV_X);
        DEBUG_PRINT ("Gate Range: %d\n", interface->range);
        retval = 0;
        goto RETURN_CENTROID;
    }

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        interface->angle_x = RAD_TO_DEG * atan(TAN_FOV_X * pixel_x / _filtered_img->width);
        interface->angle_y = RAD_TO_DEG * atan(TAN_FOV_Y * pixel_y / _filtered_img->height);
        DEBUG_PRINT ("Gate (pixels, degrees): <%d,%d> <%5.2f,%5.2f>\n", pixel_x, pixel_y, 
            interface->angle_x, interface->angle_y); 
        return retval;
}


/// ########################################################################
/// MODULE_PATH methods
/// ########################################################################
MDA_VISION_MODULE_PATH:: MDA_VISION_MODULE_PATH () {
    _filtered_img = mvCreateImage (); // common size
    _grad_img = mvCreateImage ();
    _window = new mvWindow ("Path Vision Module");
    _HSVFilter = new mvHSVFilter (MDA_VISION_PATH_SETTINGS);
    _HoughLines = new mvHoughLines (MDA_VISION_PATH_SETTINGS);
    _KMeans = new mvKMeans ();
    _lines = new mvLines ();
}

MDA_VISION_MODULE_PATH:: ~MDA_VISION_MODULE_PATH () {
    cvReleaseImage (&_filtered_img);
    cvReleaseImage (&_grad_img);
    delete _window;
    delete _HSVFilter;
    delete _HoughLines;
    delete _KMeans;
    delete _lines;
}

void MDA_VISION_MODULE_PATH:: primary_filter (const IplImage* src) {
    _HSVFilter->filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _grad_img->origin = src->origin;
    _lines->clearData ();
    _KMeans->clearData ();
    
    mvGradient (_filtered_img, _grad_img, 5, 5);
    _HoughLines->findLines (_grad_img, _lines);
    _KMeans->cluster_auto (1, 2, _lines, 1);

    _lines->drawOntoImage (_grad_img);
    _KMeans->drawOntoImage (_grad_img);

    _window->showImage (_grad_img);
}

int MDA_VISION_MODULE_PATH:: calc_vci (VCI* interface) {
    int retval = 0;
    int pixel_x = 0, pixel_y = 0;

    if (_KMeans->nClusters() == 0) {
        /// NO CLUSTERS
        printf ("Path: No clusters =(\n");
        return -1;
    }
    else if (_KMeans->nClusters() == 1) {
        /// single line, not much we can do but return its center and estimate range
        DEBUG_PRINT ("Path: 1 cluster =|\n");
        pixel_x = ((*_KMeans)[0][0].x + (*_KMeans)[0][1].x - _filtered_img->width)*0.5;       // centroid of line
        pixel_y = ((*_KMeans)[0][0].y + (*_KMeans)[0][1].y - _filtered_img->height)*0.5;
        unsigned length = line_sqr_length((*_KMeans)[0]);

        /// check that the line is near center of image
        
        /// calculate range if we pass sanity check
        interface->range = (PATH_REAL_LENGTH * _filtered_img->width) / (length * TAN_FOV_X);
        interface->angle = RAD_TO_DEG * line_angle_to_vertical((*_KMeans)[0]);
        DEBUG_PRINT ("Path Range = %d, Angle = %5.2f\n", interface->range, interface->angle);
        retval = 0;       
        goto RETURN_CENTROID;
    }
    else {
        assert (_KMeans->nClusters() == 2);
        DEBUG_PRINT ("Path: 2 clusters =)\n");
        int x00 = (*_KMeans)[0][0].x,   y00 = (*_KMeans)[0][0].y;
        int x01 = (*_KMeans)[0][1].x,   y01 = (*_KMeans)[0][1].y;
        int x10 = (*_KMeans)[1][0].x,   y10 = (*_KMeans)[1][0].y;
        int x11 = (*_KMeans)[1][1].x,   y11 = (*_KMeans)[1][1].y;
        pixel_x = (int)((x00+x01+x10+x11)*0.25 - _filtered_img->width*0.5);
        pixel_y = (int)((y00+y01+y10+y11)*0.25 - _filtered_img->height*0.5);

        float position_angle_0 = RAD_TO_DEG * line_angle_to_vertical((*_KMeans)[0]); 
        float position_angle_1 = RAD_TO_DEG * line_angle_to_vertical((*_KMeans)[1]);

        /// sanity checks
        unsigned length_0 = line_sqr_length((*_KMeans)[0]);
        unsigned length_1 = line_sqr_length((*_KMeans)[1]);
        
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

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        interface->angle_x = RAD_TO_DEG * atan(TAN_FOV_X * pixel_x / _filtered_img->width);
        interface->angle_y = RAD_TO_DEG * atan(TAN_FOV_Y * pixel_y / _filtered_img->height);
        DEBUG_PRINT ("Path (pixels, degrees): <%d,%d> <%5.2f,%5.2f>\n", pixel_x, pixel_y, 
            interface->angle_x, interface->angle_y); 
        return retval;
}


/// ########################################################################
/// MODULE_BUOY methods
/// ########################################################################
MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY () {
}

MDA_VISION_MODULE_BUOY:: ~MDA_VISION_MODULE_BUOY () {
}

void MDA_VISION_MODULE_BUOY:: primary_filter (const IplImage* src) {
}

int MDA_VISION_MODULE_BUOY:: calc_vci (VCI* interface) {
    return 0;
}

/// ########################################################################
/// MODULE_FRAME methods
/// ########################################################################
MDA_VISION_MODULE_FRAME:: MDA_VISION_MODULE_FRAME () {
    _filtered_img = mvCreateImage (); // common size
    _window = new mvWindow ("Frame Vision Module");
    _HSVFilter = new mvHSVFilter (MDA_VISION_FRAME_SETTINGS);
    _HoughLines = new mvHoughLines (MDA_VISION_FRAME_SETTINGS);
    _KMeans = new mvKMeans ();
    _lines = new mvLines ();
}

MDA_VISION_MODULE_FRAME:: ~MDA_VISION_MODULE_FRAME () {
    cvReleaseImage (&_filtered_img);
    delete _window;
    delete _HSVFilter;
    delete _HoughLines;
    delete _KMeans;
    delete _lines;
}

void MDA_VISION_MODULE_FRAME:: primary_filter (const IplImage* src) {
    _HSVFilter->filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines->clearData ();
    _KMeans->clearData ();
    
    _HoughLines->findLines (_filtered_img, _lines);
    _KMeans->cluster_auto (1, 3, _lines, 1);

    _lines->drawOntoImage (_filtered_img);
    _KMeans->drawOntoImage (_filtered_img);

    _window->showImage (_filtered_img);
}

int MDA_VISION_MODULE_FRAME:: calc_vci (VCI* interface) {
    return 0;
}
