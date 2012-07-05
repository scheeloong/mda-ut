/** mv - MDA vision library
 *  includes basic img object
 */


#ifndef __MDA_VISION_MV__
#define __MDA_VISION_MV__

#include <cv.h>

class mvImage () {
protected:
    IplImage * Image;
public:
    // these variables allow easy access to the image parameters, and are always in sync
    MvSize Size;
    MvDepth Depth; 
    MvNChannels NChannels;
    MvOrigin Origin;
    MvColorMode ColorMode;
    
public:
    /** types */
    enum MvDepth {MV_DEPTH_8U};
    enum MvNChannels {MV_1_CHANNEL, MV_3_CHANNEL};
    enum MvOrigin {TOP_LEFT, BOTTOM_LEFT}; 
    enum MvColorMode {MV_COLOR_RGB, MV_COLOR_HSV};
    typedef MvSize CvSize;
    
    /** constructors and destructor */
    mvImage ();
    mvImage (mvImage* _img); 
    mvImage (IplImage* _img); // assumed to be RGB
    mvImage (MvSize _size, MvDepth=MV_DEPTH_8U, MvNChannels=MV_1_CHANNEL);
    ~mvImage ();
    
    /** accessor functions */
    void* imageData () { return Image->imageData; }
    /*MvSize size() { return cvSize(Image->width, Image->height); }
    unsigned width(){ return (unsigned)(Image->width); }
    unsigned height() { return (unsigned)(Image->height); }
    MvDepth depth() { return Depth; }
    MvNChannels nchannels() { return NChannels; }
    MvOrigin origin() { return Origin; }
    MvColorMode colorMode { return ColorMode; }
    */
}

#endif