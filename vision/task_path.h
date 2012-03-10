#ifndef _TASK_PATH
#define _TASK_PATH

#include <cv.h>
#include <highgui.h>

#include "common.h"
#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"
#include "../mission/mission.h"

#define PATH_SKINNYNESS 0.125 // ratio of pipe diameter to length
#define PIPE_REAL_LEN 120

retcode vision_PATH (vision_in &Input, vision_out &Output, char flags=0);

void controller_PATH (vision_in &Input, Mission &m);

#endif
