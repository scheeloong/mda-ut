#include <assert.h>
#include <cv.h>
#include <highgui.h>

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
      case '\0':
         message("");
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

void JoystickOperation::dump_image()
{
  message("Saved images");
  const IplImage *img_fwd = image_input->get_image();
  if (img_fwd) {
    cvSaveImage ("image_fwd.jpg", img_fwd);
  }

  const IplImage *img_dwn = image_input->get_image(DWN_IMG);
  if (img_dwn) {
    cvSaveImage ("image_dwn.jpg", img_dwn);
  }
}
