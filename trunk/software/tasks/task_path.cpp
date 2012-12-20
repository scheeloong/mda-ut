#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_PATH:: MDA_TASK_PATH (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
}

MDA_TASK_PATH:: ~MDA_TASK_PATH ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_PATH:: run_task() {
    puts("Press q to quit");


    MDA_VISION_MODULE_PATH path_vision;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    bool done_path = false;

    while (1) {
        const IplImage* frame = image_input->get_image(DWN_IMG);
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        MDA_VISION_RETURN_CODE vision_code = path_vision.filter(frame);

        // clear fwd image
        int fwd_frame_ready = image_input->ready_image(FWD_IMG);
        (void) fwd_frame_ready;

        if (!done_path) {            
            if (vision_code == NO_TARGET) {
                actuator_output->set_attitude_change(FORWARD);           
            }
            else if (vision_code == UNKNOWN_TARGET) {
                // the goal here is to try to place the path right in front of us
                float xy_ang = path_vision.get_angular_x(); // this is its position equal to atan(x/y)
                if (abs(xy_ang) < 10){
                    actuator_output->set_attitude_change(FORWARD);           
                }
                else {
                    actuator_output->set_attitude_change(LEFT, xy_ang);
                }
            }
            else if (vision_code == ONE_SEGMENT || vision_code == FULL_DETECT) {
                // here we want to position over the path, then sink and reorient ourselves
                int pix_x = path_vision.get_pixel_x();
                int pix_y = path_vision.get_pixel_y();
                float xy_ang = path_vision.get_angular_x(); // this is its position equal to atan(x/y)
                int pos_ang = path_vision.get_angle(); // this is the orientation of the path
                int xy_distance = sqrt(pow(pix_y,2) + pow(pix_x,2));

                printf("xy_distance = %d\n", xy_distance);

                if (xy_distance < frame->height/4) {
                    // if we are oriented over the path, we can sink
                    actuator_output->set_attitude_change(FORWARD, 0);
                    actuator_output->set_attitude_absolute(DEPTH, DEPTH_TARGET);

                    if(attitude_input->depth() > DEPTH_TARGET-5 && attitude_input->depth() < DEPTH_TARGET+5){
                        actuator_output->set_attitude_change(LEFT,pos_ang);
                        if(abs(pos_ang) < 5){
                            done_path = true;
                            // settle for 2s
                            actuator_output->set_attitude_absolute(DEPTH, 350);
                            sleep (2);
                            break;
                        }
                    }
                } 
                else {
                    // if we are not oriented over the path, put the path in front of us and go fwd
                    if (abs(xy_ang) < 5){
                        actuator_output->set_attitude_change(SINK, 0);
                        actuator_output->set_attitude_change(FORWARD);
                    } 
                    else {
                        actuator_output->set_attitude_change(LEFT, xy_ang);
                    }
                }
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
            actuator_output->stop();
            ret_code = TASK_QUIT;
            break;
        }
    }

    if(done_path){
        ret_code = TASK_DONE;
    }

    return ret_code;
}
