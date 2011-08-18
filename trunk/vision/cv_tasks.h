#ifndef _CV_TASKS
#define _CV_TASKS

#include <cv.h>
#include <highgui.h>

#include "preproc_filters.h"
#include "obj_detection.h"
#include "calc_draw.h"

// vision_gate
#define N_LINES 10
#define GATE_HMIN 10 
#define GATE_HMAX 30 
#define GATE_SMIN 60
#define GATE_SMAX 255

void vision_GATE (IplImage* img, 
                  int &gateX, int &gateY,
                  char flags=0, char* window[]=NULL);
/*
void vision_PATH (IplImage* img,
                  int &x, int &y, float &ang,
                  char flags=0, char* window[]=NULL);
*/
#endif
