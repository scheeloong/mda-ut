/** mgui - MDA vision gui and utils
 *  includes window management and txt file reading
 *  Author - Ritchie Zhao
 */

#ifndef __MDA_VISION_MGUI__
#define __MDA_VISION_MGUI__

#include <highgui.h>

/** settings file reading functions */

/* Function to read a single setting from a file. It searches through the file
   and looks for the setting, getting its value if found. It returns 0 only
   if it successesfully does this. */
#define LINE_LEN 80
#define COMMENT_CHAR '#'
int read_mv_setting (const char filename[], const char setting_name[], float &value);

#endif