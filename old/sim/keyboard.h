#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../motors/motors.h"

class KeyboardInput {
  public:
    KeyboardInput(Motors motors) : m(motors), speed(0) {}
    void read_input(char key);
  private:
    Motors m;
    int speed;
};

#endif
