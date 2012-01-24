#ifndef _TASK_PATH
#define _TASK_PATH

#include <cv.h>
#include <highgui.h>

#include "common.h"
#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"

#define PATH_SKINNYNESS 0.12 // ratio of pipe diameter to length

retcode vision_PATH (vision_in Input, vision_out &Output, char flags=0);

char controller_PATH (vision_in Input, char &state);

#endif