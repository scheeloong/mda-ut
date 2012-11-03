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
#include "SimulatorSingleton.h"

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

    char get_next_char()
    {
      fd_set readfds;
      struct timeval tv = {2, 0};

      // May use ncurses
      FILE *sim_fd = SimulatorSingleton::get_instance().read_fp();
      int sim_fileno = 0;

      FD_ZERO(&readfds);
      FD_SET(STDIN_FILENO, &readfds);
      if (sim_fd) {
        sim_fileno = fileno(sim_fd);
        FD_SET(sim_fileno, &readfds);
      }
        
      if (select(FD_SETSIZE, &readfds, NULL, NULL, &tv) > 0) {
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
          return fgetc(stdin);
        }
        if (sim_fd && FD_ISSET(sim_fileno, &readfds)) {
          return fgetc(sim_fd);
        }
      }

      return '\0';
    }

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
    JoystickOperation(AttitudeInput *a, ImageInput *i, ActuatorOutput *o) : Operation(a, i, o) {}
    virtual ~JoystickOperation() {}

    virtual void work();
  private:
    void dump_images();
    void message(const char *);
};

/* Command Line implementation */
class CommandLineOperation: public Operation {
  public:
    CommandLineOperation(AttitudeInput *a, ImageInput *i, ActuatorOutput *o) : Operation(a, i, o) {}
    virtual ~CommandLineOperation() {}

    virtual void work();
};

#endif
