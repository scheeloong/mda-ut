#include "mv.h"
#include "mgui.h"
#include <cv.h>
#include <stdlib.h>

/** mvCanny */
mvCanny:: mvCanny (const char* settings_file) {
    read_mv_setting (settings_file, "CANNY_KERNEL_SIZE", KERNEL_SIZE);
    read_mv_setting (settings_file, "CANNY_LOW_THRESHOLD", LOW_THRESHOLD);
    read_mv_setting (settings_file, "CANNY_HIGH_THRESHOLD", HIGH_THRESHOLD);
}
