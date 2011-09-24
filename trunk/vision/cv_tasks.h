#ifndef _CV_TASKS
#define _CV_TASKS

#include "cv.h"
#include "highgui.h"

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"

#define FRONT_FOV 37.0 // FOV of front camera, 37 is FOV of sim camera

// vision_gate
#define GATE_SKINNYNESS 0.08 // ratio of pipe diameter to length
#define GATE_WIDTH 3.0 // physical width in meters
#define GATE_HEIGHT 1.6 // physical width in meters
#define GATE_HMIN 10 
#define GATE_HMAX 30 
#define GATE_SMIN 60
#define GATE_SMAX 255
char controller_GATE (IplImage* img, char &state, char* window[]);

#define PATH_SKINNYNESS 0.12 // ratio of pipe diameter to length
#define PATH_HMIN 10 
#define PATH_HMAX 50 
#define PATH_SMIN 100
#define PATH_SMAX 255
char controller_PATH (IplImage* img, char &state, char* window[]);

#endif