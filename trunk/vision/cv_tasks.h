#ifndef _CV_TASKS
#define _CV_TASKS

#include <cv.h>
#include <highgui.h>

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"

#define TAN_FRONT_FOV 1.20 // Tan of the FOV of front cam. Equal to full_width/range, 37 is FOV of sim camera

// flags
#define _DISPLAY 1
#define _INVERT 2
#define _QUIET 4

// vision_gate
#define GATE_SKINNYNESS 0.08 // ratio of pipe diameter to length
#define GATE_WIDTH 300.0 // physical width in centimeters
#define GATE_HEIGHT 160.0 // physical width in centimeters
char vision_GATE (IplImage* img, int &gateX, int &gateY, float &range, 
                  HSV_settings HSV, char* window[], char flags=0);
char vision_SQUARE (IplImage* img, int &X, int &Y, float &range, 
                    HSV_settings HSV, char* window[], char flags=0);
void controller_GATE (IplImage* img, Mission &m);

#define PATH_SKINNYNESS 0.12 // ratio of pipe diameter to length
char vision_PATH (IplImage* img, int &pathX, int &pathY, float &tan_angle, float &length, 
                  HSV_settings HSV, char*window[], char flags=0);
char controller_PATH (IplImage* img, char &state, char* window[]);

#endif
