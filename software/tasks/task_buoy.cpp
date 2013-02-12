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
    MDA_TASK_RETURN_CODE code;

    code = run_single_buoy(BUOY_YELLOW);
    if (code != TASK_DONE) {
        printf ("Something wrong in task_buoy!\n");
        return TASK_ERROR;
    }

    code = run_single_buoy(BUOY_RED);
    if (code != TASK_DONE) {
        printf ("Something wrong in task_buoy!\n");
        return TASK_ERROR;
    }
    
    code = run_single_buoy(BUOY_GREEN);
     if (code != TASK_DONE) {
        printf ("Something wrong in task_buoy!\n");
        return TASK_ERROR;
    }

    return TASK_DONE;
}

MDA_TASK_RETURN_CODE MDA_TASK_BUOY:: run_single_buoy(BUOY_COLOR color) {
    puts("Press q to quit");

    char settings_file[50];
    int starting_depth = 0;
    if (color == BUOY_RED) {
        sprintf (settings_file, "vision_buoy_settings_red.csv");
        starting_depth = 375;
    }
    else if (color == BUOY_YELLOW) {
        sprintf (settings_file, "vision_buoy_settings_yellow.csv");
        starting_depth = 400;
    }
    else if (color == BUOY_GREEN) {
        sprintf (settings_file, "vision_buoy_settings_green.csv");
        starting_depth = 475;
    }
    else 
        exit (1);

    MDA_VISION_MODULE_BUOY buoy_vision(settings_file);
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    /// Here we should store the starting attitude vector, so we can return to this attitude later

    bool done_buoy = false;
    int EMA_range = -1;
    printf ("Sinking to appropriate buoy depth\n");
    actuator_output->set_attitude_absolute(DEPTH, starting_depth); // this is rough depth of the buoys
    sleep(2);

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
                actuator_output->set_attitude_change(FORWARD);
            }
            else if (vision_code == UNKNOWN_TARGET) {
                // here range is not valid, probably cuz theres interference or circle too small
                // we'll ignore the vertical direction for now
                int ang_x = buoy_vision.get_angular_x();

                actuator_output->set_attitude_change(RIGHT, ang_x);
                actuator_output->set_attitude_change(FORWARD);
            }
            else if (vision_code == FULL_DETECT) {
                // range is valid, so we can use it to calculate the right depth
                int ang_x = buoy_vision.get_angular_x();
                int ang_y = buoy_vision.get_angular_y();
                int range = buoy_vision.get_range();

                int depth_change = tan(ang_y*0.017453) * range; 
                actuator_output->set_attitude_change(SINK, depth_change);

                // we cant use set_attitude_change to rise and fwd at the same time so we have to
                // check if we are roughly pointing at the target, and decide what to do
                if (abs(ang_x) < 5 && abs(ang_y) < 20) {
                    actuator_output->set_attitude_change(FORWARD);

                    // calculate an exponential moving average for range
                    EMA_range = (EMA_range == -1) ? range : 0.1*range+0.9*EMA_range;

                    // more forgiving on EMA_range
                    if (EMA_range < BUOY_RANGE_WHEN_DONE * 1.33) {
                        done_buoy = true;
                    }   
                }
                else {
                    actuator_output->set_attitude_change(RIGHT, ang_x);              
                }
            }
            else {
                printf ("Error: %s: line %d\ntask module recieved an unhandled vision code.\n", __FILE__, __LINE__);
                exit(1);
            }
        }
        else { // done_buoy
            // charge forwards, then retreat back some number of meters, then realign sub to starting attitude
            printf ("Ramming buoy\n");
            actuator_output->set_attitude_change(FORWARD);
            sleep (1);

            // retreat backwards
            printf ("Resetting Position\n");
            actuator_output->set_attitude_change(REVERSE, 3);
            sleep (4);

            actuator_output->set_attitude_change(FORWARD,0);
            ret_code = TASK_DONE;
            break;
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
