#include <limits.h>
#include "../../vision/lib/mda_vision.h"

#ifndef __MDA_CONTROL__VCI__
#define __MDA_CONTROL__VCI__

class VCI {
	friend class MDA_VISION_MODULE_TEST;
	friend class MDA_VISION_MODULE_GATE;
	friend class MDA_VISION_MODULE_PATH;
	friend class MDA_VISION_MODULE_BUOY;
	friend class MDA_VISION_MODULE_FRAME;

protected:
	int pixel_x, pixel_y; 		
	float angle_x, angle_y;
	int range;						// this is real distance in CENTIMETERS
	float angle;

public:
	static const int VCI_UNDEFINED_VALUE = -88888888;

	VCI () {
		pixel_x = pixel_y = angle_x = angle_y = range = angle = VCI_UNDEFINED_VALUE;
	} 

	void clear () {
		pixel_x = pixel_y = angle_x = angle_y = range = angle = VCI_UNDEFINED_VALUE;
	}

	int get_pixel_x() { 
		assert (pixel_x != VCI_UNDEFINED_VALUE);
		return pixel_x;
	}
	int get_pixel_y() {
		assert (pixel_y != VCI_UNDEFINED_VALUE);
		return pixel_y;
	}
	int get_angle_x() { 
		assert (angle_x != VCI_UNDEFINED_VALUE);
		return angle_x;
	}
	int get_angle_y() {
		assert (angle_y != VCI_UNDEFINED_VALUE);
		return angle_y;
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
