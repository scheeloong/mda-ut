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

    bool seen_gate = false;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    while (1) {
        const IplImage* frame = image_input->get_image(FWD_IMG);
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        window.showImage (frame);

        gate_vision.filter(frame);

        int x = gate_vision.get_pixel_x();
        int y = gate_vision.get_pixel_y();
        int z = gate_vision.get_range();

        if (!seen_gate) {
            actuator_output->set_attitude_change(FORWARD,5);
            if (x != MDA_VISION_MODULE_BASE::VISION_UNDEFINED_VALUE) {
                seen_gate = true;
            }
        } else {
            if (z < 400) {
                // Search for path
                const IplImage* down_frame = image_input->get_image(DWN_IMG);
                if (!down_frame) {
                    ret_code = TASK_ERROR;
                    break;
                }
                
                path_vision.filter(down_frame);
                int path_x = path_vision.get_pixel_x();
                if (path_x != MDA_VISION_MODULE_BASE::VISION_UNDEFINED_VALUE) {
                    ret_code = TASK_DONE;
                    actuator_output->stop();
                    break;
                }
            } else {
                if (x == MDA_VISION_MODULE_BASE::VISION_UNDEFINED_VALUE) {
                    actuator_output->set_attitude_change(FORWARD,5);
                } else if (x < -frame->width/5) {
                    actuator_output->set_attitude_change(LEFT,5);
                } else if (x > frame->width/5) {
                    actuator_output->set_attitude_change(RIGHT,5);
                } else {
                    actuator_output->set_attitude_change(FORWARD,5);
                }
            }
        }

        // Ensure debug messages are printed
        fflush(stdout);
        // Exit if instructed to
        char c = cvWaitKey(1);
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
