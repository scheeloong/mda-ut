#ifndef __MDA_TASK__MDA_TASK__
#define __MDA_TASK__MDA_TASK__

#include "mda_vision.h"
#include "viterbi.h"
#include "AttitudeInput.h"
#include "ImageInput.h"
#include "ActuatorOutput.h"
#include "CharacterStreamSingleton.h"

enum MDA_TASK {
	TASK_TEST,
	TASK_GATE,
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

	void move (ATTITUDE_CHANGE_DIRECTION direction, int delta_accel) {
		actuator_output->set_attitude_change(direction, delta_accel);	
	}
	
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

// gate task with Viterbi algorithm
class MDA_TASK_GATE_VITERBI : public MDA_TASK_BASE {

public:
	MDA_TASK_GATE_VITERBI (AttitudeInput* a, ImageInput* i, ActuatorOutput* o);
	~MDA_TASK_GATE_VITERBI ();

	MDA_TASK_RETURN_CODE run_task ();
private:
  Viterbi *v;
};

/// ########################################################################
/// Path
/// ########################################################################
class MDA_TASK_PATH : public MDA_TASK_BASE {

public:
	MDA_TASK_PATH (AttitudeInput* a, ImageInput* i, ActuatorOutput* o);
	~MDA_TASK_PATH ();

	MDA_TASK_RETURN_CODE run_task ();
};

/// ########################################################################
/// Buoy
/// ########################################################################
class MDA_TASK_BUOY : public MDA_TASK_BASE {

public:
	MDA_TASK_BUOY (AttitudeInput* a, ImageInput* i, ActuatorOutput* o);
	~MDA_TASK_BUOY ();

	MDA_TASK_RETURN_CODE run_task ();
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
