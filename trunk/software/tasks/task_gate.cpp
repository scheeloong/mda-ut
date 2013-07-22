#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_GATE:: MDA_TASK_GATE (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
}

MDA_TASK_GATE:: ~MDA_TASK_GATE ()
{
}

enum TASK_STATE {
    STARTING,
    STOPPED,
    PANNING,
};

MDA_TASK_RETURN_CODE MDA_TASK_GATE:: run_task() {
    puts("Press q to quit");

    MDA_VISION_MODULE_GATE gate_vision;
    TASK_STATE state = STARTING;

    bool done_gate = false;
    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    MDA_TASK_BASE::starting_depth = attitude_input->depth();

    // gate depth
    set (DEPTH, starting_depth+50);

    // read the starting orientation
    int starting_yaw = attitude_input->yaw();
    printf("Starting yaw: %d\n", starting_yaw);

    static TIMER timer; // keeps track of time spent in each state
    static TIMER master_timer; // keeps track of time spent not having found the target
    timer.restart();
    master_timer.restart();

    while (1) {
        IplImage* frame = image_input->get_image();
        if (!frame) {
            ret_code = TASK_ERROR;
            break;
        }
        MDA_VISION_RETURN_CODE vision_code = gate_vision.filter(frame);

        // clear dwn image. RZ - do we need this?
        // This ensures the other camera is properly logged
        // and that the webcam cache is cleared so it stays in sync - VZ
        image_input->ready_image(DWN_IMG);

        /**
        * Basic Algorithm: (repeat)
        *  - Go straight foward in STARTING state for 2 seconds. Then stop and stare for 1 second.
        *    - If we saw a ONE_SEGMENT, calculate the course we should take to allow the segment to remain in view.
        *    - If we saw a FULL_DETECT, change course to face it
        *  - Always go forwards in increments and stop for 1 seconds to stare each time.
        */
        if (!done_gate) {
            if (state == STARTING) {
                printf ("Starting: Moving Foward for 1 meter\n");
                move (FORWARD, 1);

                if (timer.get_time() > 1) {
                    set (SPEED, 0);
                    timer.restart();
                    gate_vision.clear_frames();
                    state = STOPPED;
                }
            }
            if (state == STOPPED) {
                // if havent spent 1 second in this state, keep staring
                if (timer.get_time() < 1) {
                    printf ("Stopped: Collecting Frames\n");
                }
                else {
                    if (vision_code == NO_TARGET) {
                        printf ("Stopped: No target\n");
                        if (master_timer.get_time() > 60) { // we've seen nothing for 60 seconds
                            printf ("Master Timer Timeout!!\n");
                            return TASK_MISSING;
                        }
                        if (timer.get_time() > 3) {
                            printf ("Stopped: Timeout\n");
                            timer.restart();
                            state = STARTING;
                        }
                    }
                    else if (vision_code == ONE_SEGMENT) {
                        printf ("Stopped: One Segment\n");
                        int ang_x = gate_vision.get_angular_x();

                        // only execute turn if the segment is close to out of view (check ang and range)
                        if (ang_x >= 35) {
                            ang_x -= 35;
                            printf ("Moving Left on One Segment %d Degrees\n", ang_x);
                            move (RIGHT, ang_x);
                        }
                        else if (ang_x <= -35) {
                            ang_x += 35;
                            printf ("Moving Left on One Segment %d Degrees\n", ang_x);
                            move (RIGHT, ang_x);
                        }

                        timer.restart();
                        master_timer.restart();
                        state = STARTING;
                    }
                    else if (vision_code == FULL_DETECT) {
                        printf ("Stopped: Full Detect\n");
                        int ang_x = gate_vision.get_angular_x();
                        move (RIGHT, ang_x);

                        if (gate_vision.get_range() < 300) {
                            done_gate = true;
                            return TASK_DONE;
                        }

                        timer.restart();
                        master_timer.restart();
                        state = STARTING;
                    }
                    else {
                        printf ("Error: %s: line %d\ntask module recieved an unhandled vision code.\n", __FILE__, __LINE__);
                        exit(1);
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

    return ret_code;
}
