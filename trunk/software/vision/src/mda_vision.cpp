#include "mda_vision.h"
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

    //_lines->drawOntoImage (_filtered_img);
    _window->showImage (_filtered_img);
}

int MDA_VISION_MODULE_GATE:: calc_vci (VCI* interface) {
    if (_KMeans->nClusters() == 0) {
        /// NO CLUSTERS
        printf ("No clusters =(\n");
        return -1;
    }
    else if (_KMeans->nClusters() == 1) {
        /// single line, not much we can do but return its center and estimate range
        int x00 = (*_KMeans)[0][0].x,   y00 = (*_KMeans)[0][0].y;
        int x01 = (*_KMeans)[0][1].x,   y01 = (*_KMeans)[0][1].y;
        float dy0 = abs(y01 - y00);
        float dx0 = abs(x01 - x00);

        /// check that the line is at least vertical
        if (dy0 < 9.5*dx0) { // this is like +- 6 degrees
            printf ("Gate Sanity Failure: Single line not vertical enough\n");
            return 2;
        }

        /// calculations
        const float pixel_to_real = GATE_REAL_HEIGHT / dy0;
        interface->real_x = (x00 + x01 - _filtered_img->width)*0.5;
        interface->real_y = (y00 + y01 - _filtered_img->height)*0.5;
        interface->range = (pixel_to_real * _filtered_img->height) / TAN_FOV_Y;
        return 1;
    }
    else {
        assert (_KMeans->nClusters() == 2);
    
        int x00 = (*_KMeans)[0][0].x,   y00 = (*_KMeans)[0][0].y;
        int x01 = (*_KMeans)[0][1].x,   y01 = (*_KMeans)[0][1].y;
        int x10 = (*_KMeans)[1][0].x,   y10 = (*_KMeans)[1][0].y;
        int x11 = (*_KMeans)[1][1].x,   y11 = (*_KMeans)[1][1].y;
        
        /// sanity checks
        float dy0 = abs(y01 - y00); // height of first 1ine
        float dx0 = abs(x01 - x00);
        float dy1 = abs(y11 - y10); // height of second line
        //float dx1 = abs(x11 - x10);
        
        if (dy0 > 1.3*dy1 || 1.3*dy0 < dy1) {
            printf ("Gate Sanity Failure: Lines too dissimilar\n");
            return 2;
        }
        if (dy0 < 9.5*dx0) { // this is like +- 6 degrees
            printf ("Gate Sanity Failure: Lines not vertical enough\n");
            return 2;
        }
        
        /// calculations, treat center of image as 0,0   
        int gate_pixel_width = (int)( abs(x00+x01-x10-x11) * 0.5);
        int gate_pixel_height = (int)( (abs(y00-y01) + abs(y10-y11)) * 0.5);
        float gate_width_to_height_ratio = abs((float)gate_pixel_width / gate_pixel_height);
        if (gate_width_to_height_ratio > 1.3*GATE_WIDTH_TO_HEIGHT_RATIO || 1.3*gate_width_to_height_ratio < GATE_WIDTH_TO_HEIGHT_RATIO) {
            printf ("Gate Sanity Failure: Gate dimensions inconsistent with data\n");
            return 3;
        }

        int pixel_x = (int)((x00+x01+x10+x11)*0.25 - _filtered_img->width*0.5);
        int pixel_y = (int)((y00+y01+y10+y11)*0.25 - _filtered_img->height*0.5);   
        printf ("Gate: (%d, %d),  %d X %d.\n", pixel_x, pixel_y, gate_pixel_width, gate_pixel_height);

        // calculate real distances
        const float pixel_to_real = GATE_REAL_WIDTH / gate_pixel_width;
        interface->real_x = pixel_to_real * pixel_x;
        interface->real_y = pixel_to_real * pixel_y;
        interface->range = (pixel_to_real * _filtered_img->width) / TAN_FOV_X;
        //printf ("Gate Real: (%d, %d),  range %d.\n", interface->real_x, interface->real_y, interface->range);
        return 0;
    }
}


/// ########################################################################
/// MODULE_PATH methods
/// ########################################################################
MDA_VISION_MODULE_PATH:: MDA_VISION_MODULE_PATH () {
}

MDA_VISION_MODULE_PATH:: ~MDA_VISION_MODULE_PATH () {
}

void MDA_VISION_MODULE_PATH:: primary_filter (const IplImage* src) {
}

int MDA_VISION_MODULE_PATH:: calc_vci (VCI* interface) {
    return 0;
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
