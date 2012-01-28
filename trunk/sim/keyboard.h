#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../mission/mission.h"

class KeyboardInput {
  public:
    KeyboardInput(Mission mission) : m(mission), speed(0) {}
    void read_input(char key);
  private:
    Mission m;
    int speed;
};

#endif
