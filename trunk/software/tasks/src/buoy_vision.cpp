#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

/// ########################################################################
/// MODULE_BUOY methods
/// ########################################################################
MDA_VISION_MODULE_BUOY:: MDA_VISION_MODULE_BUOY () {
}

MDA_VISION_MODULE_BUOY:: ~MDA_VISION_MODULE_BUOY () {
}

void MDA_VISION_MODULE_BUOY:: primary_filter (const IplImage* src) {
}

int MDA_VISION_MODULE_BUOY:: calc_vci (VCI* interface) {
    return 0;
}