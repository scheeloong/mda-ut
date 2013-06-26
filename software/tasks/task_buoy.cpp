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
    
    int starting_depth = 400;
/*
    char settings_file[50];
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
*/
    MDA_VISION_MODULE_BUOY buoy_vision;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    /// Here we should store the starting attitude vector, so we can return to this attitude later

    bool done_buoy = false;
    int EMA_range = 200;
    printf ("Sinking to appropriate buoy depth\n");
    set(DEPTH, starting_depth); // this is rough depth of the buoys

    // clear webcam cache
    for (int i = 0; i < WEBCAM_CACHE; i++) {
      image_input->ready_image();
      image_input->ready_image(DWN_IMG);
    }

    while (1) {
        IplImage* frame = image_input->get_image();
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }

        MDA_VISION_RETURN_CODE vision_code = buoy_vision.filter(frame);

        // NEW CODE ALERT!!!
        buoy_vision.add_frame(frame);
        if (buoy_vision.circle_stable()) printf("Circle stable!!!\n");
        // NEW CODE ALERT!!!

        // clear dwn image
        int down_frame_ready = image_input->ready_image(DWN_IMG);
        (void) down_frame_ready;

        if (!done_buoy) {
            if (vision_code == FATAL_ERROR) {
                ret_code = TASK_ERROR;
                break;
            }
            else if (vision_code == NO_TARGET) {
                //actuator_output->set_attitude_change(FORWARD);
            }
            else if (vision_code == UNKNOWN_TARGET) {
                // here range is not valid, probably cuz theres interference or circle too small
                // we'll ignore the vertical direction for now
                int ang_x = buoy_vision.get_angular_x();

                move(RIGHT, ang_x);
                //actuator_output->set_attitude_change(FORWARD);
            }
            else if (vision_code == FULL_DETECT) {
                // range is valid, so we can use it to calculate the right depth
                int ang_x = buoy_vision.get_angular_x();
                int ang_y = buoy_vision.get_angular_y();
                int range = buoy_vision.get_range();

                int depth_change = tan(ang_y*0.017453) * range; 
                //move(SINK, depth_change);

                // we cant use set_attitude_change to rise and fwd at the same time so we have to
                // check if we are roughly pointing at the target, and decide what to do
                if (abs(ang_x) < 5 && abs(ang_y) < 20) {
                    actuator_output->set_attitude_change(FORWARD);

                    // calculate an exponential moving average for range
                    //EMA_range = (EMA_range == -1) ? range : 0.1*range+0.9*EMA_range;
                    EMA_range = 0.5*range+0.5*EMA_range;
                    printf ("task_buoy: range = %d.  EMA_range = %d", range, EMA_range);

                    // more forgiving on EMA_range
                    if (EMA_range < 30) {
                        done_buoy = true;
			printf("\n\nBUOY TASK DONE\n\n\n");
                    }   
                }
                else {
                    if (ang_x > 20) ang_x = 20;
                    if (ang_x < -20) ang_x = -20;
                    printf("Turning %s %d degrees\n", (ang_x > 0) ? "right" : "left", abs(ang_x));
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
            printf ("Ramming buoy\n");
            actuator_output->set_attitude_change(FORWARD);
            sleep (1);

            // retreat backwards
            printf ("Resetting Position\n");
            actuator_output->set_attitude_change(REVERSE, 3);
            sleep (4);

            actuator_output->set_attitude_change(FORWARD,0);

            // clear stale webcam video cache
            for (int i = 0; i < WEBCAM_CACHE; i++) {
                image_input->ready_image();
                image_input->ready_image(DWN_IMG);
            }

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
