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
         "  z    - save input image screenshots as image_[fwd/dwn].jpg\n"
         "\n"
         "  wasd - use controller to move forward/reverse/left/right\n"
         "  rf   - use controller to move up/down\n"
         "  ' '  - stop\n"
         "  ijkl - move forward/reverse/left/right\n"
         "  p;   - move up/down\n"
         "  e    - nullify all acceleration\n"
         "\n"
         "  v    - enter vision mode\n"
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
      case 'z':
         dump_images();
         break;
      case 'i':
         actuator_output->special_cmd(SIM_MOVE_FWD);
         break;
      case 'k':
         actuator_output->special_cmd(SIM_MOVE_REV);
         break;
      case 'j':
         actuator_output->special_cmd(SIM_MOVE_LEFT);
         break;
      case 'l':
         actuator_output->special_cmd(SIM_MOVE_RIGHT);
         break;
      case 'p':
         actuator_output->special_cmd(SIM_MOVE_RISE);
         break;
      case ';':
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

      case '0':
         if (mode != VISION) {
           break;
         }
         delete vision_module;
         message("Selected test vision module\n");
         vision_module = new MDA_VISION_MODULE_TEST();
         use_fwd_img = true;
         break;
      case '1':
         if (mode != VISION) {
           break;
         }
         delete vision_module;
         message("Selected gate vision module\n");
         vision_module = new MDA_VISION_MODULE_GATE();
         use_fwd_img = true;
         break;
      case '2':
         if (mode != VISION) {
           break;
         }
         delete vision_module;
         message("Selected path vision module\n");
         vision_module = new MDA_VISION_MODULE_PATH();
         use_fwd_img = false;
         break;
      case '3':
         if (mode != VISION) {
           break;
         }
         delete vision_module;
         message("Selected buoy vision module\n");
         vision_module = new MDA_VISION_MODULE_BUOY();
         use_fwd_img = true;
         break;
      case '4':
         if (mode != VISION) {
           break;
         }
         delete vision_module;
         message("Selected frame vision module\n");
         vision_module = new MDA_VISION_MODULE_FRAME();
         use_fwd_img = true;
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
           "  0    - test vision\n"
           "  1    - gate vision\n"
           "  2    - path vision\n"
           "  3    - buoy vision\n"
           "  4    - frame vision\n"
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
    const IplImage* frame = image_input->get_image(use_fwd_img?FWD_IMG:DWN_IMG);
    if (frame) {
      vision_module->filter(frame);
      fflush(stdout);
    } else {
      message("Image stream over");
    }
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
