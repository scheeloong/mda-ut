#include "mv.h"

// #############################################################
// #### Helper functions
// #############################################################
inline bool within_bounds (int data, int low, int high) {
	if (high >= low)
		return (data >= low && data <= high);
	else
		return ((data <= high && data >= 0) || (data >= low && data < 180));

}


// #############################################################
// #### AdaptiveBox methods
// #############################################################
mvAdaptiveBox:: mvAdaptiveBox (int Min, int Max, int Flange, MODULO_ENUM mod) {
// for the purposes of Hue, the Max can be smaller than the Min
	assert (Min >= 0 && Min <= 255);
	assert (Max >= 0 && Max <= 255);

    min = orig_min = Min;  max = orig_max = Max;
    min_bound = min - Flange;  max_bound = max + Flange;
    if (mod == MOD_NONE) { // same as mod 256
     	min_bound = (min < Flange) ? min-Flange+256 : min-Flange;
    	max_bound = (max+Flange >= 256) ? max+Flange-256 : max+Flange;
    }
    else if (mod == MOD_180) {
    	min_bound = (min < Flange) ? min-Flange+180 : min-Flange;
    	max_bound = (max+Flange >= 180) ? max+Flange-180 : max+Flange;
    }

    box_count = box_50_count = 0;
    box_min_bound_count = box_max_bound_count = 0;
        
    int half_width = (max-min)/2;
    min_50 = min + half_width;
    max_50 = max - half_width;
}

int mvAdaptiveBox:: accumulate (int Data) {
	/*if (within_bounds (Data, min_50, max_50)) { 
		box_50_count++;
		box_count++;
		return 1;
	}
	else if (within_bounds(Data, min, max)) {
		box_count++;
		return 1;
	}
	else if (within_bounds(Data, min_bound, min)) {
		box_min_bound_count++;
		return 1;
	}
	else if (within_bounds(Data, max, max_bound)) {
		box_max_bound_count++;
		return 1;
	}
    */
    return 0;   
}

void mvAdaptiveBox:: adjust_box () {
}


// #############################################################
// #### AdaptiveFilter methods 
// #############################################################
/*mvAdaptiveFilter:: mvAdaptiveFilter (const char* Settings_File) {
	int min, max, flange;

	read_mv_setting (Settings_File, "BG_HUE_MIN", min);
    read_mv_setting (Settings_File, "BG_HUE_MAX", max);
    read_mv_setting (Settings_File, "BG_HUE_FLANGE", flange);

    read_mv_setting (Settings_File, "BG_SAT_MIN", min);
    read_mv_setting (Settings_File, "BG_SAT_MAX", max);
    read_mv_setting (Settings_File, "BG_SAT_FLANGE", flange);

    read_mv_setting (Settings_File, "BG_VAL_MIN", min);
    read_mv_setting (Settings_File, "BG_VAL_MAX", max);
    read_mv_setting (Settings_File, "BG_VAL_FLANGE", flange);

    read_mv_setting (Settings_File, "TARGET_HUE_MIN", min);
    read_mv_setting (Settings_File, "TARGET_HUE_MAX", max);
    read_mv_setting (Settings_File, "TARGET_HUE_FLANGE", flange);

    read_mv_setting (Settings_File, "TARGET_SAT_MIN", min);
    read_mv_setting (Settings_File, "TARGET_SAT_MAX", max);
    read_mv_setting (Settings_File, "TARGET_SAT_FLANGE", flange);

    read_mv_setting (Settings_File, "TARGET_VAL_MIN", min);
    read_mv_setting (Settings_File, "TARGET_VAL_MAX", max);
    read_mv_setting (Settings_File, "TARGET_VAL_FLANGE", flange);
}
*/