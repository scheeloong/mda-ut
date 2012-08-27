#ifndef MOTORS_H
#define MOTORS_H

#include "../sim_new/physical_model.h"
#include <stdlib.h>

#define DEFAULT_ACCEL 1
#define FWD_ACCEL_CONST 0.02
#define DEPTH_ACCEL_CONST 0.06
#define ANG_ACCEL_CONST 0.1

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
    
    // code interface
    void move(MOVE_DIR dir, float accel);
    void move(MOVE_DIR dir) { move(dir, DEFAULT_ACCEL); }
    
    // keyboard interface
    void key_command (char key);

    // Only for the simulator
    void translate(MOVE_DIR dir);
    void debug(DEBUG_CMD cmd);
    
  private:
    physical_model *model; // this deals with velocities in the sim
};

#endif
