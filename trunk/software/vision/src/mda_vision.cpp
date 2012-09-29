#include "mda_vision.h"
/// ########################################################################
/// MODULE_TEST methods
/// ########################################################################
MDA_VISION_MODULE_TEST:: MDA_VISION_MODULE_TEST () {
    // these images are common size
    _filtered_img = mvCreateImage ();
    
    _window = new mvWindow ("Testing Vision Module");
    _HSVFilter = new mvHSVFilter (MDA_VISION_MODULE_TEST_SETTINGS);
    _HoughLines = new mvHoughLines (MDA_VISION_MODULE_TEST_SETTINGS);
    _lines = new mvLines ();
}

MDA_VISION_MODULE_TEST:: ~MDA_VISION_MODULE_TEST () {
    delete _filtered_img;
    delete _window;
    delete _HSVFilter;
    delete _HoughLines;
    delete _lines;
}

void MDA_VISION_MODULE_TEST:: primary_filter (const IplImage* src) {
    _HSVFilter->filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    
    _HoughLines->findLines (_filtered_img, _lines);
    _lines->drawOntoImage(_filtered_img);
    
    _window->showImage (_filtered_img);
    _lines->clearData ();
}

void MDA_VISION_MODULE_TEST:: calc_vci () {
    x = y = 1;
    range = 10;
}


/// ########################################################################
/// MODULE_GATE methods
/// ########################################################################
MDA_VISION_MODULE_GATE:: MDA_VISION_MODULE_GATE () {
}

MDA_VISION_MODULE_GATE:: ~MDA_VISION_MODULE_GATE () {
}

void MDA_VISION_MODULE_GATE:: primary_filter (const IplImage* src) {
}

void MDA_VISION_MODULE_GATE:: calc_vci () {
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

void MDA_VISION_MODULE_PATH:: calc_vci () {
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

void MDA_VISION_MODULE_BUOY:: calc_vci () {
}
