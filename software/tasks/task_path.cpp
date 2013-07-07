#include "mda_tasks.h"
#include "mda_vision.h"

enum TASK_STATE {
    STARTING,
    AT_SEARCH_DEPTH,
    AT_ALIGN_DEPTH
};

MDA_TASK_PATH:: MDA_TASK_PATH (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
    pix_x_old = pix_y_old = 0;
}

MDA_TASK_PATH:: ~MDA_TASK_PATH ()
{
}

static TASK_STATE state = STARTING;

MDA_TASK_RETURN_CODE MDA_TASK_PATH:: run_task() {
    puts("Press q to quit");

    MDA_VISION_MODULE_PATH path_vision;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;
    TIMER timer;

    bool done_path = false;

    // sink to starting depth
    const int SEARCH_DEPTH = 300;
    const int DEPTH_TO_SINK = 200;
    set(DEPTH, SEARCH_DEPTH);

    // read the starting orientation
    int starting_yaw = attitude_input->yaw();
    printf("Starting yaw: %d\n", starting_yaw);

    while (1) {
        IplImage* frame = image_input->get_image(DWN_IMG);
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        MDA_VISION_RETURN_CODE vision_code = path_vision.filter(frame);

        // clear fwd image
        /*int fwd_frame_ready = image_input->ready_image(FWD_IMG);
        (void) fwd_frame_ready;
        */

        /**
        * Basic Algorithm:
        *  - Remember the starting attitude
        *  - Define 2 depths, search_depth and align_depth, for searching and aligning.
        *  - Start at search_depth
        *  - If can't see anything at search_depth, go to starting attitude and go foward
        *  - If see something at search_depth, center self
        *  - If centered, sink to align depth
        *  - If see something at align depth, align
        *  - If don't see anything at align depth, go back to search_depth
        */



        if (!done_path) {
            // calculate some values that we will need
            float xy_ang = path_vision.get_angular_x(); // angle equal to atan(x/y)
            float pos_angle = path_vision.get_angle();  // PA, equal to orientation of the thing
            int pix_x = path_vision.get_pixel_x();
            int pix_y = path_vision.get_pixel_y();
            int pix_distance = sqrt(pow(pix_y,2) + pow(pix_x,2));

            printf("xy_distance = %d    xy_angle = %5.2f\n==============================\n", pix_distance, pos_angle);



            if (state == STARTING) {
                if (vision_code == NO_TARGET) {
                    printf ("Starting: No target\n");
                    set(SPEED,1);
                    if (timer.get_time() > 120) { // timeout
                        printf ("Timeout\n");
                        return TASK_MISSING;
                    }
                }
                else if (vision_code == UNKNOWN_TARGET) {
                    printf ("Starting: Unknown target\n");
                    timer.restart();
                }
                else {
                    printf ("Starting: Good\n");
                    stop();
                    timer.restart();
                    state = AT_SEARCH_DEPTH;
                }
            }
            else if (state == AT_SEARCH_DEPTH){
                if (vision_code == NO_TARGET) {
                    printf ("Searching: No target\n");
                    if (timer.get_time() > 10) { // timeout, go back to starting state
                        printf ("Timeout\n");
                        // set starting attitude!
                        stop();
                        timer.restart();
                        state = STARTING;
                    }
                    else if (timer.get_time() % 2 == 0) { // spin around a bit to try to re-aquire?
                        //move (RIGHT, 45);
                    }
                    else { // just wait
                    }
                }
                else if (vision_code == UNKNOWN_TARGET) {
                    printf ("Searching: Unknown target\n");
                    timer.restart();
                }
                else {
                    timer.restart();
                    printf ("Searching: Good\n");
                    if(pix_distance > frame->height/5){ // move over the path
                        if (abs(xy_ang) < 10) {
                            printf ("Set speed foward\n");
                            set(SPEED,1);
                        }
                        else {
                            printf("Turning %s %d degrees (xy_ang)\n", (xy_ang > 0) ? "Right" : "Left", static_cast<int>(abs(xy_ang)));
                            stop();
                            move(RIGHT, xy_ang);
                        }
                    }
                    else {                              // we are over the path, sink and try align state
                        stop();
                        move(SINK, DEPTH_TO_SINK);
                        timer.restart();
                        state = AT_ALIGN_DEPTH;
                    }
                }   
            }
            else if (state == AT_ALIGN_DEPTH) {
                if (vision_code == NO_TARGET) {     // wait for timeout
                    printf ("Aligning: No target\n");
                    if (timer.get_time() > 6) { // timeout
                        printf ("Timeout\n");
                        move(RISE, DEPTH_TO_SINK);
                        timer.restart();
                        state = AT_SEARCH_DEPTH;
                    }
                }
                else if (vision_code == UNKNOWN_TARGET) {
                    printf ("Aligning: Unknown target\n");
                    timer.restart();
                }
                else {
                    if (abs(pos_angle) >= 10) {
                        move(RIGHT, pos_angle);
                        timer.restart();
                    }
                    else {
                        done_path = true;
                        break;
                        timer.restart();
                    }
                }
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
