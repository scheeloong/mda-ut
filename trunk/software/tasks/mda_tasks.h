#ifndef __MDA_TASK__MDA_TASK__
#define __MDA_TASK__MDA_TASK__

#include <time.h>

#include "mda_vision.h"
#include "AttitudeInput.h"
#include "ImageInput.h"
#include "ActuatorOutput.h"
#include "CharacterStreamSingleton.h"

enum MDA_TASK {
	TASK_TEST,
	TASK_GATE,
	TASK_MARKER,
	TASK_PATH,
	TASK_BUOY,
	TASK_FRAME
};

enum MDA_TASK_RETURN_CODE {
	TASK_ERROR, 
	TASK_DONE,
	TASK_REDO,
	TASK_MISSING,
	TASK_QUIT
};

class MDA_TASK_BASE {
protected:
	AttitudeInput* attitude_input;
	ImageInput* image_input;
	ActuatorOutput* actuator_output;

	// Move or set attitude until stable
	void move (ATTITUDE_CHANGE_DIRECTION direction, int delta_accel) {
		actuator_output->set_attitude_change(direction, delta_accel);	
		ATTITUDE_DIRECTION dir;
		switch (direction) {
			case LEFT:
			case RIGHT:
				dir = YAW;
				break;
			case RISE:
			case SINK:
				dir = DEPTH;
				break;
			default:
				dir = SPEED;
				break;
		}
		stabilize(dir);
	}
	void set (ATTITUDE_DIRECTION dir, int val) {
		actuator_output->set_attitude_absolute(dir, val);	
		stabilize(dir);
	}
	void stabilize (ATTITUDE_DIRECTION dir) {
		if (dir == SPEED) {
			return; // No need to stabilize
		}

		const int yaw_threshold = 3, depth_threshold = 15;
		double max_elapsed_seconds = 10.;
		time_t start_time = time(NULL);
		while (1) {
			image_input->ready_image(FWD_IMG);
			image_input->ready_image(DWN_IMG);

			// Exit in max_elapsed_seconds to prevent hanging
			if (difftime(time(NULL), start_time) > max_elapsed_seconds) {
				break;
			}

			if (dir == YAW) {
				int current_yaw = attitude_input->yaw();
				int target_yaw = actuator_output->get_target_attitude(YAW);
				if (abs(current_yaw - target_yaw) <= yaw_threshold ||
				    abs(current_yaw - target_yaw) >= 360 - yaw_threshold) {
					break;
				}
			} else if (dir == DEPTH) {
				int current_depth = attitude_input->depth();
				int target_depth = actuator_output->get_target_attitude(DEPTH);
				if (abs(current_depth - target_depth) <= depth_threshold) {
					break;
				}
			}
		}
	}

// Wait for user to press a key
#ifdef DEBUG_FRAME_BY_FRAME
	static const int TASK_WK = 0;
#else
	static const int TASK_WK = 5;
#endif
	
public:
	MDA_TASK_BASE (AttitudeInput* a, ImageInput* i, ActuatorOutput * o) :
		attitude_input (a),
		image_input (i),
		actuator_output (o)
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
class MDA_TASK_GATE : public MDA_TASK_BASE {

public:
	MDA_TASK_GATE (AttitudeInput* a, ImageInput* i, ActuatorOutput* o);
	~MDA_TASK_GATE ();

	MDA_TASK_RETURN_CODE run_task ();
};

/// ########################################################################
/// Marker
/// ########################################################################
class MDA_TASK_MARKER : public MDA_TASK_BASE {

public:
	MDA_TASK_MARKER (AttitudeInput* a, ImageInput* i, ActuatorOutput* o);
	~MDA_TASK_MARKER ();

	MDA_TASK_RETURN_CODE run_task ();
};


/// ########################################################################
/// Path
/// ########################################################################
class MDA_TASK_PATH : public MDA_TASK_BASE {
	static const int DEPTH_TARGET = 350;
	int pix_x_old;
	int pix_y_old;

public:
	MDA_TASK_PATH (AttitudeInput* a, ImageInput* i, ActuatorOutput* o);
	~MDA_TASK_PATH ();

	MDA_TASK_RETURN_CODE run_task ();
};

/// ########################################################################
/// Buoy
/// ########################################################################
class MDA_TASK_BUOY : public MDA_TASK_BASE {
	static const int BUOY_RANGE_WHEN_DONE = 40; // range to the buoy to consider the tracking finished, in cm

	enum BUOY_COLOR {
		BUOY_RED,
		BUOY_YELLOW,
		BUOY_GREEN
	};

public:
	MDA_TASK_BUOY (AttitudeInput* a, ImageInput* i, ActuatorOutput* o); // defaults to red buoy
	~MDA_TASK_BUOY ();

	MDA_TASK_RETURN_CODE run_task ();
	MDA_TASK_RETURN_CODE run_single_buoy (BUOY_COLOR color);

};

/// ########################################################################
/// U shaped Frame
/// ########################################################################
class MDA_TASK_FRAME : public MDA_TASK_BASE {

public:
	MDA_TASK_FRAME (AttitudeInput* a, ImageInput* i, ActuatorOutput* o);
	~MDA_TASK_FRAME ();

	MDA_TASK_RETURN_CODE run_task ();
};

#endif
