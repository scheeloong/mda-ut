/** mvlines - mda Line Finding algorithm library
 *  Author - Ritchie Zhao
 *  July 2012
 */ 
#ifndef __MDA_VISION_MVLINES__
#define __MDA_VISION_MVLINES__ 

#include <cv.h>

/** mvLines - object representing a collection of lines using the two endpoints */
/* When created, the class automatically allocates 6400 bytes of memory that it uses to 
 * store data. This can be recycled by calling clearData(); This memory cannot be reclaimed
 * without destroying the class instance.
 * 
 * Use nlines() to get the number of lines currently stored.
 * 
 * To access line data: lines[i] returns a 2 element array of CvPoint* => the 2 endpoints.
 * So lines[i][0] and lines[i][1] are the endpoints of ith line.
 * Use lines[i][0].x and lines[i][0].y to get x and y coordinates of an endpoint. 
 * 
 * Use drawOntoImage (img) to draw the lines onto an image.
 */
class mvLines {
    #define LINE_THICKNESS 1
    CvSeq* data;
    CvMemStorage* storage; // this stores actual line data in opencv
    
    friend class mvHoughLines;
    
    public:
    // the constructor allocates 6400 bytes of storage space, which is like 400 lines...
    mvLines () { data=NULL; storage=cvCreateMemStorage(6400); } 
    ~mvLines () {} // opencv will clean up the memory used (i hope)
    
    unsigned nlines () { return (data != NULL) ? unsigned(data->total) : 0; }
    void removeHoriz ();
    void removeVert ();
    void sortXY (); // each Horiz line has smaller X value first, each Vert smaller Y first
    void drawOntoImage (IplImage* img);
    
    CvPoint* operator [] (unsigned index) { return (CvPoint*)cvGetSeqElem(data,index); }
    
    // note clearData does NOT deallocate memory, it only allows recycling of used memory. 
    void clearData () { cvClearSeq(data); data=NULL; cvClearMemStorage(storage); } 
};

/** mvHoughLines - Hough Line finding filter */
// you must provide an mvLines object for it to work 
class mvHoughLines {
    unsigned PIX_RESOLUTION;
    float ANG_RESOLUTION; // ang_resolution is in radians
    unsigned ACCUMULATOR_THRESHOLD;
    unsigned MIN_LINE_LENGTH, MIN_COLINEAR_LINE_DIST;
    
    public:
    mvHoughLines (const char* settings_file);
    /* use default destructor */
    void findLines (IplImage *img, mvLines* lines);
};

#endif
