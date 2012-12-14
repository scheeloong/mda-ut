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
        image_input->get_image();

        if (!done_path){
            int pos_ang = path_vision.get_angle();
            
            if (vision_code == NO_TARGET){
                actuator_output->set_attitude_change(FORWARD);           
            }
            else if (vision_code == UNKNOWN_TARGET){

                if (abs(pos_ang) < 5){
                    actuator_output->set_attitude_change(FORWARD);           
                }else{
                    actuator_output->set_attitude_change(LEFT, pos_ang);
                }
            }
            else if (vision_code == ONE_SEGMENT || vision_code == FULL_DETECT) {
                int pix_x = path_vision.get_pixel_x();
                int pix_y = path_vision.get_pixel_y();
                int distance = sqrt(pow(pix_y,2) + pow(pix_x,2));

                printf("distance = %d\n", distance);

                if (distance < frame->height/2){
                    actuator_output->set_attitude_absolute(DEPTH, 585);

                    if(attitude_input->depth() > 570 && attitude_input->depth() < 600){
                        actuator_output->set_attitude_change(LEFT,pos_ang);
                        if(abs(pos_ang) < 5){
                            done_path = true;
                            break;
                        }
                    }
                }else{
                    if (abs(pos_ang) < 5){
                        actuator_output->set_attitude_change(FORWARD);
                    }else{
                        actuator_output->set_attitude_change(LEFT, pos_ang);
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
