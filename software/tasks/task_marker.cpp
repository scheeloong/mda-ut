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

    int markers_dropped = 0;
    static const int starting_depth = 630;

    // this is rough depth of the marker dropper targets
    actuator_output->set_attitude_absolute(DEPTH, starting_depth);
    sleep(2);

    while (!done_marker) {
        IplImage* frame = image_input->get_image(DWN_IMG);
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        MDA_VISION_RETURN_CODE vision_code = marker_vision.filter(frame);

        // clear fwd image
        int fwd_frame_ready = image_input->ready_image();
        (void) fwd_frame_ready;
        
        if (vision_code == FATAL_ERROR) {
            ret_code = TASK_ERROR;
            break;
        }
        else if (vision_code == NO_TARGET || vision_code == UNKNOWN_TARGET) {
            actuator_output->set_attitude_change(FORWARD);
        }
        else if (vision_code == FULL_DETECT) {
            markers_dropped++;
            printf("Dropping marker here.\n");
            fflush(stdout);
            actuator_output->stop();
            sleep(2);

            if (markers_dropped == 2) {
                ret_code = TASK_DONE;
                printf ("Done marker task.\n");
                break;
            }

            // continue going forward to search for more targets
            actuator_output->set_attitude_change(FORWARD);
            sleep(1);
        }
        else {
            printf ("Error: %s: line %d\ntask module recieved an unhandled vision code.\n", __FILE__, __LINE__);
            exit(1);
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