#include "mda_vision.h"
#include "mda_tasks.h"

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
        IplImage* frame = image_input->get_image();
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        MDA_VISION_RETURN_CODE vision_code = frame_vision.filter(frame);

        // clear dwn image
        int down_frame_ready = image_input->ready_image(DWN_IMG);
        (void) down_frame_ready;

        if(!done_frame){
            if (vision_code == FATAL_ERROR) {
                ret_code = TASK_ERROR;
                break;
            }
            else if (vision_code == NO_TARGET || vision_code == UNKNOWN_TARGET) {
                set(SPEED, 1);
            }
            else if (vision_code == ONE_SEGMENT) {
                int ang_x = frame_vision.get_angular_x();
                int ang_y = frame_vision.get_angular_y();
                int range = frame_vision.get_range();
                int depth_change = tan(ang_y*0.017453) * range; 

                if(fabs(ang_y) > 10.0) move(SINK,depth_change);
                else if(abs(ang_x) > 5.0) move(RIGHT, ang_x);
                else set(SPEED, 1);
            } 
            else if (vision_code == FULL_DETECT) {
                // if we can see full frame and range is less than 400 we are done the frame part
                if (frame_vision.get_range() < 400) {
                    // set target yaw to current yaw and go forward
                    stop();
                    done_frame = true;
                    ret_code = TASK_DONE;
                    break;
                }

                int ang_x = frame_vision.get_angular_x();
                int ang_y = frame_vision.get_angular_y();
                int range = frame_vision.get_range();
                int depth_change = tan(ang_y*0.017453) * range; 


                if(fabs(ang_y) > 10.0) move(SINK, depth_change);
                else if(abs(ang_x) > 5.0) move(RIGHT, ang_x);
                else set(SPEED, 1);
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
