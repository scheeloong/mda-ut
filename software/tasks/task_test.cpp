#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_TEST:: MDA_TASK_TEST (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o),
    window ("task_test")
{
}

MDA_TASK_TEST:: ~MDA_TASK_TEST ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_TEST:: run_task() {
    puts("Press q to quit");

    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    const int sink_depth = 450;
    const int rise_depth = 300;

    enum state {SINK_STATE, FWD_STATE, RISE_STATE, REV_STATE};
    enum state cur_state = SINK_STATE;

    while (1) {
        IplImage* frame = image_input->get_image(FWD_IMG);
        if (!frame) {
            break;
        }
        window.showImage (frame);

        image_input->ready_image(DWN_IMG);

        switch (cur_state) {
          case SINK_STATE:
            set(DEPTH, sink_depth);
            cur_state = FWD_STATE;
            move(LEFT, 20);
            move(RIGHT, 40);
            move(LEFT, 20);
            break;
          case FWD_STATE:
            move(FORWARD, 5); // 5 seconds
            cur_state = RISE_STATE;
            break;
          case RISE_STATE:
            set(DEPTH, rise_depth);
            cur_state = REV_STATE;
            move(RIGHT, 20);
            move(LEFT, 40);
            move(RIGHT, 20);
            break;
          case REV_STATE:
            move(REVERSE, 5); // 5 seconds
            cur_state = SINK_STATE;
            break;
        }

        printf("Yaw: %d Depth %d\n", attitude_input->yaw(), attitude_input->depth());

        // Ensure debug messages are printed
        fflush(stdout);
        // Exit if instructed to
        char c = cvWaitKey(TASK_WK);
        if (c != -1) {
            CharacterStreamSingleton::get_instance().write_char(c);
        }
        if (CharacterStreamSingleton::get_instance().wait_key(1) == 'q'){
            stop();
            ret_code = TASK_QUIT;
            break;
        }
    }

    return ret_code;
}
