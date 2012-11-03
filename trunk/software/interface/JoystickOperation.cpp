#include <assert.h>
#include <cv.h>

#include "ImageInput.h"
#include "Operation.h"
#include "mda_vision.h"
#include "mda_tasks.h"
#include "vci.h"

enum MDA_VISION_OR_TASK_ENUM {
  NONE,
  TEST_VISION,
  TEST_TASK,
  GATE_VISION,
  GATE_TASK
};

MDA_VISION_OR_TASK_ENUM vision_task_enum = NONE;
MDA_VISION_MODULE_BASE* vision_module = NULL;
MDA_TASK_BASE* task_module = NULL;
VCI vci; // remove this

void create_vision_or_task (char c) {
  // destroy the current module if we have one
  if (((c >= '0') && (c <= '9')) || (c == 'v')) {
    if (vision_module != NULL) {
      delete vision_module;
    }
    if (task_module != NULL) {
      delete task_module;
    }
  }

  // make new module
  switch (c) {
    case '1':
      vision_task_enum = TEST_VISION;
      vision_module = new MDA_VISION_MODULE_TEST();
      printw ("Selected TEST_VISION\n");
      refresh ();
      break;
    case '2':
      vision_task_enum = GATE_VISION;
      vision_module = new MDA_VISION_MODULE_GATE();
      printw ("Selected GATE_VISION\n");
      refresh ();
      break;
    case 'v':
      vision_task_enum = NONE;
      break;
    default:
      printw ("Unknown Selection %c\n", c);
      refresh ();
  }
}

void JoystickOperation::work()
{
  // ncurses stuff
  initscr();
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
      case 'v':
         printw(
           "Entering Vision Mode:\n"
           "  1    - test vision\n"
           "  2    - gate vision"
           "  3    - path vision\n"
           "  4    - frame vision\n"
           "  5    - \n"
           "  6    - test controller\n"
           "  7    - gate contoller"
           "  8    - path controller\n"
           "  9    - frame controller\n"
           "  0    - \n"
           "  q    - exit vision mode\n"
           "  v    - turn off all vision\n"
           "\n"
         );
         refresh();

         char task = 0;
         while (task == 0) {
            task = get_next_char();
         };

         printw ("task = %c\n",task);
         refresh();
        
         create_vision_or_task (task);
    }

    const IplImage* frame = image_input->get_image(FWD_IMG);

    switch (vision_task_enum) {
      case TEST_VISION:
      case GATE_VISION:
        vision_module->filter (frame, &vci);
        break;
      default:
        break;
    }
  }

  if (vision_module != NULL)
    delete vision_module;
  if (task_module != NULL)
    delete task_module;

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
