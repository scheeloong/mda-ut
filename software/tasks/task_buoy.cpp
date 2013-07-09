#include "mda_vision.h"
#include "mda_tasks.h"

MDA_TASK_BUOY:: MDA_TASK_BUOY (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
}


MDA_TASK_BUOY:: ~MDA_TASK_BUOY ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_BUOY:: run_task() {
    MDA_TASK_RETURN_CODE code;
/*
    code = run_single_buoy(BUOY_YELLOW);
    if (code != TASK_DONE) {
        printf ("Something wrong in task_buoy!\n");
        return TASK_ERROR;
    }
*/
    code = run_single_buoy(BUOY_RED);
    if (code != TASK_DONE) {
        printf ("Something wrong in task_buoy!\n");
        return TASK_ERROR;
    }
/*    
    code = run_single_buoy(BUOY_GREEN);
     if (code != TASK_DONE) {
        printf ("Something wrong in task_buoy!\n");
        return TASK_ERROR;
    }
*/
    return TASK_DONE;
}

MDA_TASK_RETURN_CODE MDA_TASK_BUOY:: run_single_buoy(BUOY_COLOR color) {
    puts("Press q to quit");
    
    MDA_VISION_MODULE_BUOY buoy_vision;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    /// Here we store the starting attitude vector, so we can return to this attitude later
    int starting_yaw = attitude_input->yaw();
    printf("Starting yaw: %d\n", starting_yaw);

    bool done_buoy = false;
    int EMA_range = 200;

    while (1) {
        IplImage* frame = image_input->get_image();
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }

        MDA_VISION_RETURN_CODE vision_code = buoy_vision.filter(frame);

        // clear dwn image
        int down_frame_ready = image_input->ready_image(DWN_IMG);
        (void) down_frame_ready;

        if (!done_buoy) {
            if (vision_code == FATAL_ERROR) {
                ret_code = TASK_ERROR;
                break;
            }
            else if (vision_code == NO_TARGET) {
                //set(SPEED, 1);
            }
            else if (vision_code == FULL_DETECT) {
                // range is valid, so we can use it to calculate the right depth
                int ang_x = buoy_vision.get_angular_x();
                int ang_y = buoy_vision.get_angular_y();
                int range = buoy_vision.get_range();

                int depth_change = tan(ang_y*0.017453) * range; 

                // we cant use set_attitude_change to rise and fwd at the same time so we have to
                // check if we are roughly pointing at the target, and decide what to do
                if (abs(ang_x) < 15 && range < BUOY_RANGE_WHEN_DONE) {
                    done_buoy = true;
                    printf("\n\nBUOY TASK DONE\n\n\n");
		}
                /*else if (abs(depth_change) > 20 && range > 200) {
                    if (depth_change > 40) depth_change = 40;
                    if (depth_change < -40) depth_change = -40;
                    move(SINK, depth_change);
                }*/
                else if (abs(ang_x) <= 5) {
                    set(SPEED, 1);

                    // calculate an exponential moving average for range
                    //EMA_range = (EMA_range == -1) ? range : 0.1*range+0.9*EMA_range;
                    EMA_range = 0.5*range+0.5*EMA_range;
                    printf ("task_buoy: range = %d.  EMA_range = %d", range, EMA_range);
                    fflush(stdout);
                }
                else {
                    if (ang_x > 20) ang_x = 20;
                    if (ang_x < -20) ang_x = -20;
                    move(RIGHT, ang_x); 
                }
            }
            else {
                printf ("Error: %s: line %d\ntask module recieved an unhandled vision code.\n", __FILE__, __LINE__);
                exit(1);
            }
        }
        else { // done_buoy
            // charge forwards, then retreat back some number of meters, then realign sub to starting attitude
            printf("Ramming buoy\n");
            move(FORWARD, 5);

            // retreat backwards
            printf("Resetting Position\n");
            move(REVERSE, 5);

            stop();

            ret_code = TASK_DONE;
            break;
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
