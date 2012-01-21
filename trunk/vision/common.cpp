#include "common.h"

HSV_settings:: HSV_settings () {
    H_MIN = S_MIN  = V_MIN = 0;
    H_MAX = 180;    S_MAX = V_MAX = 255;
}
HSV_settings:: HSV_settings (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax) {
    H_MIN = hmin; H_MAX = hmax; S_MIN = smin; S_MAX = smax; V_MIN = vmin; V_MAX = vmax;
}
void HSV_settings:: setAll (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax) {
    H_MIN = hmin; H_MAX = hmax; S_MIN = smin; S_MAX = smax; V_MIN = vmin; V_MAX = vmax;
}
void HSV_settings:: setHue (int hmin, int hmax) { H_MIN = hmin; H_MAX = hmax; }
void HSV_settings:: setSat (unsigned smin, unsigned smax) { S_MIN = smin; S_MAX = smax; }
void HSV_settings:: setVal (unsigned vmin, unsigned vmax) { V_MIN = vmin; V_MAX = vmax; }
void HSV_settings:: setRange1 () { H_MIN=40; H_MAX=70; S_MIN=125; S_MAX=255; V_MIN=110; V_MAX=255;}



vision_in:: vision_in () {
    window = (char**) malloc(3*sizeof(char*));
    window[0]=(char*)malloc(10); window[1]=(char*)malloc(10); window[2]=(char*)malloc(10);
    strcpy(window[0], WIN0); strcpy(window[1], WIN1); strcpy(window[2], WIN2);
}
vision_in:: ~vision_in () {
    for (int i = 0; i < 3; i++) 
        free (window[i]);
    free (window);
}

void create_windows () {
    cvNamedWindow(WIN0,1);   // create 3 windows for cv to use
    cvMoveWindow(WIN0, 300, 0);
    cvNamedWindow(WIN1,1);
    cvMoveWindow(WIN1, 600, 0);
    cvNamedWindow(WIN2,1);
    cvMoveWindow(WIN2, 300, 200);   
}
void destroy_windows () {
    cvDestroyWindow(WIN0);   
    cvDestroyWindow(WIN1);
    cvDestroyWindow(WIN2);
}

int sqr_length (CvPoint** clusters, int i) {
// returns square length of ith line
    int dx = clusters[i][0].x-clusters[i][1].x,  dy = clusters[i][0].y-clusters[i][1].y;
    return dx*dx + dy*dy;
}

float tangent_angle (CvPoint** clusters, int i) {
    return float(clusters[i][0].y-clusters[i][1].y) / (clusters[i][0].x-clusters[i][1].x);
}