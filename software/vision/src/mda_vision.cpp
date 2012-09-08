#include "mda_vision.h"

MDA_VISION_MODULE_TEST:: MDA_VISION_MODULE_TEST () {
    // these images are common size
    _resized_img = mvCreateImage_Color ();
    _filtered_img = mvCreateImage (); 
    
    _window = new mvWindow ("Testing Vision Module");
    _HSVFilter = new mvHSVFilter (MDA_VISION_MODULE_TEST_SETTINGS);
    _HoughLines = new mvHoughLines (MDA_VISION_MODULE_TEST_SETTINGS);
    _lines = new mvLines ();
}

MDA_VISION_MODULE_TEST:: ~MDA_VISION_MODULE_TEST () {
    delete _window;
    delete _HSVFilter;
    delete _HoughLines;
    delete _lines;
}

void MDA_VISION_MODULE_TEST:: filter (const IplImage* src, IplImage* &dst) {
    assert (src != NULL);
    assert (src->nChannels == 3);

    cvResize (src, _resized_img); // resize src to common size
    _HSVFilter->filter (_resized_img, _filtered_img);
    _filtered_img->origin = src->origin;
    
    _HoughLines->findLines (_filtered_img, _lines);
    _lines->drawOntoImage(_filtered_img);
    
    _window->showImage (_filtered_img);
    _lines->clearData ();
    dst = _filtered_img;
}