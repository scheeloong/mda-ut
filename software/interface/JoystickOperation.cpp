#include <assert.h>
#include <cv.h>

#include "Operation.h"

void JoystickOperation::display_start_message()
{
  // ncurses stuff
  initscr();
  clear();
  cbreak();

  // info message
  printw(
         "Commands:\n"
         "  q    - exit simulator\n"
         "  p    - save input image screenshots as image_[fwd/dwn].jpg\n"
         "\n"
         "  wasd - use controller to move forward/reverse/left/right\n"
         "  rf   - use controller to move up/down\n"
         "  ' '  - stop\n"
         "  kjhl - move forward/reverse/left/right\n"
         "  io   - move up/down\n"
         "  e    - nullify all acceleration\n"
         "\n");
  refresh();
}

void JoystickOperation::work()
{
  display_start_message();
  int count = 0;

  // Take keyboard commands
  bool loop = true;
  while (loop) {
    char c = get_next_char();

    // Clear input every 2 seconds
    count++;
    if (c == '\0' && count % 128 == 0) {
      message("");
    }

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
      case '1':
         if (mode != VISION) {
           break;
         }
         delete vision_module;
         message("Selected test vision module\n");
         vision_module = new MDA_VISION_MODULE_TEST();
         break;
      case '2':
         if (mode != VISION) {
           break;
         }
         delete vision_module;
         message("Selected test vision module\n");
         vision_module = new MDA_VISION_MODULE_GATE();
         break;
      case 'x':
         if (mode == NORMAL) {
           break;
         }
         delete vision_module;
         vision_module = NULL;
         mode = NORMAL;
         fflush(stdout);
         display_start_message();
         break;
      case 'v':
         endwin();
         mode = VISION;
         message(
           "Entering Vision Mode:\n"
           "  1    - test vision\n"
           "  2    - gate vision"
           "\n"
           "  x    - exit vision mode\n"
         );
         break;
      case '\0': // timeout
        process_image();
        break;
    }
  }

  // close ncurses
  endwin();

  delete vision_module;
}

void JoystickOperation::process_image()
{
  if (vision_module) {
    const IplImage* frame = image_input->get_image(FWD_IMG);
    vision_module->filter(frame, &vci);
    fflush(stdout);
  }
}

void JoystickOperation::message(const char *msg)
{
  if (mode == VISION) {
    if (strlen(msg) == 0) {
      return;
    }
    printf("%s\n", msg);
    fflush(stdout);
    return;
  }

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
