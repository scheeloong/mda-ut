#include "mv.h"
#include "mgui.h"
#include <cv.h>
#include <stdlib.h>

#define MV_DEBUG
#ifdef MV_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

void mvBinaryMorphology (
        MV_MORPHOLOGY_TYPE morphology_type,
        const IplImage* src, IplImage* dst, IplImage* temp, 
        int kernel_width, int kernel_height, 
        MV_KERNEL_SHAPE shape
        )
{
    assert (kernel_width % 2 == 1); // odd number
    assert (kernel_height % 2 == 1);
    assert (src->width == dst->width);
    assert (src->height == dst->height);
    assert (src->nChannels == 1);
    assert (dst->nChannels == 1);
    
    /// TERRIBLE TERRIBLE CODING
    #define IN_SRC(x) (((x)>=src_addr_first) && ((x)<src_addr_last))
    #define IN_TEMP(x) (((x)>=temp_addr_first) && ((x)<temp_addr_last))

    /// if no temp image is provided we have to create and later release our own
    bool Created_Temp_Image = false;
    if (temp == NULL) {
        Created_Temp_Image = true;
        temp = cvCreateImage (cvGetSize(src), IPL_DEPTH_8U, 1);
    }
    if (morphology_type == MV_ERODE)
        cvCopy (src, temp);
    else
        cvZero (temp);

    unsigned kernel_area = unsigned(kernel_width*kernel_height);
    unsigned width = src->width;
    unsigned height = src->height;
    unsigned widthStep = src->widthStep;    // width in bytes
    unsigned char* src_addr_first = (unsigned char*)src->imageData;
    unsigned char* src_addr_last = src_addr_first + widthStep*height;
    unsigned char* temp_addr_first = (unsigned char*)temp->imageData;
    unsigned char* temp_addr_last = temp_addr_first + widthStep*height;

    /// first calculate an array of linear shifts for each point in the kernel
    int kernel_point_array[kernel_area];
    unsigned array_index = 0;
    for (int j = -(kernel_height-1)/2; j <= (kernel_height-1)/2; j++)
        for (int i = -(kernel_width-1)/2; i <= (kernel_width-1)/2; i++)
            kernel_point_array[array_index++] = i*widthStep + j;    

    /// here we go over every pixel in src. If that pixel is High, we set a
    /// kernel around it in temp to be High as well.
    unsigned char *srcPtr;
    unsigned char *tempPtr;
    bool Prev_Pixel_High = false;
    for (unsigned r = 0; r < height; r++) {
        srcPtr = (unsigned char*) (src->imageData + r*widthStep);
        tempPtr = (unsigned char*) (temp->imageData + r*widthStep);
 
        for (unsigned c = 0; c < width; c++) {
            if (*srcPtr != 0) { // only work on non-zero (high) pixels
                // dilate = make all surrounding pixels high
                if (morphology_type == MV_DILATE) { 
                    for (unsigned i = Prev_Pixel_High?kernel_area/2:0; i < kernel_area; i++) {
                        unsigned char *ptr = tempPtr + kernel_point_array[i];
                        if (IN_TEMP(ptr)) {
                            *ptr = 255;
                            Prev_Pixel_High = true;
                        }
                        else
                            Prev_Pixel_High = false;
                    }
                }
                // erode = if there is a zero pixel surrounding, make current low
                else if (morphology_type == MV_ERODE) {
                    for (unsigned i = 0; i < kernel_area; i++) {
                        unsigned char *ptr = srcPtr + kernel_point_array[i];
                        if (IN_SRC(ptr) && *ptr == 0) {
                            *tempPtr = 0;
                            break;
                        }
                    }

                }
            }
            else {
                Prev_Pixel_High = false;
            }
            srcPtr++;
            tempPtr++;
        }
    }

    cvCopy (temp, dst);
    if (Created_Temp_Image) {
        cvReleaseImage (&temp);
    }
}
