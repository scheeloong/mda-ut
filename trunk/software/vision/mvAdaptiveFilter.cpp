#include "mv.h"
#include <math.h>

#define FILTER_DEBUG
#ifdef FILTER_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

#define DISPLAY_HIST 1
#define HIST_PIXEL_SCALE 10

//####################################################################################
//####################################################################################
//####################################################################################

mvAdaptiveFilter3:: mvAdaptiveFilter3 (const char* settings_file) :
    bin_adaptive ("Adaptive3 - Logic"),
    bin_CvtColor ("Adaptive3 - CvtColor")
{
    read_mv_setting (settings_file, "HUE_MIN", hue_min);
    read_mv_setting (settings_file, "HUE_MAX", hue_max);
    read_mv_setting (settings_file, "SAT_MIN", sat_min);
    read_mv_setting (settings_file, "SAT_MAX", sat_max);
    read_mv_setting (settings_file, "VAL_MIN", val_min);
    read_mv_setting (settings_file, "VAL_MAX", val_max);

    src_HSV = mvGetScratchImage_Color();
    hue_img = mvCreateImageHeader();
    sat_img = mvCreateImageHeader();

    if (DISPLAY_HIST) {
        hist_img = cvCreateImage(
            cvSize(nbins_hue*HIST_PIXEL_SCALE, nbins_sat*HIST_PIXEL_SCALE),
            IPL_DEPTH_8U,
            1
        );
    }
    else
        hist_img = NULL;

    win = new mvWindow ("Adaptive Filter 3");

    int size[] = {nbins_hue, nbins_sat};
    float hue_range[] = {hue_range_min, hue_range_max};
    float sat_range[] = {sat_range_min, sat_range_max};
    float* ranges[] = {hue_range, sat_range};

    hist = cvCreateHist (
        2,              //dims
        size,           // num of bins per dims
        CV_HIST_ARRAY,  // dense matrix
        ranges,         // upper & lower bound for bins
        1               // uniform
    );
}

mvAdaptiveFilter3:: ~mvAdaptiveFilter3 () {
    delete win;
    cvReleaseHist (&hist);
    mvReleaseScratchImage_Color();
    cvReleaseImageHeader(&hue_img);
    cvReleaseImageHeader(&sat_img);
    if (DISPLAY_HIST)
        cvReleaseImage(&hist_img);
}

void mvAdaptiveFilter3:: setQuad (Quad &Q, int h0, int s0, int h1, int s1) {
    Q.h0 = h0; Q.s0 = s0;
    Q.h1 = h1; Q.s1 = s1;
}

int mvAdaptiveFilter3:: getQuadValue (Quad Q) {
    if (Q.s0 == -1)
        return -1;

    int count = 0;
    int num_bins = 0;
    for (int h = Q.h0; ; h++) {
        if (h >= nbins_hue) h = 0;  // allows looping of hue from bin16 to bin2, ect
        for (int s = Q.s0; s <= Q.s1; s++) {
            count += cvQueryHistValue_2D (hist, h, s);
            num_bins++;   
        }
        if(h == Q.h1) break;
    }
    return count/num_bins;
}

void mvAdaptiveFilter3:: filter (const IplImage* src, IplImage* dst) {
    assert (src != NULL);
    assert (dst != NULL);
    assert (src->nChannels == 3);
    assert (dst->nChannels == 1);
    assert (src->width == src_HSV->width);
    assert (src->height == src_HSV->height);

    /// convert imageto HSV  
    bin_CvtColor.start();
    cvCvtColor (src, src_HSV, CV_BGR2HSV); // convert to Hue,Saturation,Value 
    bin_CvtColor.stop();

    /// Mark all invalid (Value not within limits) pixels as Hue=255 and Sat = 0 which is outside histogram range
    bin_adaptive.start();
    unsigned char *imgPtr;
    for (int i = 0; i < src_HSV->height; i++) {
        imgPtr = (unsigned char*) src_HSV->imageData + i*src_HSV->widthStep;
        for (int j = 0; j < src_HSV->width; j++) {
            if ((*(imgPtr+2) < val_min) || (*(imgPtr+2) > val_max)) {
                *imgPtr = 255;
                *(imgPtr+1) = 0;
            }
            imgPtr+=3;
        }
    }

    /// in place split of the image
    mvSplitImage (src_HSV, &hue_img, &sat_img);

    /// genereate the hue-sat histogram and normalize it 
    IplImage* planes[] = {hue_img, sat_img};
    cvCalcHist (
        planes, hist,
        0,      // accumulate
        NULL    // possible boolean mask image
    ); 

    cvNormalizeHist (hist, HISTOGRAM_NORM_FACTOR);
    
    #ifdef FILTER_DEBUG
        print_histogram();
        show_histogram();
    #endif

    DEBUG_PRINT ("\nBeginning Histogram Filter Algorithm\n");

    /// Attempt to find a local maximum within the user-defined hue-sat bounds
    int bin_min_index_hue = hue_min*nbins_hue / (hue_range_max - hue_range_min);
    int bin_max_index_hue = hue_max*nbins_hue / (hue_range_max - hue_range_min) - 1;
    int bin_min_index_sat = sat_min*nbins_sat / (sat_range_max - sat_range_min);
    int bin_max_index_sat = sat_max*nbins_sat / (sat_range_max - sat_range_min) - 1;
    DEBUG_PRINT ("Bins Searched: Hue (%d-%d)  Sat (%d-%d)\n", bin_min_index_hue, bin_max_index_hue, bin_min_index_sat, bin_max_index_sat);
    
    unsigned local_max_bin_index[2] = {0,0};
    unsigned local_max_bin_value = 0;
    for (int h = bin_min_index_hue; ; h++) {
        if (h >= nbins_hue) h = 0;  // allows looping of hue from bin16 to bin2, ect

        for (int s = bin_min_index_sat; s <= bin_max_index_sat; s++) {
            unsigned bin_value = cvQueryHistValue_2D (hist, h, s);
            if (bin_value > local_max_bin_value) {
                local_max_bin_value = bin_value;
                local_max_bin_index[0] = h;
                local_max_bin_index[1] = s;
            }
        }

        if(h == bin_max_index_hue) break;
    }

    if (local_max_bin_value == 0) { // no non-zero bins in range
        printf ("All bins in search range are zero!\n");
        bin_adaptive.stop();
        return;
    }

    DEBUG_PRINT ("Local Max at bin (%d, %d)\n", local_max_bin_index[0], local_max_bin_index[1]);

    /// now define rect, which is the rectangle in BinIndex space representing the accepted H-S window
    Quad rect;
    setQuad (rect, local_max_bin_index[0], local_max_bin_index[1], local_max_bin_index[0], local_max_bin_index[1]);
    Quad sides[NUM_SIDES_RECTANGLE];

    DEBUG_PRINT ("Accumulating Bins\n");
    bool successful = false;

    for(int i = 0; i < 4; i++){
        DEBUG_PRINT ("  Iteration %d\n", i);
        
        /// Obtain the BinIndex space representation of the 4 sides of the rect
        getRectangleNeighbours (rect, sides);
        int rect_val = getQuadValue(rect);

        /// find the avg value of the squares on the sides.
        int side_val[NUM_SIDES_RECTANGLE];
        int side_val_avg = 0;
        int num_valid_sides = 0;
        for (int j = 0; j < NUM_SIDES_RECTANGLE; j++) {
            side_val[j] = getQuadValue(sides[j]);
            if (side_val[j] >= 0) {
                num_valid_sides++;
                side_val_avg += side_val[j];
            }
        }

        side_val_avg /= num_valid_sides;
   
        DEBUG_PRINT ("    rect: (%d,%d,%d,%d) - %d\n", rect.h0, rect.s0, rect.h1, rect.s1, rect_val);
        for (int j = 0; j < NUM_SIDES_RECTANGLE; j++){
            DEBUG_PRINT("    side: (%d,%d,%d,%d) - %d\n", sides[j].h0, sides[j].s0, sides[j].h1, sides[j].s1, side_val[j]);
        }        
        DEBUG_PRINT ("    side_avg: %d\n", side_val_avg);

        /// If the avg value of the sides >> value inside the rect, thenthe algorithm is done
        if (rect_val > 8*side_val_avg) {
            DEBUG_PRINT ("  Break on iteration %d\n", i);
            successful = true;
            break;
        }

        /// Otherwise add the best side to the rectangle
        int best_side = -1;
        int best_side_val = -1;
        for (int j = 0; j < NUM_SIDES_RECTANGLE; j++) {
            if (side_val[j] >= 0 && side_val[j] > best_side_val) {
                best_side = j;
                best_side_val = side_val[j];
            }
        }
        if (best_side == 0 || best_side == 3) {
            rect.h0 = sides[best_side].h0; 
            rect.s0 = sides[best_side].s0;
        }
        else {
            rect.h1 = sides[best_side].h1;
            rect.s1 = sides[best_side].s1;
        }
    }

    /// Calculate the H-S space coordinates of the allowed window using the BinIndex values
    unsigned hue_min_hist = (unsigned)rect.h0 * hue_range_max / nbins_hue;
    unsigned hue_max_hist = (unsigned)(rect.h1+1) * hue_range_max / nbins_hue;
    unsigned sat_min_hist = (unsigned)rect.s0 * sat_range_max / nbins_sat;
    unsigned sat_max_hist = (unsigned)(rect.s1+1) * sat_range_max / nbins_sat;
    DEBUG_PRINT ("Hue Range: %d-%d\n", hue_min_hist, hue_max_hist);
    DEBUG_PRINT ("Sat Range: %d-%d\n", sat_min_hist, sat_max_hist);

    /// if successful, generate dst image
    unsigned count = 0;
    if (successful) {
        unsigned char *dstPtr, *huePtr, *satPtr;
        for (int i = 0; i < dst->height; i++) {
            dstPtr = (unsigned char*) dst->imageData + i*dst->widthStep;
            huePtr = (unsigned char*) hue_img->imageData + i*hue_img->widthStep;
            satPtr = (unsigned char*) sat_img->imageData + i*sat_img->widthStep;

            for (int j = 0; j < dst->width; j++) {
                if ((*huePtr != 255) && hue_in_range (*huePtr, hue_min_hist, hue_max_hist) && 
                    (*satPtr >= sat_min_hist && *satPtr <= sat_max_hist))
                {
                    *dstPtr = 255;
                    count++;
                }
                else {                    
                    *dstPtr = 0;
                }
                
                dstPtr++; 
                huePtr++;
                satPtr++;
            }
        }
    }
    else {
        cvZero (dst);
    }

    DEBUG_PRINT ("Allowed %d pixels\n", count);
    bin_adaptive.stop();
}

void mvAdaptiveFilter3:: print_histogram () {
    int hist_height = hist->mat.dim[0].size;
    int hist_width = hist->mat.dim[1].size;
    printf ("\nprint_histogram():\n");

    for (int i = 0; i < hist_height; i++) {
        for (int j = 0; j < hist_width; j++) {
            int binval = cvQueryHistValue_2D(hist, i,j);
            unsigned hue_min = (unsigned)i * hue_range_max / nbins_hue;
            unsigned hue_max = (unsigned)(i+1) * hue_range_max / nbins_hue;
            unsigned sat_min = (unsigned)j * sat_range_max / nbins_sat;
            unsigned sat_max = (unsigned)(j+1) * sat_range_max / nbins_sat;
        
            if (binval > 0)
                printf ("Bin(%2d,%2d) HS(%d-%d,%d-%d) count=%d\n",i,j, hue_min,hue_max,sat_min,sat_max, binval);
        }
    }
}

void mvAdaptiveFilter3:: show_histogram () {
    cvZero (hist_img);
    
    float max;
    cvGetMinMaxHistValue (hist, 0, &max, 0, 0);

    int hist_height = hist->mat.dim[0].size;
    int hist_width = hist->mat.dim[1].size;

    for (int i = 0; i < hist_height; i++) {
        for (int j = 0; j < hist_width; j++) {
            float binval = cvQueryHistValue_2D(hist, i,j);
            int intensity = cvRound (sqrt(binval/max) * 255);

            cvRectangle (
                hist_img,
                cvPoint (j*HIST_PIXEL_SCALE, i*HIST_PIXEL_SCALE), // x coord first
                cvPoint ((j+1)*HIST_PIXEL_SCALE, (i+1)*HIST_PIXEL_SCALE),
                CV_RGB(intensity,intensity,intensity),
                CV_FILLED
            );
        }
    }

    win->showImage(hist_img);
}

void mvAdaptiveFilter3:: getRectangleNeighbours(Quad rect, Quad sides[]){
    int h0 = rect.h0;
    int s0 = rect.s0;
    int h1 = rect.h1;
    int s1 = rect.s1;

    setQuad(sides[0], (h0>0) ? h0-1: nbins_hue-1, s0, (h0>0) ? h0-1: nbins_hue-1, s1);
    setQuad(sides[1], h0, (s1<nbins_sat-1) ? s1+1 : -1, h1, (s1<nbins_sat-1) ? s1+1: -1);
    setQuad(sides[2], (h1<nbins_hue-1) ? h1+1: 0, s0, (h1<nbins_hue-1) ? h1+1: 0, s1);
    setQuad(sides[3], h0, (s0>0)? s0-1: -1, h1, (s0>0)? s0-1: -1);


}

// assumptions:
// Hue stays ~ the same in all lighting conditions
// Saturation can vary, but always greater than a certain threshold

#define IN_SRC(x) (((x)>=src_addr_first) && ((x)<src_addr_last))

// Get the octant a coordinate pair is in.
#define OCTANTIFY(_x, _y, _o)   do {                            \
    int _t; _o= 0;                                              \
    if(_y<  0)  {            _x= -_x;   _y= -_y; _o += 4; }     \
    if(_x<= 0)  { _t= _x;    _x=  _y;   _y= -_t; _o += 2; }     \
    if(_x<=_y)  { _t= _y-_x; _x= _x+_y; _y=  _t; _o += 1; }     \
} while(0);

// Some constants for dealing with atanLUT.
static const unsigned int ATAN_ONE = 0x1000, ATAN_FP= 12;
static const unsigned int ATANLUT_STRIDE = ATAN_ONE / 0x80, ATANLUT_STRIDE_SHIFT= 5;
static const unsigned int BRAD_PI_SHIFT=14,   BRAD_PI = 1<<BRAD_PI_SHIFT;
static const unsigned int BRAD_HPI= BRAD_PI/2, BRAD_2PI= BRAD_PI*2; 

static inline int QDIV(int num, int den, const int bits)
{
    return (((int64_t)num << 16) / (int64_t)den);
}

// Arctangens LUT. Interval: [0, 1] (one=128); PI=0x20000
const unsigned short atanLUT[130]
{
    0x0000,0x0146,0x028C,0x03D2,0x0517,0x065D,0x07A2,0x08E7,
    0x0A2C,0x0B71,0x0CB5,0x0DF9,0x0F3C,0x107F,0x11C1,0x1303,
    0x1444,0x1585,0x16C5,0x1804,0x1943,0x1A80,0x1BBD,0x1CFA,
    0x1E35,0x1F6F,0x20A9,0x21E1,0x2319,0x2450,0x2585,0x26BA,
    0x27ED,0x291F,0x2A50,0x2B80,0x2CAF,0x2DDC,0x2F08,0x3033,
    0x315D,0x3285,0x33AC,0x34D2,0x35F6,0x3719,0x383A,0x395A,
    0x3A78,0x3B95,0x3CB1,0x3DCB,0x3EE4,0x3FFB,0x4110,0x4224,
    0x4336,0x4447,0x4556,0x4664,0x4770,0x487A,0x4983,0x4A8B,
// 64
    0x4B90,0x4C94,0x4D96,0x4E97,0x4F96,0x5093,0x518F,0x5289,
    0x5382,0x5478,0x556E,0x5661,0x5753,0x5843,0x5932,0x5A1E,
    0x5B0A,0x5BF3,0x5CDB,0x5DC1,0x5EA6,0x5F89,0x606A,0x614A,
    0x6228,0x6305,0x63E0,0x64B9,0x6591,0x6667,0x673B,0x680E,
    0x68E0,0x69B0,0x6A7E,0x6B4B,0x6C16,0x6CDF,0x6DA8,0x6E6E,
    0x6F33,0x6FF7,0x70B9,0x717A,0x7239,0x72F6,0x73B3,0x746D,
    0x7527,0x75DF,0x7695,0x774A,0x77FE,0x78B0,0x7961,0x7A10,
    0x7ABF,0x7B6B,0x7C17,0x7CC1,0x7D6A,0x7E11,0x7EB7,0x7F5C,
// 128
    0x8000,0x80A2
};

const short sinLUT[180]
{
    0x0000,0x0478,0x08EE,0x0D61,0x11D0,0x163A,0x1A9D,0x1EF7,
    0x2348,0x278E,0x2BC7,0x2FF3,0x3410,0x381D,0x3C18,0x4000,
    0x43D4,0x4794,0x4B3D,0x4ECE,0x5247,0x55A6,0x58EB,0x5C13,
    0x5F1F,0x620E,0x64DE,0x678E,0x6A1E,0x6C8D,0x6EDA,0x7104,
    0x730C,0x74EF,0x76AE,0x7848,0x79BC,0x7B0B,0x7C33,0x7D34,
    0x7E0E,0x7EC1,0x7F4C,0x7FB0,0x7FEC,0x8000,0x7FEC,0x7FB0,
    0x7F4C,0x7EC1,0x7E0E,0x7D34,0x7C33,0x7B0B,0x79BC,0x7848,
    0x76AE,0x74EF,0x730C,0x7104,0x6EDA,0x6C8D,0x6A1E,0x678E,
    0x64DE,0x620E,0x5F1F,0x5C13,0x58EB,0x55A6,0x5247,0x4ECE,
    0x4B3D,0x4794,0x43D4,0x4000,0x3C18,0x381D,0x3410,0x2FF3,
    0x2BC7,0x278E,0x2348,0x1EF7,0x1A9D,0x163A,0x11D0,0x0D61,
    0x08EE,0x0478,0x0000,0xFB88,0xF712,0xF29F,0xEE30,0xE9C6,
    0xE563,0xE109,0xDCB8,0xD872,0xD439,0xD00D,0xCBF0,0xC7E3,
    0xC3E8,0xC000,0xBC2C,0xB86C,0xB4C3,0xB132,0xADB9,0xAA5A,
    0xA715,0xA3ED,0xA0E1,0x9DF2,0x9B22,0x9872,0x95E2,0x9373,
    0x9126,0x8EFC,0x8CF4,0x8B11,0x8952,0x87B8,0x8644,0x84F5,
    0x83CD,0x82CC,0x81F2,0x813F,0x80B4,0x8050,0x8014,0x8000,
    0x8014,0x8050,0x80B4,0x813F,0x81F2,0x82CC,0x83CD,0x84F5,
    0x8644,0x87B8,0x8952,0x8B11,0x8CF4,0x8EFC,0x9126,0x9373,
    0x95E2,0x9872,0x9B22,0x9DF2,0xA0E1,0xA3ED,0xA715,0xAA5A,
    0xADB9,0xB132,0xB4C3,0xB86C,0xBC2C,0xC000,0xC3E8,0xC7E3,
    0xCBF0,0xD00D,0xD439,0xD872,0xDCB8,0xE109,0xE563,0xE9C6,
    0xEE30,0xF29F,0xF712,0xFB88
};

const short cosLUT[180]
{
    0x8000,0x7FEC,0x7FB0,0x7F4C,0x7EC1,0x7E0E,0x7D34,0x7C33,
    0x7B0B,0x79BC,0x7848,0x76AE,0x74EF,0x730C,0x7104,0x6EDA,
    0x6C8D,0x6A1E,0x678E,0x64DE,0x620E,0x5F1F,0x5C13,0x58EB,
    0x55A6,0x5247,0x4ECE,0x4B3D,0x4794,0x43D4,0x4000,0x3C18,
    0x381D,0x3410,0x2FF3,0x2BC7,0x278E,0x2348,0x1EF7,0x1A9D,
    0x163A,0x11D0,0x0D61,0x08EE,0x0478,0x0000,0xFB88,0xF712,
    0xF29F,0xEE30,0xE9C6,0xE563,0xE109,0xDCB8,0xD872,0xD439,
    0xD00D,0xCBF0,0xC7E3,0xC3E8,0xC000,0xBC2C,0xB86C,0xB4C3,
    0xB132,0xADB9,0xAA5A,0xA715,0xA3ED,0xA0E1,0x9DF2,0x9B22,
    0x9872,0x95E2,0x9373,0x9126,0x8EFC,0x8CF4,0x8B11,0x8952,
    0x87B8,0x8644,0x84F5,0x83CD,0x82CC,0x81F2,0x813F,0x80B4,
    0x8050,0x8014,0x8000,0x8014,0x8050,0x80B4,0x813F,0x81F2,
    0x82CC,0x83CD,0x84F5,0x8644,0x87B8,0x8952,0x8B11,0x8CF4,
    0x8EFC,0x9126,0x9373,0x95E2,0x9872,0x9B22,0x9DF2,0xA0E1,
    0xA3ED,0xA715,0xAA5A,0xADB9,0xB132,0xB4C3,0xB86C,0xBC2C,
    0xC000,0xC3E8,0xC7E3,0xCBF0,0xD00D,0xD439,0xD872,0xDCB8,
    0xE109,0xE563,0xE9C6,0xEE30,0xF29F,0xF712,0xFB88,0x0000,
    0x0478,0x08EE,0x0D61,0x11D0,0x163A,0x1A9D,0x1EF7,0x2348,
    0x278E,0x2BC7,0x2FF3,0x3410,0x381D,0x3C18,0x4000,0x43D4,
    0x4794,0x4B3D,0x4ECE,0x5247,0x55A6,0x58EB,0x5C13,0x5F1F,
    0x620E,0x64DE,0x678E,0x6A1E,0x6C8D,0x6EDA,0x7104,0x730C,
    0x74EF,0x76AE,0x7848,0x79BC,0x7B0B,0x7C33,0x7D34,0x7E0E,
    0x7EC1,0x7F4C,0x7FB0,0x7FEC
};

//returns [0-180]
unsigned char atan2Lerp(int x, int y)
{
    if(y==0)    return (x>=0 ? 0 : 90);

    int phi;
    unsigned short t, fa, fb, h;

    OCTANTIFY(x, y, phi);
    phi *= BRAD_PI/4;

    t= QDIV(y, x, ATAN_FP);
    h= t % ATANLUT_STRIDE;
    fa= atanLUT[t/ATANLUT_STRIDE  ];
    fb= atanLUT[t/ATANLUT_STRIDE+1];

    return (unsigned char)((phi + ( fa + ((fb-fa)*h >> ATANLUT_STRIDE_SHIFT) )/8)/182);
}

short sinL(int x) 
{
    return (sinLUT[(unsigned short)x]);
}

short cosL(int x) 
{
    return(cosLUT[(unsigned short)x]);
}

void mvMeanShift_internal(const IplImage* src, IplImage* dst, int kernel_size, int h_dist, int s_dist, int v_dist) {
    assert (kernel_size % 2 == 1);
    const int s_min = 60;
    const int v_min = 30;

    // generate kernel point array
    int kernel_area = kernel_size*kernel_size;
    int kernel_rad = (kernel_size-1)/2;
    int widthStep = src->widthStep;
    int* kernel_point_array = new int[kernel_area];
    unsigned array_index = 0;
    for (int j = -kernel_rad; j <= kernel_rad; j++)
        for (int i = -kernel_rad; i <= kernel_rad; i++)
            kernel_point_array[array_index++] = i*widthStep + j;

    const IplImage * HSVImg = src;

    unsigned char* src_addr_first = (unsigned char*)src->imageData;
    unsigned char* src_addr_last = src_addr_first + widthStep*src->height;

    unsigned char* imgPtr, *resPtr;

    unsigned long xSum, ySum;

    for (int r = 0; r < HSVImg->height; r++) {                         
        imgPtr = (unsigned char*) (HSVImg->imageData + r*HSVImg->widthStep); // imgPtr = first pixel of rth's row
        resPtr = (unsigned char*) (dst->imageData + r*dst->widthStep);
        
        for (int c = 0; c < dst->width; c++) {
            unsigned char H = *imgPtr;
            unsigned char S = *(imgPtr+1);
            unsigned char V = *(imgPtr+2);
            
            *resPtr = *(resPtr+1) = *(resPtr+2) = 0;

            // check if means sv min reqs
            if (S >= s_min && V >= v_min) {
                unsigned char* tempPtr;
                unsigned S2 = S, V2 = V;
                //unsigned H2 = H;
                xSum = cosL(H);
                ySum = sinL(H);
                int good_pixels = 1, total_pixels = 1;

                // go thru each pixel in the kernel
                for (int i = 0; i < kernel_area; i++) {
                    tempPtr = imgPtr + 3*kernel_point_array[i];
                    if (!IN_SRC(tempPtr))
                        continue;

                    if (std::min(abs(H-tempPtr[0]),180-abs(H-tempPtr[0])) <= h_dist && 
                        abs(S-tempPtr[1]) <= s_dist && 
                        abs(V-tempPtr[2]) <= v_dist) 
                    {
                        xSum += cosL((unsigned)tempPtr[0]);
                        ySum += sinL((unsigned)tempPtr[0]);
                        //H2 += (unsigned)tempPts[0];
                        S2 += (unsigned)tempPtr[1];
                        V2 += (unsigned)tempPtr[2];
                        good_pixels++;
                    }

                    total_pixels++;
                }

                if (6*good_pixels >= total_pixels) {
                    resPtr[0] = (unsigned char)(atan2Lerp((int)(xSum / good_pixels), (int)(ySum/good_pixels)));
                    //resPtr[0] = (unsigned char)(H2 / good_pixels);
                    resPtr[1] = (unsigned char)(S2 / good_pixels);
                    resPtr[2] = (unsigned char)(V2 / good_pixels);
                }
            }
            
            imgPtr += 3;
            resPtr += 3;
        }
    }

    delete[] kernel_point_array;
}

void mvMeanShift(const IplImage* src, IplImage* dst, int kernel_size, int h_dist, int s_dist, int v_dist) {
    int N = 2;
    IplImage* src_resized = cvCreateImage(cvSize(src->width/N,src->height/N), IPL_DEPTH_8U, 3);
    IplImage* dst_resized = cvCreateImage(cvSize(src->width/N,src->height/N), IPL_DEPTH_8U, 3);

    cvResize (src, src_resized, CV_INTER_NN);
    cvCvtColor (src_resized, src_resized, CV_BGR2HSV);

    mvMeanShift_internal (src_resized, dst_resized, kernel_size, h_dist, s_dist, v_dist);

    cvCvtColor (dst_resized, dst_resized, CV_HSV2BGR);
    cvResize (dst_resized, dst, CV_INTER_LINEAR);
 
    cvReleaseImage(&src_resized);
    cvReleaseImage(&dst_resized);   
}


struct mvTarget {
    unsigned char h, s, v;
};

char mDistance(int a, int b, int c, int x, int y, int z){
    return((4*std::min(abs(x-a),180-abs(x-a)) + abs(y-b) + abs(z-c))/5);
}

void AdaptiveFilter2(const IplImage* src, IplImage* dst){
    mvTarget targets[] = {{50,130,60},{100,80,40}};

    unsigned char minDist, tempDist;
    unsigned char* imgPtr, *resPtr;

    IplImage * HSVImg = mvGetScratchImage_Color();
    cvCvtColor (src, HSVImg, CV_BGR2HSV);


    for (int r = 0; r < HSVImg->height; r++) {                         
        imgPtr = (unsigned char*) (HSVImg->imageData + r*HSVImg->widthStep); // imgPtr = first pixel of rth's row
        resPtr = (unsigned char*) (dst->imageData + r*dst->widthStep);
        
        for (int c = 0; c < dst->width; c++) {
            minDist = 255;
            for(int i =0; i<2; i++){
                tempDist = mDistance(*imgPtr, *(imgPtr+1), *(imgPtr+2), targets[i].h, targets[i].s, targets[i].v);
                if(tempDist < minDist) minDist = tempDist;
            }
            *resPtr = minDist;
            imgPtr+=3; resPtr++;
        }
    }

    mvReleaseScratchImage_Color();
}

