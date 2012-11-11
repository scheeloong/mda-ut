#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_GATE:: MDA_TASK_GATE (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o),
    window ("task_gate")
{
}

MDA_TASK_GATE:: ~MDA_TASK_GATE ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_GATE:: run_task() {
    puts("Press q to quit");

    // Use gate and path vision
    MDA_VISION_MODULE_GATE gate_vision;
    MDA_VISION_MODULE_PATH path_vision;

    bool done_gate = false;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    while (1) {
        const IplImage* frame = image_input->get_image(FWD_IMG);
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        window.showImage (frame);

        if (!done_gate) {
            MDA_VISION_RETURN_CODE vision_code = gate_vision.filter(frame);
 
            if (vision_code == FATAL_ERROR) {
                ret_code = TASK_ERROR;
                break;
            }
            else if (vision_code == NO_TARGET || vision_code == UNKNOWN_TARGET) {
                actuator_output->set_attitude_change(FORWARD,5);
            }
            else if (vision_code == ONE_SEGMENT || vision_code == FULL_DETECT) {
                int x = gate_vision.get_pixel_x();

                if (x < -frame->width/5)
                    actuator_output->set_attitude_change(LEFT,5);
                else if (x > frame->width/5)
                    actuator_output->set_attitude_change(RIGHT,5);
                else 
                    actuator_output->set_attitude_change(FORWARD,5);

                // if we can see full gate and range is less than 400 we are done the gate part
                if (vision_code == FULL_DETECT && gate_vision.get_range() < 400) {
                    actuator_output->set_attitude_change(FORWARD,5);
                    done_gate = true;
                }
            }
        }
        else {
            // if done gate, we look for path
            const IplImage* down_frame = image_input->get_image(DWN_IMG);
            if (!down_frame) {
                ret_code = TASK_ERROR;
                break;
            }

            MDA_VISION_RETURN_CODE vision_code = path_vision.filter(down_frame);

            if (vision_code == ONE_SEGMENT || vision_code == FULL_DETECT) {
                ret_code = TASK_DONE;
                actuator_output->stop();
                break;
            }
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
