#include <stdlib.h>
#include <stdio.h>

#include "mda_vision.h"
#include "mda_control.h"
#include "vci.h"

// This is just a program to test proper linking of code in task

#define IMAGE_NAME "../vision/color1.jpg"

int main (int argc, char** argv) {
    IplImage * img = cvLoadImage(IMAGE_NAME);
    VCI vci;

    MDA_VISION_MODULE_BASE* test_module = NULL;
    MDA_VISION_MODULE_BASE* gate_module = NULL;
    MDA_VISION_MODULE_BASE* path_module = NULL;
    MDA_VISION_MODULE_BASE* buoy_module = NULL;
    MDA_VISION_MODULE_BASE* frame_module = NULL;
    
    test_module = new MDA_VISION_MODULE_TEST;
    gate_module = new MDA_VISION_MODULE_GATE;
    path_module = new MDA_VISION_MODULE_PATH;
    buoy_module = new MDA_VISION_MODULE_BUOY;
    frame_module = new MDA_VISION_MODULE_FRAME;

    test_module->filter (img, &vci);
    gate_module->filter (img, &vci);
    path_module->filter (img, &vci);
    buoy_module->filter (img, &vci);
    frame_module->filter (img, &vci);

    delete test_module;
    delete gate_module;
    delete path_module;
    delete buoy_module;
    delete frame_module;

    printf ("VCI_UNDEFINED_VALUE=%d\n", VCI::VCI_UNDEFINED_VALUE);
    printf ("\nTest PASSED.\n");
    return 0;
}
