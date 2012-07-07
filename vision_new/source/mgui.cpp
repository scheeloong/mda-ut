#include "mgui.h"

static bool windows_array[NUM_SUPPORTED_WINDOWS] = {false,false,false,false};

mvWindow:: mvWindow (const char _name[]) { // this has to be the h file
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

mvWindow:: ~mvWindow () { 
    cvDestroyWindow (name); 
    windows_array[window_number] = false;
}
    
