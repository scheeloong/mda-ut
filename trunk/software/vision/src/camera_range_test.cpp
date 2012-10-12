#include "mgui.h"
#include "mda_vision.h"

class MDA_CAMERA_RANGE_TEST : public MDA_VISION_MODULE_BASE {
#define MODULE_SETTINGS "camera_range_test_settings.csv"
    static const float OBJ_REAL_WIDTH = 40.0;
    static const float OBJ_REAL_HEIGHT = 30.0;

    mvWindow* _window;
    mvHSVFilter* _HSVFilter;
    mvHoughLines* _HoughLines;
    mvLines* _lines;
    mvKMeans* _KMeans;
    
    IplImage* _filtered_img;
    
public:
    MDA_CAMERA_RANGE_TEST ();
    ~MDA_CAMERA_RANGE_TEST ();
    
    void primary_filter (const IplImage* src);
    void calc_vci (VCI* interface);
};

/** Main Function */


int main (int argc, char** argv) {
    MDA_CAMERA_RANGE_TEST test;
    mvCamera camera (0, 0);
    VCI interface;

    char c;
    IplImage* frame;

    for (;;) {
        frame = camera.getFrame();
        test.filter (frame, &interface);

        c = cvWaitKey (20);
        if (c == 'q')
           break;
    }

    return 0;
}

/** Class methods */

MDA_CAMERA_RANGE_TEST:: MDA_CAMERA_RANGE_TEST () {
    _filtered_img = mvCreateImage ();
    _window = new mvWindow ("Camera Range Test");
    _HSVFilter = new mvHSVFilter (MODULE_SETTINGS);
    _HoughLines = new mvHoughLines (MODULE_SETTINGS);
    _lines = new mvLines ();
    _KMeans = new mvKMeans ();
}

MDA_CAMERA_RANGE_TEST:: ~MDA_CAMERA_RANGE_TEST () {
    cvReleaseImage (&_filtered_img);
    delete _window;
    delete _HSVFilter;
    delete _HoughLines;
    delete _lines;
    delete _KMeans;
}

void MDA_CAMERA_RANGE_TEST:: primary_filter (const IplImage* src) {
    _HSVFilter->filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines->clearData ();
    
    _HoughLines->findLines (_filtered_img, _lines);
    _KMeans->cluster_auto (1, 2, _lines);
    _KMeans->drawOntoImage (_filtered_img);

    //_lines->drawOntoImage (_filtered_img);
    _window->showImage (_filtered_img);
}

void MDA_CAMERA_RANGE_TEST:: calc_vci (VCI* interface) {
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
        return;
    }
    if (dy0 < 9.5*dx0) { // this is like +- 6 degrees
        printf ("Gate Sanity Failure: Lines not vertical enough\n");
        return;
    }
    
    /// calculations, treat center of image as 0,0   
    int gate_pixel_width = (int)( abs(x00+x01-x10-x11) * 0.5);
    int gate_pixel_height = (int)( (abs(y00-y01) + abs(y10-y11)) * 0.5);

    int pixel_x = (int)((x00+x01+x10+x11)*0.25 - _filtered_img->width*0.5);
    int pixel_y = (int)((y00+y01+y10+y11)*0.25 - _filtered_img->height*0.5);   
    // calculate real distances
    const float pixel_to_real = OBJ_REAL_WIDTH / gate_pixel_width;
    const float pixel_to_real_y = OBJ_REAL_HEIGHT / gate_pixel_height;
    int real_x = pixel_to_real * pixel_x;
    int real_y = pixel_to_real * pixel_y;
    int range = (pixel_to_real * _filtered_img->width) / TAN_FOV_X;
    int range_y = (pixel_to_real_y * _filtered_img->height) / TAN_FOV_Y;
    printf ("Obj Real: (%d, %d),  range %d or %d.\n", real_x, real_y, range, range_y);
}


