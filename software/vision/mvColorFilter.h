#ifndef __MDA_VISION_MVCOLORFILTER__
#define __MDA_VISION_MVCOLORFILTER__

#include <cv.h>
#include "mv.h"
#include "mgui.h"

// ##################################################################################################
//  Utility Functions and Filters that are not classes
// ##################################################################################################

inline bool hue_in_range (unsigned char hue, int HMIN, int HMAX) { // helper function for the filter
    if (HMAX >= HMIN) 
        return (hue >= HMIN && hue <= HMAX);
    else
        return ((hue <= HMIN && hue <= HMAX) || (hue >= HMIN && hue >= HMAX)); 
}

void ManhattanDistanceFilter(IplImage* src, IplImage* dst);

// ##################################################################################################
//  HSVFilter - very basic filter that looks at each pixel and sees if it falls within user defined 
//              HSV bounds
// ##################################################################################################

/** HSV color limit filter */
// Currently support for changing the HSV values on the fly are limited
class mvHSVFilter {
    unsigned IMG_WIDTH, IMG_HEIGHT;
    int HMIN,HMAX;
    unsigned SMIN,SMAX, VMIN, VMAX;
    
    IplImage* scratch_3;

    PROFILE_BIN bin_WorkingLoop;
    PROFILE_BIN bin_CvtColor;
    
    void filter_internal (IplImage* HSV_img, IplImage* result);

    public:
    mvHSVFilter (const char* settings_file);
    ~mvHSVFilter ();
    
    void filter (IplImage* img, IplImage* result) {
        bin_CvtColor.start();
        cvCvtColor (img, scratch_3, CV_BGR2HSV); // convert to HSV 
        bin_CvtColor.stop();

        filter_internal (scratch_3, result);
    }

    void filter_non_common_size (IplImage* img, IplImage* result) {
        IplImage * temp_scratch; // scratch_3 is common size. temp_scratch will be same size as img
        temp_scratch = mvCreateImage_Color (img);

        bin_CvtColor.start();
        cvCvtColor (img, temp_scratch, CV_BGR2HSV); // convert to HSV 
        bin_CvtColor.stop();

        filter_internal (temp_scratch, result);

        cvReleaseImage (&temp_scratch);
    }   
};

// ##################################################################################################
//  mvHistogramFilter - uses histogram bins to determine best H,S bounds
// ##################################################################################################
class mvHistogramFilter {
    static const int nbins_hue = 30;
    static const int nbins_sat = 25;
    static const int hue_range_min = 0;
    static const int hue_range_max = 179;
    static const int sat_range_min = 6;
    static const int sat_range_max = 255;
    static const int MAX_BINS_MARKED = 9;
    static const int HISTOGRAM_NORM_FACTOR = 100000;
    static const bool DISPLAY_HIST = true;
    static const int NUM_SIDES_RECTANGLE = 4;


    // a "Quad" is a collection of histogram bins in 2D. So a quad that goes from Hue=[0,20] Sat=[50,80]
    // will contain all histogram bins with those H,S values
    struct Quad{
        int h0, s0, h1, s1;
    };

    int hue_min;
    int hue_max;
    int sat_min;
    int sat_max;
    int val_min;
    int val_max;

    IplImage* HSV_img; // stores the image but converted to HSV
    IplImage* hue_img; 
    IplImage* sat_img;
    IplImage* hist_img; // used to display the histogram if needed

    CvHistogram* hist; 

    PROFILE_BIN bin_Hist;
    PROFILE_BIN bin_CvtColor;
    mvWindow* win;

    private:
    void setQuad (Quad &Q, int h0, int s0, int h1, int s1); // set boundaries on a quad
    int getQuadAvgCount (Quad Q);                              // get average bin count
    void getRectangleNeighbours(Quad rect, Quad sides[]);

    public:
    mvHistogramFilter (const char* settings_file);
    ~mvHistogramFilter ();
    void filter (IplImage* src, IplImage* dst);
    void print_histogram ();
    void show_histogram ();
};

// ##################################################################################################
//  Hue_Box - helper class for mvAdvancedColorFilter
// ##################################################################################################
class Hue_Box {
public:
    unsigned char HUE_MIN;
    unsigned char HUE_MAX;
    unsigned char SAT_MIN;
    unsigned char SAT_MAX;
    unsigned char VAL_MIN;
    unsigned char VAL_MAX;
    int BOX_NUMBER;
    int BOX_COLOR;
    bool BOX_ENABLED; // whether the box is being used or not

    static const int HUE_GUTTER_LEN = 6;
    static const int HUE_ADP_LEN = 2;
    static const float RESET_THRESHOLD_FRACTION = 0.0005;
    unsigned char HUE_MIN_OUT, HUE_MAX_OUT;     // outer hue limits for expanding the box
    unsigned char HUE_MIN_IN, HUE_MAX_IN;       // inner hue limits for contracting the box
    unsigned char HUE_MIN_ADP, HUE_MAX_ADP;     // normal hue limits
    int inner_count;                            // num of pixels within the inner box
    int min_inside_count, max_inside_count;     // num of pixels within the normal box
    int min_outside_count, max_outside_count;   // num of pixels within the outside box

    Hue_Box (const char* settings_file, int box_number);

    bool check_hsv (unsigned char hue, unsigned char sat, unsigned char val) {
        // shifting logic goes here
        if (sat >= SAT_MIN && sat <= SAT_MAX && val >= VAL_MIN && val <= VAL_MAX) {
            if (HUE_MAX >= HUE_MIN) 
                return (hue >= HUE_MIN && hue <= HUE_MAX);
            else
                return ((hue <= HUE_MIN && hue <= HUE_MAX) || (hue >= HUE_MIN && hue >= HUE_MAX)); 
        }
        return false;
    }

    bool check_hsv_adaptive_hue (unsigned char hue, unsigned char sat, unsigned char val);
    void update_hue ();

    bool is_enabled () {
        return BOX_ENABLED;
    }
};

// ##################################################################################################
//  Color_Triple - helper class for mvAdvancedColorFilter
// ##################################################################################################
class Color_Triple {
public:
    unsigned m1;
    unsigned m2;
    unsigned m3;
    unsigned n_pixels;
    unsigned index_number;

    static const int NUM_HISTOGRAM_BINS = 5;
    unsigned m1_histogram[NUM_HISTOGRAM_BINS];
    unsigned m2_histogram[NUM_HISTOGRAM_BINS];
    unsigned m3_histogram[NUM_HISTOGRAM_BINS];

    Color_Triple(){
        m1 = m2 = m3 = n_pixels = index_number = 0;
        init_histograms();
    }
    Color_Triple(unsigned M1, unsigned M2, unsigned M3, unsigned Index_Number){
        m1 = M1; 
        m2 = M2;
        m3 = M3;
        n_pixels = 1;
        index_number = Index_Number;
        init_histograms();
    }
    void init_histograms() {
        for (int i = 0; i < NUM_HISTOGRAM_BINS; i++) {
            m1_histogram[i] = 0;
            m2_histogram[i] = 0;
            m3_histogram[i] = 0;
        }
    }
    void calc_average () {
        m1 /= n_pixels;
        m2 /= n_pixels;
        m3 /= n_pixels;
    }
    void BGR_to_HSV () {
        unsigned char H,S,V;
        tripletBGR2HSV (static_cast<unsigned char>(m1),static_cast<unsigned char>(m2),static_cast<unsigned char>(m3)
                        ,H,S,V); // m1,m2,m3 are actually in BGR
        m1 = static_cast<unsigned>(H);
        m2 = static_cast<unsigned>(S);
        m3 = static_cast<unsigned>(V);
    }
    void merge (Color_Triple B) {
        unsigned total = n_pixels + B.n_pixels;
        m1 = (m1*n_pixels + B.m1*B.n_pixels) / total;
        m2 = (m2*n_pixels + B.m2*B.n_pixels) / total;
        m3 = (m3*n_pixels + B.m3*B.n_pixels) / total;
        for (int i = 0; i < NUM_HISTOGRAM_BINS; i++) {
            m1_histogram[i] += B.m1_histogram[i];
            m2_histogram[i] += B.m2_histogram[i];
            m3_histogram[i] += B.m3_histogram[i];
        }
        n_pixels = total;
    }
    void print () {
        printf ("color_triplet #%2d (%d pixels): %d %d %d\n", index_number, n_pixels, m1, m2, m3);
    }
    void print_with_stats () {
        printf ("color_triplet #%2d (%d pixels):\n", index_number, n_pixels);
        printf ("\t%3d\t\t", m1);
        for (int i = 0; i < Color_Triple::NUM_HISTOGRAM_BINS; i++) {
            printf ("%4.2f ", (float)m1_histogram[i]/n_pixels);
        }    
        printf ("\n");
        printf ("\t%3d\t\t", m2);
        for (int i = 0; i < Color_Triple::NUM_HISTOGRAM_BINS; i++) {
            printf ("%4.2f ", (float)m2_histogram[i]/n_pixels);
        }    
        printf ("\n");
        printf ("\t%3d\t\t", m3);
        for (int i = 0; i < Color_Triple::NUM_HISTOGRAM_BINS; i++) {
            printf ("%4.2f ", (float)m3_histogram[i]/n_pixels);
        }    
        printf ("\n");
    }
};

// ##################################################################################################
//  mvAdvancedColorFilter - mean_shift, flood_fill, and other more complicated algorithms
// ##################################################################################################
class mvAdvancedColorFilter {

    //declare constants here
    static const int DS_FACTOR = 1; // downsampling
    static const int GOOD_PIXELS_FACTOR = 6;
    static const int KERNEL_SIZE = 7;
    static const int S_MIN = 60;
    static const int V_MIN = 30;
    static const unsigned char BAD_PIXEL = 253;
    static const unsigned char TEMP_PIXEL = 251;

public:
    static const int NUM_BOXES = 3;

private:
    // parameters read from settings file
    int COLOR_DIST;
    int H_DIST;
    int S_DIST;
    int V_DIST;

    // calculated parameters
    int KERNEL_AREA;
    int KERNEL_RAD;

    // internal data
    Hue_Box* hue_box[NUM_BOXES]; // array of pointers to boxes
    std::vector<Color_Triple> color_triple_vector;
    int* kernel_point_array;
    IplImage* ds_scratch_3;   // downsampled scratch image 3 channel
    IplImage* ds_scratch;   // 1 channel

    // these variables are used to support flood_image_interactive_callback


    // profile bins
    PROFILE_BIN bin_Resize;
    PROFILE_BIN bin_MeanShift;
    PROFILE_BIN bin_Filter;
    
    void downsample_from(IplImage* src) {    // downsamples src to internal scratch image
        assert (src->nChannels == 3);
        bin_Resize.start();
          cvResize (src, ds_scratch_3, CV_INTER_NN);
        bin_Resize.stop();
    }
    void upsample_to_3 (IplImage* dst) {     // upsamples internal scrach to dst
        assert (dst->nChannels == 3);
        bin_Resize.start();
          cvResize (ds_scratch_3, dst, CV_INTER_NN);
        bin_Resize.stop();
    }
    void upsample_to (IplImage* dst) {     // upsamples internal scrach to dst
        assert (dst->nChannels == 1);
        bin_Resize.start();
          cvResize (ds_scratch, dst, CV_INTER_NN);
        bin_Resize.stop();
    }

    bool check_and_accumulate_pixel (unsigned char* pixel, unsigned char* ref_pixel,
                                    Color_Triple &triple);
    bool check_and_accumulate_pixel_BGR (unsigned char* pixel, unsigned char* ref_pixel,
                                    Color_Triple &triple);
    bool check_and_accumulate_pixel_HSV (unsigned char* pixel, unsigned char* ref_pixel,
                                    Color_Triple &triple);
    
    void meanshift_internal(IplImage* scratch);
    void colorfilter_internal();
    void colorfilter_internal_adaptive_hue();
    void flood_image_internal ();
    bool flood_from_pixel(int r, int c, unsigned index_number);

public: 
    mvAdvancedColorFilter (const char* settings_file); //constructor
    ~mvAdvancedColorFilter(); // destructor
    void mean_shift(IplImage* src, IplImage* dst);
    void flood_image(IplImage* src, IplImage* dst, bool interactive=false);
    void watershed(IplImage* src, IplImage* dst);
    void filter(IplImage *src, IplImage* dst);
    void combined_filter(IplImage *src, IplImage* dst);


    friend void flood_image_interactive_callback(int event, int x, int y, int flags, void* param);
};

void flood_image_interactive_callback(int event, int x, int y, int flags, void* param);

#endif
