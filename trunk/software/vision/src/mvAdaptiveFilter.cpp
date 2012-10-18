#include "mgui.h"
#include "mv.h"

// #############################################################
// #### AdaptiveBox methods
// #############################################################
mvAdaptiveBox:: mvAdaptiveBox (uchar Min, uchar Max, uchar Min_Bound, uchar Max_Bound) {
    min = orig_min = Min;  max = orig_max = Max;
    min_bound = Min_Bound;  max_bound = Max_Bound;
    box_count = box_50_count = 0;
    box_min_bound_count = box_max_bound_count = 0;
        
    uchar half_width = (max-min)/2;
    min_50 = min + half_width;
    max_50 = max - half_width;
}

int mvAdaptiveBox:: accumulate (uchar Data) {
    return 0;
}

void mvAdaptiveBox:: adjust_box () {
}


// #############################################################
// #### AdaptiveFilter methods 
// #############################################################
mvAdaptiveFilter:: mvAdaptiveFilter (const char* Settings_File) {
}
