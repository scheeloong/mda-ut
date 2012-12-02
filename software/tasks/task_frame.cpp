#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_FRAME:: MDA_TASK_FRAME (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
}

MDA_TASK_FRAME:: ~MDA_TASK_FRAME ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_FRAME:: run_task() {
    puts("Press q to quit");

    MDA_VISION_MODULE_FRAME frame_vision;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    bool done_frame = false;

    while (1) {
        const IplImage* frame = image_input->get_image(FWD_IMG);
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        if(!done_frame){
            MDA_VISION_RETURN_CODE vision_code = frame_vision.filter(frame);
            if (vision_code == FATAL_ERROR) {
                ret_code = TASK_ERROR;
                break;
            }
            else if (vision_code == NO_TARGET || vision_code == UNKNOWN_TARGET) {
                actuator_output->set_attitude_change(FORWARD);
            }
            else if (vision_code == ONE_SEGMENT) {
                int ang_x = frame_vision.get_angular_x();
                //int ang_y = frame_vision.get_angular_y();

                //if(ang_y < 5.0) actuator_output->set_attitude_change(SINK);
                //else if(ang_y > 5.0) actuator_output->set_attitude_change(RISE);
                if(abs(ang_x) > 5.0) actuator_output->set_attitude_change(RIGHT, ang_x);
                //else if(ang_x < -5.0) actuator_output->set_attitude_change(LEFT, ang_x);
                else actuator_output->set_attitude_change(FORWARD);
            } 
            else if (vision_code == FULL_DETECT) {
                // if we can see full frame and range is less than 400 we are done the frame part
                if (frame_vision.get_range() < 400) {
                    // set target yaw to current yaw and go forward
                    actuator_output->stop();
                    done_frame = true;
                    ret_code = TASK_DONE;
                    break;
                }

                int ang_x = frame_vision.get_angular_x();
                //int ang_y = frame_vision.get_angular_y();

                //if(ang_y < 5.0) actuator_output->set_attitude_change(SINK);
                //else if(ang_y > 5.0) actuator_output->set_attitude_change(RISE);
                if(abs(ang_x) > 5.0) actuator_output->set_attitude_change(RIGHT, ang_x);
                //else if(ang_x < -5.0) actuator_output->set_attitude_change(LEFT, ang_x);
                else actuator_output->set_attitude_change(FORWARD);
            }

            //(void) vision_code;

        }

        // Ensure debug messages are printed
        fflush(stdout);
        // Exit if instructed to
        char c = cvWaitKey(3);
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
