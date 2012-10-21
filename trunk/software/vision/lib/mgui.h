/** mgui - MDA vision gui and utils
 *  includes window management and txt file reading
 *  Author - Ritchie Zhao
 */

#ifndef __MDA_VISION_MGUI__
#define __MDA_VISION_MGUI__

#include <highgui.h>
#include <stdio.h>
#include <cv.h>

// Hopefully this is the only hardcoded settings file
#define MDA_SETTINGS_DIR_ENV_VAR "MDA_VISION_SETTINGS_PATH"
#define MDA_BACKUP_SETTINGS_DIR "./settings/"
#define COMMON_SETTINGS_FILE "common_settings.csv"

/** read_mv_setting and co. - Functions to read settings from .csv files **/
// This function searches through the file and looks for the setting, getting
//   its value if found. Template allow for int,unsigned,float versions 
#define LINE_LEN 80
#define COMMENT_CHAR '#'
template <typename TYPE>
void read_mv_setting (const char filename[], const char setting_name[], TYPE &value);
void read_mv_setting (const char filename[], const char setting_name[], std::string &value);
template <typename TYPE>
void read_common_mv_setting (const char setting_name[], TYPE &value) 
{
    return read_mv_setting (COMMON_SETTINGS_FILE, setting_name, value);
}
inline void read_common_mv_setting (const char setting_name[], std::string &value)
{
    return read_mv_setting (COMMON_SETTINGS_FILE, setting_name, value);
}

/** mvWindow - class and infrastructure for managing opencv windows **/
// This class represents a single window and lets you display images
// The first 4 windows created are moved automatically to good locations
// on the screen
#define NUM_SUPPORTED_WINDOWS 4 
#define WINDOW_NAME_LEN 50
class mvWindow {
    char _name[WINDOW_NAME_LEN];
    int _window_number;
    
    public:
    mvWindow (const char name[]);
    ~mvWindow (); 
    void showImage (const CvArr* image) { cvShowImage(_name, image); }
    void move (unsigned x, unsigned y) { cvMoveWindow(_name, x, y); }
};

/** mvCamera - class for managing webcams and video writers **/
// This class lets you open a camera and write video to disk 
// Usage is simple. 
class mvCamera {
    #define FRAMERATE 30
    CvCapture* _capture;
    CvVideoWriter* _writer;
    IplImage* _imgResized;

    public:
    mvCamera (unsigned cam_number, unsigned write=0);
    mvCamera (const char* video_file);
    ~mvCamera ();

    /* In a monsterous betrayal of good coding, the behaviour of getFrame and 
     * getFrameResized are a bit different.
     * getFrame will return a pointer to the frame in the camera's buffer. 
     * Meaning the returned img must not be freed/modified by the user.
     * getFrameResized by necessity uses a created image, and every time it is
     * called the image will be overwritten. This img also must not be freed/
     * modified by the user.
     */ 
    IplImage* getFrame () { return cvQueryFrame(_capture); } 
    IplImage* getFrameResized () {
        IplImage* frame = cvQueryFrame (_capture);
        if (!frame)
            return NULL;
        cvResize (frame, _imgResized, CV_INTER_LINEAR); // bilienar interpolation
        return _imgResized;
    } 

    void writeFrame (IplImage* frame) {
        assert (_writer != NULL); // If fails you have not created the video writer
        cvWriteFrame(_writer,frame);
   }
};

#endif
