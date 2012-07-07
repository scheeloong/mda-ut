/** mgui - MDA vision gui and utils
 *  includes window management and txt file reading
 *  Author - Ritchie Zhao
 */

#ifndef __MDA_VISION_MGUI__
#define __MDA_VISION_MGUI__

#include <highgui.h>
#include <stdio.h>

// Hopefully this is the only hardcoded settings file
#define COMMON_SETTINGS_FILE "settings/common_settings.mda"

/* Function to read a single setting from a file. It searches through the file
   and looks for the setting, getting its value if found. It returns 0 only
   if it successesfully does this. */
#define LINE_LEN 80
#define COMMENT_CHAR '#'
// basic function - templated!
template <typename TYPE>
void read_mv_setting (const char filename[], const char setting_name[], TYPE &value);

// common_settings version - this is inline!
template <typename TYPE>
void read_common_mv_setting (const char setting_name[], TYPE &value) 
{
    return read_mv_setting (COMMON_SETTINGS_FILE, setting_name, value);
}

/* Class and infrastructure for managing opencv windows */
// this array is used to determine what slot the window can go to, and where
// it will be moved to on the screen
#define NUM_SUPPORTED_WINDOWS 4 
#define WINDOW_NAME_LEN 20
class mvWindow {
    char name[WINDOW_NAME_LEN];
    int window_number;
    
    public:
    mvWindow (const char _name[]);
    ~mvWindow (); 
    void showImage (const CvArr* _image) { cvShowImage (name, _image); }
};

//num_windows = 0;

#endif