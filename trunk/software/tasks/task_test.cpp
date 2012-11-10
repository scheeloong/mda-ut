#include "mda_tasks.h"
#include "mda_vision.h"

MDA_TASK_TEST:: MDA_TASK_TEST (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o),
    window ("task_test")
{
    vision_module = new MDA_VISION_MODULE_TEST ();
}

MDA_TASK_TEST:: ~MDA_TASK_TEST ()
{
    delete vision_module;
}

MDA_TASK_RETURN_CODE MDA_TASK_TEST:: run_task() {
    puts("Press q to quit");
    while (1) {
        const IplImage* frame = image_input->get_image(FWD_IMG);
        if (!frame) {
            break;
        }
        window.showImage (frame);

        // Ensure debug messages are printed
        fflush(stdout);
        // Exit if instructed to
        if (CharacterStreamSingleton::get_instance().get_next_char() == 'q')
            break;
        char c = cvWaitKey (10);
        if (c == 'q')
            break;
    }

    return TASK_DONE;
}
