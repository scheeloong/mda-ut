#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_GATE:: MDA_TASK_GATE (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
}

MDA_TASK_GATE:: ~MDA_TASK_GATE ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_GATE:: run_task() {
    puts("Press q to quit");

    MDA_VISION_MODULE_GATE gate_vision;

    bool done_gate = false;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    MDA_TASK_BASE::starting_depth = attitude_input->depth();

    move(SINK, 50);

    while (1) {
        if (!done_gate) {
            IplImage* frame = image_input->get_image();
            if (!frame) {
                ret_code = TASK_ERROR;
                break;
            }
            MDA_VISION_RETURN_CODE vision_code = gate_vision.filter(frame);

            // clear dwn image
            image_input->ready_image(DWN_IMG);
 
            if (vision_code == FATAL_ERROR) {
                ret_code = TASK_ERROR;
                break;
            }
            else if (vision_code == NO_TARGET || vision_code == UNKNOWN_TARGET) {
                set(SPEED, 1);
            }
            /*else if (vision_code == ONE_SEGMENT) {
                int ang_x = gate_vision.get_angular_x();

                move(RIGHT, ang_x);

                if (fabs(ang_x) < 5.0) {
                    set(SPEED, 1);
                }
            }*/
            else if (vision_code == FULL_DETECT) {
                // if we can see full gate and range is less than 400 we are done the gate part
                if (gate_vision.get_range() < 400) {
                    // go forward
                    set(SPEED, 1);
                    done_gate = true;
                    continue;
                }

                int ang_x = gate_vision.get_angular_x();

                move(RIGHT, ang_x);

                if (fabs(ang_x) < 5.0) {
                    set(SPEED, 1);
                }
            }
            else {
                printf ("Error: %s: line %d\ntask module recieved an unhandled vision code.\n", __FILE__, __LINE__);
                exit(1);
            }
        }

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
