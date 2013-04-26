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
    const int depth_threshold = 10;
    const int yaw_threshold = 5;
    const int fwd_timesteps = 25;
    const int speed = 2;

    enum state {SINK_STATE, TURN_FWD_STATE, FWD_STATE, RISE_STATE, TURN_REV_STATE, REV_STATE};
    enum state cur_state = SINK_STATE;

    int fwd_yaw = attitude_input->yaw();
    int rev_yaw = fwd_yaw + 180;
    if (rev_yaw > 180) rev_yaw -= 360;
    int counter = 0;

    printf("Sinking\n");

    while (1) {
        IplImage* frame = image_input->get_image(FWD_IMG);
        if (!frame) {
            break;
        }
        window.showImage (frame);

        image_input->ready_image(DWN_IMG);

        switch (cur_state) {
          case SINK_STATE:
            actuator_output->set_attitude_absolute(DEPTH, sink_depth);
            actuator_output->set_attitude_absolute(SPEED, 0);
            if (abs(attitude_input->depth() - sink_depth) < depth_threshold) {
              printf("Going forward\n");
              cur_state = TURN_FWD_STATE;
              counter = 0;
              actuator_output->set_attitude_absolute(YAW, fwd_yaw);
            }
            break;
          case TURN_FWD_STATE:
            if (abs(attitude_input->yaw() - fwd_yaw) < yaw_threshold || abs(attitude_input->yaw() - fwd_yaw) > 360 - yaw_threshold) {
              cur_state = FWD_STATE;
            }
            break;
          case FWD_STATE:
            actuator_output->set_attitude_absolute(SPEED, speed);
            printf("Current timestep: %d Maximum timestep: %d\n", counter, fwd_timesteps);
            counter++;
            if (counter == fwd_timesteps) {
              printf("Rising\n");
              cur_state = RISE_STATE;
            }
            break;
          case RISE_STATE:
            actuator_output->set_attitude_absolute(DEPTH, rise_depth);
            actuator_output->set_attitude_absolute(SPEED, 0);
            if (abs(attitude_input->depth() - rise_depth) < depth_threshold) {
              printf("Going back\n");
              cur_state = TURN_REV_STATE;
              counter = 0;
              actuator_output->set_attitude_absolute(YAW, rev_yaw);
            }
            break;
          case TURN_REV_STATE:
            if (abs(attitude_input->yaw() - rev_yaw) < yaw_threshold || abs(attitude_input->yaw() - rev_yaw) > 360 - yaw_threshold) {
              cur_state = REV_STATE;
            }
            break;
          case REV_STATE:
            actuator_output->set_attitude_absolute(SPEED, speed);
            printf("Current timestep: %d Maximum timestep: %d\n", counter, fwd_timesteps);
            counter++;
            if (counter == fwd_timesteps) {
              printf("Sinking\n");
              cur_state = SINK_STATE;
            }
            break;
        }

        printf("Yaw: %d Depth %d\n", attitude_input->yaw(), attitude_input->depth());

        // Ensure debug messages are printed
        fflush(stdout);
        // Exit if instructed to
        char c = cvWaitKey(5);
        if (c != -1) {
            CharacterStreamSingleton::get_instance().write_char(c);
        }
        if (CharacterStreamSingleton::get_instance().wait_key(1) == 'q'){
            actuator_output->stop();
            ret_code = TASK_QUIT;
            break;
        }
    }

    return ret_code;
}
