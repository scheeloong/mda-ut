/* ControlAlgorithm interface.

   This interface represents the operation of the submarine, given attitude and image inputs, and
   a method to actuate the submarine.
*/

#ifndef CONTROL_ALGORITHM_H
#define CONTROL_ALGORITHM_H

#include "AttitudeInput.h"
#include "ImageInput.h"
#include "ActuatorOutput.h"

/* Image Input interface */
class ControlAlgorithm {
  public:
    virtual ~ControlAlgorithm() {}

    virtual void initialize(AttitudeInput *, ImageInput *, ActuatorOutput *) = 0;
    virtual void work() = 0;
};

/* A don't care implementation */
class ControlAlgorithmNull : public ControlAlgorithm {
  public:
    virtual ~ControlAlgorithmNull() {}

    virtual void initialize(AttitudeInput *a, ImageInput *i, ActuatorOutput *o)
    {
      attitude_input = a;
      image_input = i;
      actuator_output = o;
    }
    virtual void work() {}
  private:
    AttitudeInput *attitude_input;
    ImageInput *image_input;
    ActuatorOutput *actuator_output;
};

/* Joystick implementation */
class ControlAlgorithmJoystick : public ControlAlgorithm {
  public:
    virtual ~ControlAlgorithmJoystick() {}

    virtual void initialize(AttitudeInput *a, ImageInput *i, ActuatorOutput *o)
    {
      attitude_input = a;
      image_input = i;
      actuator_output = o;
    }
    virtual void work();
  private:
    AttitudeInput *attitude_input;
    ImageInput *image_input;
    ActuatorOutput *actuator_output;
};

/* Command Line implementation */
class ControlAlgorithmCommandLine : public ControlAlgorithm {
  public:
    virtual ~ControlAlgorithmCommandLine() {}

    virtual void initialize(AttitudeInput *a, ImageInput *i, ActuatorOutput *o)
    {
      attitude_input = a;
      image_input = i;
      actuator_output = o;
    }
    virtual void work();
  private:
    AttitudeInput *attitude_input;
    ImageInput *image_input;
    ActuatorOutput *actuator_output;
};

/* The real submarine implementation */
class ControlAlgorithmSubmarine : public ControlAlgorithm {
  public:
    virtual ~ControlAlgorithmSubmarine();

    virtual void initialize(AttitudeInput *a, ImageInput *i, ActuatorOutput *o);
    virtual void work();
  private:
    AttitudeInput *attitude_input;
    ImageInput *image_input;
    ActuatorOutput *actuator_output;
};

#endif
