#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_MARKER:: MDA_TASK_MARKER (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
}

MDA_TASK_MARKER:: ~MDA_TASK_MARKER ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_MARKER:: run_task() {
    puts("Press q to quit");

    MDA_VISION_MODULE_MARKER marker_vision;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;
    bool done_marker = false;

    while (1) {
        IplImage* frame = image_input->get_image();
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        MDA_VISION_RETURN_CODE vision_code = marker_vision.filter(frame);
        
        if (!done_marker) {
            if (vision_code == FATAL_ERROR) {
                ret_code = TASK_ERROR;
                break;
            }
            else if (vision_code == NO_TARGET || vision_code == UNKNOWN_TARGET) {
                actuator_output->set_attitude_change(FORWARD);
            }
            else if (vision_code == FULL_DETECT) {
                ret_code = TASK_DONE;
                actuator_output->stop();
                printf ("Plane Found.\n");
                break;
            }
            else {
                printf ("Error: %s: line %d\ntask module recieved an unhandled vision code.\n", __FILE__, __LINE__);
                exit(1);
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
            ret_code = TASK_QUIT;
            break;
        }
    }

    return ret_code;
}