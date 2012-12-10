/* Operation interface.

   This interface represents the operation of the submarine, given attitude and image inputs, and
   a method to actuate the submarine.
*/

#ifndef OPERATION_H
#define OPERATION_H

#include <curses.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

#include "AttitudeInput.h"
#include "ImageInput.h"
#include "ActuatorOutput.h"
#include "CharacterStreamSingleton.h"
#include "mda_vision.h"
#include "mda_tasks.h"

/* Operation interface */
class Operation {
  public:
    Operation(AttitudeInput *a, ImageInput *i, ActuatorOutput *o)
    {
      attitude_input = a;
      image_input = i;
      actuator_output = o;
    }
    virtual ~Operation() {}

    // to be implemented by the derived class!
    virtual void work() = 0;
  protected:
    AttitudeInput *attitude_input;
    ImageInput *image_input;
    ActuatorOutput *actuator_output;
};

/* A don't care implementation */
class OperationNull : public Operation {
  public:
    OperationNull(AttitudeInput *a, ImageInput *i, ActuatorOutput *o) : Operation(a, i, o) {}
    virtual ~OperationNull() {}

    virtual void work();
};

/* Joystick implementation */
class JoystickOperation: public Operation {
  public:
    JoystickOperation(AttitudeInput *a, ImageInput *i, ActuatorOutput *o) : Operation(a, i, o),
      mode(NORMAL), vision_module(NULL), use_fwd_img(true), show_raw_images(false), count(0) {}
    virtual ~JoystickOperation() { delete vision_module; }

    virtual void work();
  private:
    void dump_images();
    void message(const char *);
    void message_hold(const char *, int delay_in_s = 2);
    void display_start_message();
    void process_image();

    enum MDA_JOYSTICK_MODE {
      NORMAL,
      TASK,
      VISION
    };

    MDA_JOYSTICK_MODE mode;
    MDA_VISION_MODULE_BASE* vision_module;
    bool use_fwd_img, show_raw_images;
    int count;
};

/* Command Line implementation */
class CommandLineOperation: public Operation {
  public:
    CommandLineOperation(AttitudeInput *a, ImageInput *i, ActuatorOutput *o) : Operation(a, i, o) {}
    virtual ~CommandLineOperation() {}

    virtual void work();
};

#endif
