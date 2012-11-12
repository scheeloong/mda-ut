#include "mv.h"

static IplImage* scratch_img_1channel = NULL;
static int instances_1channel = 0;
static IplImage* scratch_img_3channel = NULL;
static int instances_3channel = 0;

IplImage* mvGetScratchImage() {
    if (instances_1channel == 0) {
        assert (scratch_img_1channel == NULL);
        instances_1channel++;
        scratch_img_1channel = mvCreateImage();
        return scratch_img_1channel;
    }
    else {
        instances_1channel++;
        return scratch_img_1channel;
    }
}

void mvReleaseScratchImage() {
    if (instances_1channel == 1) {
        printf ("Released 1channel Scratch Image\n");
        cvReleaseImage (&scratch_img_1channel);
        instances_1channel = 0;
    }
    else if (instances_1channel > 1) {
        instances_1channel--;
    }
    else {
        fprintf (stderr, "Tried to release 1 channel image without getting it first\n");
        exit (1);
    }
}

IplImage* mvGetScratchImage_Color() {
    if (instances_3channel == 0) {
        assert (scratch_img_3channel == NULL);
        instances_3channel++;
        scratch_img_3channel = mvCreateImage_Color();
        return scratch_img_3channel;
    }
    else {
        instances_3channel++;
        return scratch_img_3channel;
    }
}

void mvReleaseScratchImage_Color() {
    if (instances_3channel == 1) {
        printf ("Released 3channel Scratch Image\n");
        cvReleaseImage (&scratch_img_3channel);
        instances_3channel = 0;
    }
    else if (instances_3channel > 1) {
        instances_3channel--;
    }
    else {
        fprintf (stderr, "Tried to release 1 channel image without getting it first\n");
        exit (1);
    }
}