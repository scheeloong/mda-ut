#ifndef _TASK_BUOY
#define _TASK_BUOY

#include <cv.h>
#include <highgui.h>

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"
#include "../mission/mission.h"

#define BUOY_REAL_RAD 8.0 //12.0 // in centimeters

retcode vision_BUOY (vision_in &Input, vision_out &Output, char flags=0);
void controller_BUOY (vision_in &Input, Mission &m);

#endif
