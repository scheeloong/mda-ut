#include <limits.h>
#include "../../vision/lib/mda_vision.h"

#ifndef __MDA_CONTROL__VCI__
#define __MDA_CONTROL__VCI__

class VCI {
	friend class MDA_VISION_MODULE_TEST;
	friend class MDA_VISION_MODULE_GATE;
	friend class MDA_VISION_MODULE_PATH;
	friend class MDA_VISION_MODULE_BUOY;

protected:
	int real_x, real_y; 		// these are real lengths in CENTIMETERS
	int range;					
	int angle;

public:
	static const int VCI_UNDEFINED_VALUE = -88888888;

	VCI () {
		real_x = real_y = range = angle = VCI_UNDEFINED_VALUE;
	} 

	void clear () {
		real_x = real_y = range = angle = VCI_UNDEFINED_VALUE;
	}

	int get_real_x() { 
		assert (real_x != VCI_UNDEFINED_VALUE);
		return real_x;
	}
	int get_real_y() {
		assert (real_y != VCI_UNDEFINED_VALUE);
		return real_y;
	}
	int get_range() {
		assert (range != VCI_UNDEFINED_VALUE);
		return range;
	}
	int get_angle() {
		assert (angle != VCI_UNDEFINED_VALUE);
		return angle;
	}

};

#endif
