#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_GATE:: MDA_TASK_GATE (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o),
    window ("task_gate")
{
    // vision_module = new MDA_VISION_MODULE_TEST ();
}

MDA_TASK_GATE:: ~MDA_TASK_GATE ()
{
    delete vision_module;
}

MDA_TASK_RETURN_CODE MDA_TASK_GATE:: run_task() {
    puts("Press q to quit");
    vision_module = new MDA_VISION_MODULE_GATE();
    while (1) {
        const IplImage* frame = image_input->get_image(FWD_IMG);
        if (!frame) {
            break;
        }
        window.showImage (frame);

        vision_module->filter(frame);

        int x = vision_module->get_pixel_x();
        int y = vision_module->get_pixel_y();
        int z = vision_module->get_range();

        printf("range = %d\n",z);

        if (z < 400){
            actuator_output->set_attitude_change(FORWARD,5);
        }else{
            if (x == MDA_VISION_MODULE_BASE::VISION_UNDEFINED_VALUE){
                actuator_output->set_attitude_change(FORWARD,5);
            }else if (x < -frame->width/5){
                actuator_output->set_attitude_change(LEFT,5);
            }else if (x > frame->width/5){
                actuator_output->set_attitude_change(RIGHT,5);
            }else{
                actuator_output->set_attitude_change(FORWARD,5);
            }
        }


        // Ensure debug messages are printed
        fflush(stdout);
        // Exit if instructed to
        char c = cvWaitKey(1);
        if (c != -1) {
            CharacterStreamSingleton::get_instance().write_char(c);
        }
        if (CharacterStreamSingleton::get_instance().wait_key(1) == 'q'){
            actuator_output->stop();
            break;
        }
    }

    return TASK_DONE;
}
