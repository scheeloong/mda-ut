/** mgui - MDA vision gui and utils
 *  includes window management and txt file reading
 *  Author - Ritchie Zhao
 */

#ifndef __MDA_VISION_MGUI__
#define __MDA_VISION_MGUI__

#include <highgui.h>
#include <stdio.h>

// Hopefully this is the only hardcoded settings file
#define COMMON_SETTINGS_FILE "settings/common_settings.csv"

/** read_mv_setting and co. - Functions to read settings from .csv files **/
// This function searches through the file and looks for the setting, getting
//   its value if found. Template allow for int,unsigned,float versions 
#define LINE_LEN 80
#define COMMENT_CHAR '#'
template <typename TYPE>
void read_mv_setting (const char filename[], const char setting_name[], TYPE &value);
template <typename TYPE>
void read_common_mv_setting (const char setting_name[], TYPE &value) 
{
    return read_mv_setting (COMMON_SETTINGS_FILE, setting_name, value);
}

/** mvWindow - class and infrastructure for managing opencv windows **/
// This class represents a single window and lets you display images
// The first 4 windows created are moved automatically to good locations
// on the screen
#define NUM_SUPPORTED_WINDOWS 4 
#define WINDOW_NAME_LEN 20
class mvWindow {
    char name[WINDOW_NAME_LEN];
    int window_number;
    
    public:
    mvWindow (const char _name[]);
    ~mvWindow (); 
    void showImage (const CvArr* _image) { cvShowImage(name, _image); }
    void move (unsigned x, unsigned y) { cvMoveWindow(name, x, y); }
};

/** mvCamera - class for managing webcams and video writers **/
// This class lets you open a camera and write video to disk 
class mvCamera {
    CvCapture* capture;
    CvVideoWriter* writer;
    int _WRITE_;    

    public:
    mvCamera (const char* settings_file);
    ~mvCamera ();
    IplImage* getFrame () { return cvQueryFrame(capture); }
    void writeFrame (IplImage* frame) { cvWriteFrame(writer,frame); }
};

#endif
