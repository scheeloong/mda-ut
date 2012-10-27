/* ControlAlgorithm interface.

   This interface represents the operation of the submarine, given attitude and image inputs, and
   a method to actuate the submarine.
*/

#ifndef CONTROL_ALGORITHM_H
#define CONTROL_ALGORITHM_H

#include <curses.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

#include "AttitudeInput.h"
#include "ImageInput.h"
#include "ActuatorOutput.h"
#include "SimulatorSingleton.h"

/* Image Input interface */
class ControlAlgorithm {
  public:
    virtual ~ControlAlgorithm() {}

    virtual void initialize(AttitudeInput *, ImageInput *, ActuatorOutput *) = 0;
    virtual void work() = 0;

    char get_next_char()
    {
      fd_set readfds;
      struct timeval tv = {100000, 0};

      // May use ncurses
      while (1) {
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
      }
    }
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
    virtual void work()
    {
        // ncurses stuff
        initscr();
        cbreak();

        // regular io
        printf("Press 'q' to quit...");
        fflush(stdout);

        // wait on glut window and terminal for quit character
        while (get_next_char() != 'q')
          ;

        // close ncurses
        endwin();
    }
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
