#include "mv.h"
#include <stdlib.h>

#define MV_DEBUG
#ifdef MV_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

/// TERRIBLE TERRIBLE CODING
#define IN_SRC(x) (((x)>=src_addr_first) && ((x)<src_addr_last))
#define IN_DST(x) (((x)> dst_addr_first) && ((x)<dst_addr_last))

void mvBinaryDilate (const IplImage* src, IplImage* dst, int kernel_point_array[], unsigned kernel_area);
void mvBinaryErode (const IplImage* src, IplImage* dst, int kernel_point_array[], unsigned kernel_area);
void mvBinaryGradient (const IplImage* src, IplImage* dst, int kernel_point_array[], unsigned kernel_area);


mvBinaryMorphology:: mvBinaryMorphology (int Kernel_Width, int Kernel_Height, MV_KERNEL_SHAPE Shape) :
    bin_morph ("mvMorphology - morph"),
    bin_gradient ("mvMorphology - gradient")
{
    assert (Kernel_Width % 2 == 1); // odd number
    assert (Kernel_Height % 2 == 1);

    temp = mvCreateImage ();
    kernel_width = Kernel_Width;
    kernel_height = Kernel_Height;   
    kernel_area = Kernel_Width * Kernel_Height;
    int widthStep = temp->widthStep;

    if (Shape == MV_KERN_RECT) {
        /// calculate an array of linear shifts for each point in the kernel
        kernel_point_array = new int[kernel_area];
        unsigned array_index = 0;
        for (int j = -(kernel_height-1)/2; j <= (kernel_height-1)/2; j++)
            for (int i = -(kernel_width-1)/2; i <= (kernel_width-1)/2; i++)
                kernel_point_array[array_index++] = i*widthStep + j;    
    }
    else {
        fprintf (stderr, "%s: %d: Unrecognized kernel shape.\n", __FILE__, __LINE__);
        exit (1);
    }
}

mvBinaryMorphology:: ~mvBinaryMorphology () {
	cvReleaseImage (&temp);
}

void mvBinaryMorphology:: mvBinaryMorphologyMain (
        MV_MORPHOLOGY_TYPE morphology_type,
        const IplImage* src, IplImage* dst
)
{
    assert (src->width == temp->width);
    assert (src->height == temp->height);
    assert (src->width == dst->width);
    assert (src->height == dst->height);
    assert (src->nChannels == 1);
    assert (dst->nChannels == 1);
    
    cvZero (temp);

    if (morphology_type == MV_DILATE) {
        mvBinaryDilate (src, temp, kernel_point_array, kernel_area);
    }
    else if (morphology_type == MV_ERODE) {
        mvBinaryErode (src, temp, kernel_point_array, kernel_area);
    }
    else if (morphology_type == MV_CLOSE) {
        mvBinaryDilate (src, temp, kernel_point_array, kernel_area);
        cvZero (dst);
        mvBinaryErode (temp, dst, kernel_point_array, kernel_area);
    }
    else if (morphology_type == MV_OPEN) {
        mvBinaryErode (src, temp, kernel_point_array, kernel_area);
        cvZero (dst);
        mvBinaryDilate (temp, dst, kernel_point_array, kernel_area);
    }
    else if (morphology_type == MV_GRADIENT) {
        mvBinaryGradient (src, temp, kernel_point_array, kernel_area);
    }
    
    cvCopy (temp, dst);
}


//#########################################################################
//#### Helper functions for mvBinaryMorphology
//#########################################################################
void mvBinaryDilate (
    const IplImage* src, IplImage* dst,
    int* kernel_point_array,
    unsigned kernel_area
)
{
    assert (src != dst);
    unsigned width = src->width;
    unsigned height = src->height;
    unsigned widthStep = src->widthStep;    // width in bytes
    unsigned char* dst_addr_first = (unsigned char*)dst->imageData;
    unsigned char* dst_addr_last = dst_addr_first + widthStep*height;

    unsigned char *srcPtr;
    unsigned char *dstPtr;
    bool Prev_Pixel_High = false;

    /// go over every pixel in src. If that pixel is High, we set a
    /// kernel around it in dst to be High as well.
    for (unsigned r = 0; r < height; r++) {
        srcPtr = (unsigned char*) (src->imageData + r*widthStep);
        dstPtr = (unsigned char*) (dst->imageData + r*widthStep);
 
        for (unsigned c = 0; c < width; c++) {
            if (*srcPtr != 0) { // only work on non-zero (high) pixels

                for (unsigned i = Prev_Pixel_High?kernel_area/2:0; i < kernel_area; i++) {
                    unsigned char *ptr = dstPtr + kernel_point_array[i];
                    if (IN_DST(ptr)) {
                        *ptr = 255;
                        Prev_Pixel_High = true;
                    }
                    else
                        Prev_Pixel_High = false;
                }

            }
            else
                Prev_Pixel_High = false;

            srcPtr++;
            dstPtr++;
        }
    }
}

void mvBinaryErode (
    const IplImage* src, IplImage* dst,
    int* kernel_point_array,
    unsigned kernel_area
)
{
    assert (src != dst);
    unsigned width = src->width;
    unsigned height = src->height;
    unsigned widthStep = src->widthStep;    // width in bytes
    unsigned char* src_addr_first = (unsigned char*)src->imageData;
    unsigned char* src_addr_last = src_addr_first + widthStep*height;

    unsigned char *srcPtr;
    unsigned char *dstPtr;

    /// go over every pixel in src. If that pixel is High, we check a
    /// kernel around it for 0s. If a 0 is found set dst pixel to 0
    for (unsigned r = 0; r < height; r++) {
        srcPtr = (unsigned char*) (src->imageData + r*widthStep);
        dstPtr = (unsigned char*) (dst->imageData + r*widthStep);
 
        for (unsigned c = 0; c < width; c++) {
            if (*srcPtr != 0) { // only work on non-zero (high) pixels

                bool BREAK = false;
                for (unsigned i = 0; i < kernel_area; i++) {
                    unsigned char *ptr = srcPtr + kernel_point_array[i];
                    if (IN_SRC(ptr) && *ptr == 0) {
                        *dstPtr = 0;
                        BREAK = true;
                        break;
                    }
                }
                if (!BREAK)
                    *dstPtr = 255;

            }
            srcPtr++;
            dstPtr++;
        }
    }
}

void mvBinaryGradient (
    const IplImage* src, IplImage* dst,
    int* kernel_point_array,
    unsigned kernel_area
)
{
    // first do dilate, then use the result + the src to do the gradient
    mvBinaryDilate (src, dst, kernel_point_array, kernel_area);

    assert (src != dst);
    unsigned width = src->width;
    unsigned height = src->height;
    unsigned widthStep = src->widthStep;    // width in bytes
    unsigned char* src_addr_first = (unsigned char*)src->imageData;
    unsigned char* src_addr_last = src_addr_first + widthStep*height;

    unsigned char *srcPtr;
    unsigned char *dstPtr;

    // go over each high pixel in dst (dilated img). If the kernel around the 
    // equivalent src pixel is zero or will be eroded to zero, leave it alone.
    // Else set it to 0
    for (unsigned r = 0; r < height; r++) {
        srcPtr = (unsigned char*) (src->imageData + r*widthStep);
        dstPtr = (unsigned char*) (dst->imageData + r*widthStep);
 
        for (unsigned c = 0; c < width; c++) {
            if (*srcPtr != 0) { // we must check if *srcPtr will be eroded to 0
                
                bool BREAK = false;
                for (unsigned i = 0; i < kernel_area; i++) {
                    unsigned char *ptr = srcPtr + kernel_point_array[i];
                    if (IN_SRC(ptr) && *ptr == 0) { // *srcPtr eroded to 0
                        BREAK = true;
                        break;
                    }
                }
                if (!BREAK)
                    *dstPtr = 0;

            }   
            srcPtr++;
            dstPtr++;
        }
    }
}

//#########################################################################
//#### Functions for BGR2HSV. Not used.
//#########################################################################
typedef unsigned char uchar;
// Takes in 3 numbers B,R,G, and modifies them to instead be in H,S,V format
void tripletBRG2HSV (uchar Blue, uchar Green, uchar Red, uchar &Hue, uchar &Sat, uchar &Val) {
    /// find the max and min color component
    uchar M, m, Chroma;
    if (Blue > Red) {
        M = std::max(Blue, Green);
        m = std::min(Red, Green);
    }
    else {
        M = std::max(Red, Green);
        m = std::min(Blue, Green);
    }
    Chroma = M - m;

    /// calculate HSV components
    if (Chroma == 0) {
        Hue = Sat = 0;
        Val = M;
        return;
    }
    else if (M == Red)
        Hue = 30 *(((Green - Blue) / Chroma) % 6);
    else if (M == Green)
        Hue = 30 * ((Blue - Red) / Chroma + 2);
    else
        Hue = 30 * ((Red - Green) / Chroma + 4);

    Val = M;
    Sat = 255 *Chroma / Val;
}

void mvBRG2HSV(const IplImage* src, IplImage* dst) {
    
    assert (src != NULL);
    assert (dst != NULL);
    assert (src->nChannels == 3);
    assert (dst->nChannels == 3);
    
    int i, j;
    unsigned char* srcPtr, * dstPtr;

    for ( i = 0; i < src->height; i++) {
      	srcPtr = (unsigned char *)((src->imageData) + i*(src->widthStep));
	dstPtr = (unsigned char *)((dst->imageData) + i*(dst->widthStep));
        for ( j = 0; j < src->width; j++) {
	    
	    tripletBRG2HSV(*srcPtr, *(srcPtr+1), *(srcPtr+2), *dstPtr, *(dstPtr+1), *(dstPtr+2));
	    srcPtr +=3;
	    dstPtr +=3;
	  
	}
    }
    
  return;
  
}

