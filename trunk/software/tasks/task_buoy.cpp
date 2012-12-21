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
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    /// Here we should store the starting attitude vector, so we can return to this attitude later

    bool done_buoy = false;
    actuator_output->set_attitude_absolute(DEPTH, 375); // this is rough depth of the buoys

    while (1) {
        const IplImage* frame = image_input->get_image();
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
                }
                else {
                    actuator_output->set_attitude_change(RIGHT, ang_x);
                    if (range < BUOY_RANGE_WHEN_DONE) {
                        done_buoy = true;
                    }                 
                }
            }
            else {
                printf ("Error: %s: line %d\ntask module recieved an unhandled vision code.\n", __FILE__, __LINE__);
                exit(1);
            }
        }
        else { // done_buoy
            // charge forwards, then retreat back some number of meters, then realign sub to starting attitude
            actuator_output->set_attitude_change(FORWARD);
            sleep (2);

            // just charge fowards for now
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
