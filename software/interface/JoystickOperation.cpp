#include "Operation.h"

void JoystickOperation::work()
{
  // ncurses stuff
  initscr();
  cbreak();

  // regular I/O
  printf(
         "Commands: \r\n"
         "  q - exit simulator\r\n"
         "  p - save input image screenshots as image_[fwd/dwn].jpg\r\n"
         "\n");
  fflush(stdout);

  // Take keyboard commands
  bool loop = true;
  while (loop) {
    char c = get_next_char();
    switch(c) {
      case 'q':
         loop = false;
         break;
      case 'p':
         dump_image();
         break;
    }
  }

  // close ncurses
  endwin();
}

void JoystickOperation::dump_image()
{
  clrtoeol();
  printf("\rSaved images");
  fflush(stdout);
}
