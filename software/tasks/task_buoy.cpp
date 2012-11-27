#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_BUOY:: MDA_TASK_BUOY (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
}

MDA_TASK_BUOY:: ~MDA_TASK_BUOY ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_BUOY:: run_task() {
    puts("Press q to quit");

    MDA_VISION_MODULE_BUOY buoy_vision;
    MDA_VISION_MODULE_PATH path_vision;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    while (1) {
        const IplImage* frame = image_input->get_image();
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }

        MDA_VISION_RETURN_CODE vision_code = buoy_vision.filter(frame);
        (void) vision_code;

        /* This code skips the buoy task */
        const IplImage* down_frame = image_input->get_image(DWN_IMG);
        vision_code = path_vision.filter(down_frame);
        if (vision_code == ONE_SEGMENT || vision_code == FULL_DETECT || vision_code == UNKNOWN_TARGET) {
            actuator_output->set_attitude_change(FORWARD);
        } else {
            ret_code = TASK_DONE;
            actuator_output->stop();
            break;
        }
        /* End buoy task skipping code */

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
