#ifndef _TASK_U
#define _TASK_U

#include <cv.h>
#include <highgui.h>

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"

#include "../motors/motors.h"

//#define U_SKINNYNESS 0.08 // ratio of pipe diameter to length
#define U_WIDTH 180 // physical width in centimeters
#define U_HEIGHT 100 // physical width in centimeters

retcode vision_U (vision_in &Input, vision_out &Output, char flags=0);

void controller_U (vision_in &Input, Motors &m);

#endif