/** mvlines - mda circle finding algorithm library
 *  Author - Ritchie Zhao
 *  July 2012
 */ 
#ifndef __MDA_VISION_MVCIRCLES__
#define __MDA_VISION_MVCIRCLES__ 

#include <cv.h>

#define CIRCLE_THICKNESS 2

/** Circle_Struct - simple structure to represent a circle */
// note OpenCV stores its circle finding algorithm's output in a
// float* array, where [0],[1] are x,y and [2] is radius
// so you can easily typecast the float* into Circle_Struct*
// and have easier, more readable code
typedef struct _Circle_ {
    float x;
    float y;
    float rad;
} Circle_Struct;

/** mvCircles - class representing a set of circles */
// The class automatically allocates memory needed for storage
// 
// If you declare mvCircles circles; and want to access data for circle i, use 
// circles[i].x , circles[i].y , circles[i].rad
class mvCircles {
    CvSeq* data;
    CvMemStorage* storage; // this stores actual circle data in opencv
    
    friend class mvHoughCircles;
    
    public:
    // the constructor allocates 800 bytes of storage space, which is like 70 circles...
    mvCircles () { data=NULL; storage=cvCreateMemStorage(800); } 
    ~mvCircles () {} // opencv will clean up the memory used (i hope)
    
    unsigned ncircles () { return (data != NULL) ? unsigned(data->total) : 0; }
    void drawOntoImage (IplImage* img); // defined in .cpp
    
    Circle_Struct operator [] (unsigned index) { return  (*(Circle_Struct*)cvGetSeqElem(data,index)); }
    
    // note clearData does NOT deallocate memory, it only allows recycling of used memory. 
    void clearData () { 
        cvClearSeq(data); 
        data=NULL; 
        cvClearMemStorage(storage); 
    } 
};

/** mvHoughLines - Hough Line finding filter */
// you must provide an mvCircles object for it to work
// for a filled circle, it is best to take the gradient seperately 
// before using HoughCircles
class mvHoughCircles {
    unsigned PIX_RESOLUTION;
    unsigned MIN_CENTER_DIST; // ang_resolution is in radians
    unsigned CANNY_HIGH_THRESHOLD;
    unsigned ACCUMULATOR_THRESHOLD;
    
    public:
    mvHoughCircles (const char* settings_file);
    /* use default destructor */
    void findCircles (IplImage *img, mvCircles* circles);
};

#endif