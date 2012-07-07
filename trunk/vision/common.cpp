#include "common.h"

HSV_settings:: HSV_settings () {
    H_MIN = S_MIN  = V_MIN = 0;
    H_MAX = 179;    S_MAX = V_MAX = 255;
}
HSV_settings:: HSV_settings (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax) {
    H_MIN = (hmin>=0) ? hmin : hmin+180; 
    H_MAX = (hmax<180) ? hmax : hmax-180; 
    S_MIN = smin; S_MAX = smax; V_MIN = vmin; V_MAX = vmax;
}
void HSV_settings:: setAll (int hmin, int hmax, unsigned smin, unsigned smax, unsigned vmin, unsigned vmax) {
    H_MIN = (hmin>=0) ? hmin : hmin+180; 
    H_MAX = (hmax<180) ? hmax : hmax-180;
    S_MIN = smin; S_MAX = smax; V_MIN = vmin; V_MAX = vmax;
}
void HSV_settings:: setHue (int hmin, int hmax) { 
    H_MIN = (hmin>=0) ? hmin : hmin+180; 
    H_MAX = (hmax<180) ? hmax : hmax-180; 
}
void HSV_settings:: setSat (unsigned smin, unsigned smax) { S_MIN = smin; S_MAX = smax; }
void HSV_settings:: setVal (unsigned vmin, unsigned vmax) { V_MIN = vmin; V_MAX = vmax; }
//void HSV_settings:: setRange1 () { H_MIN=40; H_MAX=70; S_MIN=125; S_MAX=255; V_MIN=110; V_MAX=255;}
void HSV_settings:: setSim_gate () { H_MIN=5; H_MAX=30; S_MIN=70; S_MAX=255; V_MIN=150; V_MAX=255;}
void HSV_settings:: setSim_path () { H_MIN=20; H_MAX=60; S_MIN=80; S_MAX=255; V_MIN=110; V_MAX=255;}
void HSV_settings:: setSim_buoyR () { H_MIN=170; H_MAX=10; S_MIN=70; S_MAX=255; V_MIN=110; V_MAX=255;}
void HSV_settings:: setSim_buoyG () { H_MIN=50; H_MAX=70; S_MIN=70; S_MAX=255; V_MIN=110; V_MAX=255;}
void HSV_settings:: setSim_buoyY () { H_MIN=15; H_MAX=45; S_MIN=70; S_MAX=255; V_MIN=110; V_MAX=255;}
void HSV_settings:: setSim_U () { H_MIN=50; H_MAX=70; S_MIN=75; S_MAX=255; V_MIN=110; V_MAX=255;}
void HSV_settings:: setSim_torp () { H_MIN=175; H_MAX=10; S_MIN=120; S_MAX=255; V_MIN=160; V_MAX=255;}



int HSV_settings:: HueInRange (unsigned hue) { 
    if (H_MAX >= H_MIN) 
        return ((hue >= unsigned(H_MIN)) && (hue <= unsigned(H_MAX)));
    else
        return (((hue <= unsigned(H_MIN))&&(hue <= unsigned(H_MAX))) 
             || ((hue >= unsigned(H_MIN))&&(hue >= unsigned(H_MAX)))); 
}

vision_in:: vision_in () {
    window = new char*[3]; //(char**) malloc(3*sizeof(char*));
    window[0]=new char[10]; window[1]=new char[10]; window[2]=new char[10];
    strcpy(window[0], WIN0); strcpy(window[1], WIN1); strcpy(window[2], WIN2);
}
vision_in:: ~vision_in () {
    for (int i = 0; i < 3; i++) 
        delete[] (window[i]);
    delete[] (window);
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
// positive y direction is UP, positive X direction is LEFT because OPENCV is WIERD
// finds tangent of angle between your vector and the Y AXIS
    int dx = -clusters[i][0].x + clusters[i][1].x;
    int dy = clusters[i][0].y - clusters[i][1].y;
    if (dy == 0) { // horizontal line
        if (dx > 0) 
            return 10000; // this is like 0.001 degrees
        else 
            return -10000;
    }
    return float(dx)/dy;
}
