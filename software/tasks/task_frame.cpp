#include "mda_vision.h"
#include "mda_tasks.h"

const int MASTER_TIMEOUT = 45;

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
    
    int starting_yaw = attitude_input->yaw();
    printf("Starting yaw: %d\n", starting_yaw);

    bool done_frame = false;

    int FRAME_DEPTH;
    read_mv_setting ("hacks.csv", "FRAME_DEPTH", FRAME_DEPTH);
    
    if (FRAME_DEPTH > 500)
        set(DEPTH, 500);
    if (FRAME_DEPTH > 600)
        set(DEPTH, 600);
    set(DEPTH, FRAME_DEPTH);
    set(YAW, starting_yaw);

    TIMER t;
    t.restart();

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
            else if (vision_code == NO_TARGET) {
                set(SPEED, 5);

                if (t.get_time() > MASTER_TIMEOUT) {
                    stop();
                    return TASK_MISSING;
                }
            }
            else if (vision_code == FULL_DETECT) {
                int ang_x = frame_vision.get_angular_x();
                int ang_y = frame_vision.get_angular_y();
                int range = frame_vision.get_range();
                int depth_change = tan(ang_y*0.017453) * range; 

                // if we can see full frame and range is less than 400 we are done the frame part
                if (frame_vision.get_range() < 350) {
                    t.restart();
                    while (t.get_time() < 5) {
                        set (SPEED, 8);
                    }
                    stop();
                    done_frame = true;
                    ret_code = TASK_DONE;
                    break;
                }

                if(fabs(ang_y) > 30.0) {
                    stop();
                    move(SINK, depth_change);
                }
                else if(abs(ang_x) > 10.0) {
                    stop();
                    move(RIGHT, ang_x);
                }
                else {
                    set(SPEED, 5);
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
