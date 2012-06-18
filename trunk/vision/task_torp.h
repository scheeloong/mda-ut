#ifndef _TASK_TORP
#define _TASK_TORP

#include <cv.h>
#include <highgui.h>

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"
#include "../motors/motors.h"
#define BUOY_REAL_RAD 12.0 // in centimeters

retcode vision_TORP (vision_in &Input, vision_out &Output, char flags=0);
void controller_TORP (vision_in &Input, Motors &m);

#endif