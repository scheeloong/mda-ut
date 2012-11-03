#ifndef __MDA_TASK__MDA_TASK__
#define __MDA_TASK__MDA_TASK__

#include "mda_vision.h"
#include "../interface/AttitudeInput.h"
#include "../interface/ImageInput.h"
#include "../interface/ActuatorOutput.h"

enum MDA_TASK {
	TASK_TEST,
	TASK_GATE,
	TASK_PATH,
	TASK_BUOY,
	TASK_FRAME
};

enum MDA_TASK_RETURN_CODE {
	FATAL_ERROR, 
	TASK_DONE,
	TASK_REDO,
	TASK_MISSING
};

class MDA_TASK_BASE {
protected:
	AttitudeInput* attitude_input;
	ImageInput* image_input;
	ActuatorOutput* actuator_output;

	MDA_VISION_MODULE_BASE* vision_module;

	void move (ATTITUDE_CHANGE_DIRECTION direction, int delta_accel) {
		actuator_output->set_attitude_change(direction, delta_accel);	
	}
	
public:
	MDA_TASK_BASE (AttitudeInput* a, ImageInput* i, ActuatorOutput * o) :
		attitude_input (a),
		image_input (i),
		actuator_output (o),
		vision_module (NULL)
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
	mvWindow window;

public:
	MDA_TASK_TEST (AttitudeInput* a, ImageInput* i, ActuatorOutput* o);
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
