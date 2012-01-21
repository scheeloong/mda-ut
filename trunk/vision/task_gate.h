#ifndef _TASK_GATE
#define _TASK_GATE

#include <cv.h>
#include <highgui.h>

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"

#define GATE_SKINNYNESS 0.08 // ratio of pipe diameter to length
#define GATE_WIDTH 300.0 // physical width in centimeters
#define GATE_HEIGHT 160.0 // physical width in centimeters

retcode vision_GATE (vision_in &Input, vision_out &Output, char flags=0);

char controller_GATE (vision_in Input, char &state);

#endif