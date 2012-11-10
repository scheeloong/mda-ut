#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_TEST:: MDA_TASK_TEST (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o),
    window ("task_test")
{
}

MDA_TASK_TEST:: ~MDA_TASK_TEST ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_TEST:: run_task() {
    puts("Press q to quit");

    MDA_TASK_RETURN_CODE ret_code = TASK_MISSING;

    while (1) {
        const IplImage* frame = image_input->get_image(FWD_IMG);
        if (!frame) {
            break;
        }
        window.showImage (frame);
        
        /* INSERT YOUR CODE HERE */  

        // Ensure debug messages are printed
        fflush(stdout);
        // Exit if instructed to
        char c = cvWaitKey(1);
        if (c != -1) {
            CharacterStreamSingleton::get_instance().write_char(c);
        }
        if (CharacterStreamSingleton::get_instance().wait_key(1) == 'q'){
            ret_code = TASK_QUIT;
            break;
        }
    }

    return ret_code;
}
