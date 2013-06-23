#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_PATH:: MDA_TASK_PATH (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
    pix_x_old = pix_y_old = 0;
}

MDA_TASK_PATH:: ~MDA_TASK_PATH ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_PATH:: run_task() {
    puts("Press q to quit");


    MDA_VISION_MODULE_PATH path_vision;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    bool done_path = false;

    // sink to starting depth
    const int starting_depth = 300; 
    set(DEPTH, starting_depth); // this is rough depth of the buoys

    // clear webcam cache
    for (int i = 0; i < WEBCAM_CACHE; i++) {
      image_input->ready_image();
      image_input->ready_image(DWN_IMG);
    }

    while (1) {
        IplImage* frame = image_input->get_image(DWN_IMG);
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
                int pix_x = path_vision.get_pixel_x();
                int pix_y = path_vision.get_pixel_y();
                int xy_distance = sqrt(pow(pix_y,2) + pow(pix_x,2));

                printf("xy_distance = %d    xy_angle = %5.2f\n==============================\n", xy_distance, xy_ang);

                if(xy_distance > frame->height/5){
                    if (abs(xy_ang) < 10){
                        actuator_output->set_attitude_change(FORWARD);           
                    }
                    else {
                        move(RIGHT, xy_ang);
                        printf("Turning %s %d degrees (xy_ang)\n", (xy_ang > 0) ? "right" : "left", static_cast<int>(abs(xy_ang)));
                    }
                }
                else{
                    // TODO: set depth based on range, not a hard-coded value
                    actuator_output->set_attitude_absolute(DEPTH, DEPTH_TARGET);
                }
            }
            else if (vision_code == ONE_SEGMENT || vision_code == FULL_DETECT || vision_code == FULL_DETECT_PLUS 
                || vision_code == DOUBLE_DETECT || vision_code == TWO_SEGMENT) {
                // here we want to position over the path, then sink and reorient ourselves
                int pix_x = path_vision.get_pixel_x();
                int pix_y = path_vision.get_pixel_y();
                float xy_ang = 0;
                int pos_ang = 0;

                //If there are 2 fully defined paths, pick the one closest to the last path we followed
                if(vision_code != ONE_SEGMENT && vision_code != FULL_DETECT){
                    int pix_x_alt = path_vision.get_pixel_x_alt();
                    int pix_y_alt = path_vision.get_pixel_y_alt();
                
                    if(sqrt(pow(pix_y_alt - pix_y_old,2) + pow(pix_x_alt - pix_x_old,2)) < sqrt(pow(pix_y - pix_y_old,2) + pow(pix_x - pix_x_old,2))){
                        xy_ang = path_vision.get_angular_x_alt(); // this is its position equal to atan(x/y)
                        pos_ang = path_vision.get_angle_alt(); // this is the orientation of the path
                        pix_x = pix_x_alt;
                        pix_y = pix_y_alt;
                    }
                    else{
                        xy_ang = path_vision.get_angular_x(); // this is its position equal to atan(x/y)
                        pos_ang = path_vision.get_angle(); // this is the orientation of the path
                    }
                }
                else{
                    xy_ang = path_vision.get_angular_x(); // this is its position equal to atan(x/y)
                    pos_ang = path_vision.get_angle(); // this is the orientation of the path
                }
                int xy_distance = sqrt(pow(pix_y,2) + pow(pix_x,2));

                pix_x_old = pix_x;
                pix_y_old = pix_y;

                printf("xy_distance = %d    xy_angle = %5.2f\n==============================\n", xy_distance, xy_ang);

                if (xy_distance < frame->height/6) {
                    // if we are oriented over the path, we can sink
                    actuator_output->set_attitude_absolute(DEPTH, DEPTH_TARGET);

                    if(attitude_input->depth() > DEPTH_TARGET-5 && attitude_input->depth() < DEPTH_TARGET+5){
                        move(RIGHT,pos_ang);
                        printf("Turning %s %d (pos_ang) degrees\n", (pos_ang > 0) ? "right" : "left", abs(pos_ang));
                        if(abs(pos_ang) < 5){
                            done_path = true;
                            // settle for 2s
                            sleep(2);
                            break;
                        }
                    }
                } 
                else {
                    // if we are not oriented over the path, put the path in front of us and go fwd
                    if (abs(xy_ang) < 10){
                        actuator_output->set_attitude_change(FORWARD);
                    } 
                    else {
                        move(RIGHT, xy_ang);
                        printf("Turning %s %d (xy_ang2) degrees\n", (xy_ang > 0) ? "right" : "left", static_cast<int>(abs(xy_ang)));
                    }
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

    if(done_path){
        ret_code = TASK_DONE;
    }

    return ret_code;
}
