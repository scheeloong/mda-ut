/** mgui - MDA vision gui and utils
 *  includes window management and txt file reading
 *  Author - Ritchie Zhao
 */

#ifndef __MDA_VISION_MGUI__
#define __MDA_VISION_MGUI__

#include <highgui.h>
#include <stdio.h>

/* Function to read a single setting from a file. It searches through the file
   and looks for the setting, getting its value if found. It returns 0 only
   if it successesfully does this. */
#define LINE_LEN 80
#define COMMENT_CHAR '#'
int read_mv_setting (const char filename[], const char setting_name[], float &value);

/* Class and infrastructure for managing opencv windows */
// this array is used to determine what slot the window can go to, and where
// it will be moved to on the screen
#define NUM_SUPPORTED_WINDOWS 4 
static bool windows_array[NUM_SUPPORTED_WINDOWS] = {false,false,false,false};

class mvWindow {
    #define WINDOW_NAME_LEN 20
    char name[WINDOW_NAME_LEN];
    int window_number;
    
    public:
    mvWindow (const char* _name);
    ~mvWindow (); 
    void showImage (const CvArr* _image) { cvShowImage (name, _image); }
};

inline mvWindow:: mvWindow (const char* _name) { // this has to be the h file
    assert (strlen(_name) < WINDOW_NAME_LEN);
    sprintf (name, "%s", _name);

    cvNamedWindow (name, CV_WINDOW_AUTOSIZE);

    int i = 0;
    while (windows_array[i] == true && i < NUM_SUPPORTED_WINDOWS) 
        i++;                    // find next free slot

    if (i >= NUM_SUPPORTED_WINDOWS) return; // return if no slot
    windows_array[i] = true;    // mark slot as used
    window_number = i;
    
    switch (window_number) {
        case 0: cvMoveWindow (name, 500,10); break;
        case 1: cvMoveWindow (name, 850,10); break;
        case 2: cvMoveWindow (name, 500,310); break;
        case 3: cvMoveWindow (name, 850,310); break;
    }
}

inline mvWindow:: ~mvWindow () { 
    cvDestroyWindow (name); 
    windows_array[window_number] = false;
}
//num_windows = 0;

#endif