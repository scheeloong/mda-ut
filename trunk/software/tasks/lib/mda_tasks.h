#ifndef __MDA_TASK__MDA_TASK__
#define __MDA_TASK__MDA_TASK__

#include "ImageInput.h"
#include "ActuatorOutput.h"

enum MDA_TASK_RETURN_CODE {
	FATAL_ERROR, 
	TASK_DONE,
	TASK_REDO,
	TASK_MISSING
};

class MDA_TASK_BASE {
protected:
	ActuatorOutput* _Actuator;
	ImageInput* _ImageInput;
	const IplImage* frame;

	MDA_VISION_MODULE_BASE* vision_module;

	bool grabFrame (ImageDirection dir = FWD_IMG) {
		frame = _ImageInput->get_image(dir);
		return (frame != NULL);
	}
	void writeFrame () {
		exit(1); // need to figure out how ImageInput supports image writes
	}
	void move (ATTITUDE_CHANGE_DIRECTION direction, int delta_accel) {
		_Actuator->set_attitude_change(direction, delta_accel);	
	}
	

public:
	MDA_TASK_BASE (ImageInput* imageInput, ActuatorOutput * actuator) :
		_Actuator (actuator),
		_ImageInput (imageInput),
		frame (NULL)
	{
	}
	virtual ~MDA_TASK_BASE() {}

	// the following function loops until it completes the task and returns
	virtual MDA_TASK_RETURN_CODE run_task () = 0; 
};

/// ########################################################################
/// this class is to test stuff. Write anything
/// ########################################################################
class MDA_TASK_TEST : public MDA_TASK_BASE {
public:
	MDA_TASK_TEST (const char* settings_file, ImageInput* imageInput, ActuatorOutput* actuator);
	~MDA_TASK_TEST ();

	MDA_TASK_RETURN_CODE run_task ();
};

/// ########################################################################
/// Gate
/// ########################################################################

/// ########################################################################
/// Path
/// ########################################################################

/// ########################################################################
/// Buoy
/// ########################################################################

/// ########################################################################
/// U shaped Frame
/// ########################################################################

#endif
