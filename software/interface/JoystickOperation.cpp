#include <assert.h>
#include <cv.h>

#include "ImageInput.h"
#include "Operation.h"

void JoystickOperation::work()
{
  // ncurses stuff
  initscr();
  cbreak();

  // regular I/O
  printf(
         "Commands: \r\n"
         "  q    - exit simulator\r\n"
         "  p    - save input image screenshots as image_[fwd/dwn].jpg\r\n"
         "\n"
         "  wasd - use controller to move forward/reverse/left/right\r\n"
         "  rf   - use controller to move up/down\r\n"
         "  ' '  - stop\r\n"
         "  kjhl - move forward/reverse/left/right\r\n"
         "  io   - move up/down\r\n"
         "  e    - nullify all acceleration\r\n"
         "\n");
  fflush(stdout);

  // Take keyboard commands
  bool loop = true;
  while (loop) {
    char c = get_next_char();
    message("");
    switch(c) {
      case 'q':
         loop = false;
         break;
      case 'p':
         dump_images();
         break;
      case 'k':
         actuator_output->special_cmd(SIM_MOVE_FWD);
         break;
      case 'j':
         actuator_output->special_cmd(SIM_MOVE_REV);
         break;
      case 'h':
         actuator_output->special_cmd(SIM_MOVE_LEFT);
         break;
      case 'l':
         actuator_output->special_cmd(SIM_MOVE_RIGHT);
         break;
      case 'i':
         actuator_output->special_cmd(SIM_MOVE_RISE);
         break;
      case 'o':
         actuator_output->special_cmd(SIM_MOVE_SINK);
         break;
      case 'e':
         actuator_output->special_cmd(SIM_ACCEL_ZERO);
         break;
      case 'w':
         actuator_output->set_attitude_change(FORWARD);
         break;
      case 's':
         actuator_output->set_attitude_change(REVERSE);
         break;
      case 'a':
         actuator_output->set_attitude_change(LEFT);
         break;
      case 'd':
         actuator_output->set_attitude_change(RIGHT);
         break;
      case 'r':
         actuator_output->set_attitude_change(RISE);
         break;
      case 'f':
         actuator_output->set_attitude_change(SINK);
         break;
      case ' ':
         actuator_output->stop();
         break;
    }
  }

  // close ncurses
  endwin();
}

void JoystickOperation::message(const char *msg)
{
  int x, y;
  getmaxyx(stdscr, y, x);
  assert(y > 0);

  putchar('\r');
  for (int i = 0; i < x; i++) {
    putchar(' ');
  }
  printf("\r%s", msg);

  fflush(stdout);
}

void JoystickOperation::dump_images()
{
  message("Saved images");
  image_input->dump_images();
}
