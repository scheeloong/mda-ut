#include <assert.h>
#include <cv.h>

#include "Mission.h"
#include "Operation.h"
#include "CharacterStreamSingleton.h"

void ManualOperation::display_start_message()
{
  // clear regular I/O
  fflush(stdout);

  // ncurses stuff
  initscr();
  clear();
  cbreak();

  // info message
  printw(
         "Commands:\n"
         "  q    - exit simulator\n"
         "  z    - save input image screenshots as image_[fwd/dwn].jpg\n"
         "  y    - toggle display of raw input image stream\n"
         "\n"
         "  wasd - use controller to move forward/reverse/left/right\n"
         "  rf   - use controller to move up/down\n"
         "  'e'  - stop\n"
         "  ijkl - move forward/reverse/left/right\n"
         "  p;   - move up/down\n"
         "  ' '  - nullify all speed and acceleration\n"
         "\n"
         "  v    - enter vision mode\n"
         "\n"
         "  0    - run test task\n"
         "  1    - run gate task\n"
         "  2    - run path task\n"
         "  3    - run buoy task\n"
         "  4    - run frame task\n"
         "  m    - run mission\n"
         "\n");
  refresh();
}

#define SPEED_CHG 5
#define YAW_CHG_IN_DEG 10
#define DEPTH_CHG_IN_CM 200

#define REFRESH_RATE_IN_HZ 60

void ManualOperation::work()
{
  display_start_message();

  // Take keyboard commands
  bool loop = true;
  while (loop) {
    char c = CharacterStreamSingleton::get_instance().wait_key(1000/REFRESH_RATE_IN_HZ);

    // Print yaw and depth unless delayed by another message
    if (c == '\0') {
      if (count < 0) {
        count++;
      } else if (mode != VISION) {
        char buf[128];
        sprintf(buf, "Yaw: %+04d degrees, Depth: %+04d cm, Target Yaw: %+04d degrees, Target Depth: %+04d",
          attitude_input->yaw(), attitude_input->depth(), attitude_input->target_yaw(), attitude_input->target_depth());
        message(buf);
      }
    }

    switch(c) {
      case 'q':
         loop = false;
         break;
      case 'z':
         dump_images();
         break;
      case 'y':
         if (image_input->can_display()) {
           show_raw_images = !show_raw_images;
         } else {
           message_hold("Image stream should already be displayed");
         }
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
         actuator_output->stop();
         break;
      case 'w':
         actuator_output->set_attitude_change(FORWARD, SPEED_CHG);
         break;
      case 's':
         actuator_output->set_attitude_change(REVERSE, SPEED_CHG);
         break;
      case 'a':
         actuator_output->set_attitude_change(LEFT, YAW_CHG_IN_DEG);
         break;
      case 'd':
         actuator_output->set_attitude_change(RIGHT, YAW_CHG_IN_DEG);
         break;
      case 'r':
         actuator_output->set_attitude_change(RISE, DEPTH_CHG_IN_CM);
         break;
      case 'f':
         actuator_output->set_attitude_change(SINK, DEPTH_CHG_IN_CM);
         break;
      case ' ':
         actuator_output->special_cmd(SIM_ACCEL_ZERO);
         break;
      case 'm':
         endwin();
         // Scope mission so that it is destructed before display_start_message
         {
           Mission m(attitude_input, image_input, actuator_output);
           m.work();
         }
         display_start_message();
         message_hold("Mission complete!");
         break;

      case '0':
         if (mode != VISION) {
           endwin();

           MDA_TASK_RETURN_CODE ret_code;
           // Scope task so that it is destructed before display_start_message
           {
             MDA_TASK_TEST test_task(attitude_input, image_input, actuator_output);
             ret_code = test_task.run_task();
           }

           display_start_message();

           switch(ret_code) {
             case TASK_DONE:
                message_hold("Test task completed successfully");
                break;
             case TASK_QUIT:
                message_hold("Test task quit by user");
                break;
             default:
                message_hold("Test task errored out");
                break;
           }
           break;
         }
         delete vision_module;
         message_hold("Selected test vision module\n");
         vision_module = new MDA_VISION_MODULE_TEST();
         use_fwd_img = true;
         break;
      case '1':
         if (mode != VISION) {
           endwin();

           MDA_TASK_RETURN_CODE ret_code;
           // Scope task so that it is destructed before display_start_message
           {
             MDA_TASK_GATE gate_task(attitude_input, image_input, actuator_output);
             ret_code = gate_task.run_task();
           }

           display_start_message();

           switch(ret_code) {
             case TASK_DONE:
                message_hold("Gate task completed successfully");
                break;
             case TASK_QUIT:
                message_hold("Gate task quit by user");
                break;
             default:
                message_hold("Gate task errored out");
                break;
           }
           break;
         }
         delete vision_module;
         message_hold("Selected gate vision module\n");
         vision_module = new MDA_VISION_MODULE_GATE();
         use_fwd_img = true;
         break;
      case '2':
         if (mode != VISION) {
           endwin();

           MDA_TASK_RETURN_CODE ret_code;
           // Scope task so that it is destructed before display_start_message
           {
             MDA_TASK_PATH path_task(attitude_input, image_input, actuator_output);
             ret_code = path_task.run_task();
           }

           display_start_message();

           switch(ret_code) {
             case TASK_DONE:
                message_hold("Path task completed successfully");
                break;
             case TASK_QUIT:
                message_hold("Path task quit by user");
                break;
             default:
                message_hold("Path task errored out");
                break;
           }
           break;
         }
         delete vision_module;
         message_hold("Selected path vision module\n");
         vision_module = new MDA_VISION_MODULE_PATH();
         use_fwd_img = false;
         break;
      case '3':
         if (mode != VISION) {
           endwin();

           MDA_TASK_RETURN_CODE ret_code;
           // Scope task so that it is destructed before display_start_message
           {
             MDA_TASK_BUOY buoy_task(attitude_input, image_input, actuator_output);
             ret_code = buoy_task.run_task();
           }

           display_start_message();

           switch(ret_code) {
             case TASK_DONE:
                message_hold("Buoy task completed successfully");
                break;
             case TASK_QUIT:
                message_hold("Buoy task quit by user");
                break;
             default:
                message_hold("Buoy task errored out");
                break;
           }
           break;
         }
         delete vision_module;
         message_hold("Selected buoy vision module\n");
         vision_module = new MDA_VISION_MODULE_BUOY();
         use_fwd_img = true;
         break;
      case '4':
         if (mode != VISION) {
           endwin();

           MDA_TASK_RETURN_CODE ret_code;
           // Scope task so that it is destructed before display_start_message
           {
             MDA_TASK_FRAME frame_task(attitude_input, image_input, actuator_output);
             ret_code = frame_task.run_task();
           }

           display_start_message();

           switch(ret_code) {
             case TASK_DONE:
                message_hold("Frame task completed successfully");
                break;
             case TASK_QUIT:
                message_hold("Frame task quit by user");
                break;
             default:
                message_hold("Frame task errored out");
                break;
           }
           break;
         }
         delete vision_module;
         message_hold("Selected frame vision module\n");
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
         display_start_message();
         break;
      case 'v':
         if (mode == VISION) {
           break;
         }
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
}

void ManualOperation::process_image()
{
  if (vision_module) {
    const IplImage* frame = image_input->get_image(use_fwd_img?FWD_IMG:DWN_IMG);
    if (frame) {
      vision_module->filter(frame);
      for (int i = 0; i < 3; i++) {
        char ch = cvWaitKey(3);
        CharacterStreamSingleton::get_instance().write_char(ch);
      }
      fflush(stdout);
    } else {
      message_hold("Image stream over");
    }
    if (show_raw_images) {
      // show the other image by getting it
      image_input->get_image(use_fwd_img?DWN_IMG:FWD_IMG);
    }
  } else {
    // needs to be called periodically for highgui event-processing
    if (show_raw_images) {
      image_input->get_image(FWD_IMG);
      image_input->get_image(DWN_IMG);
    }
    char ch = cvWaitKey(3);
    if (ch) {
      CharacterStreamSingleton::get_instance().write_char(ch);
    }
  }
}

void ManualOperation::message(const char *msg)
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

void ManualOperation::message_hold(const char *msg, int delay_in_s)
{
  if (mode == VISION) {
    message(msg);
  } else {
    message(msg);
    count = -delay_in_s * REFRESH_RATE_IN_HZ;
  }
}

void ManualOperation::dump_images()
{
  message_hold("Saved images");
  image_input->dump_images();
}
