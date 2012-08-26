#ifndef MOTORS_H
#define MOTORS_H

#include "../sim/physical_model.h"
#include <stdlib.h>

#define DEFAULT_SPEED 1

enum MOVE_DIR {
  STOP,
  FORWARD,
  REVERSE,
  LEFT,
  RIGHT,
  SINK,
  RISE
};

enum DEBUG_CMD {
  SCREENSHOT,
  RESET_POS,
  RESET_ALL,
  CMD_EXIT
};

class Motors {
  public:
    Motors (physical_model *m = NULL) : model(m) {}
    void move(MOVE_DIR dir, float speed);
    void move(MOVE_DIR dir) { move(dir, DEFAULT_SPEED); }

    // Only for the simulator
    void translate(MOVE_DIR dir);
    void debug(DEBUG_CMD cmd);
    
  private:
    physical_model *model; // this deals with velocities in the sim
    float accel_x, accel_y, accel_z;
};

#endif
