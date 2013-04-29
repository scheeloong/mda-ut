#include "mv.h"

// Uncomment to disable scratch images (more memory usage, but better for debugging)
//#define DISABLE_SCRATCH_IMAGE

static IplImage* scratch_img_1channel = NULL;
static int instances_1channel = 0;
static IplImage* scratch_img_1channel_2 = NULL;
static int instances_1channel_2 = 0;
static IplImage* scratch_img_1channel_3 = NULL;
static int instances_1channel_3 = 0;

static IplImage* scratch_img_3channel = NULL;
static int instances_3channel = 0;

IplImage* mvGetScratchImage() {
#ifndef DISABLE_SCRATCH_IMAGE
    if (instances_1channel == 0) {
#endif
        instances_1channel++;

        mvGetScratchImage_Color();
        scratch_img_1channel = mvCreateImage();

        return scratch_img_1channel;
#ifndef DISABLE_SCRATCH_IMAGE
    }
    else {
        instances_1channel++;
        scratch_img_1channel->imageData = scratch_img_3channel->imageData;
        return scratch_img_1channel;
    }
#endif
}

void mvReleaseScratchImage() {
#ifndef DISABLE_SCRATCH_IMAGE
    if (instances_1channel == 1) {
#endif
        mvReleaseScratchImage_Color();
        cvReleaseImageHeader(&scratch_img_1channel);
        instances_1channel = 0;
#ifndef DISABLE_SCRATCH_IMAGE
    }
    else if (instances_1channel >= 1) {
        instances_1channel--;
    }
    else {
        fprintf (stderr, "Tried to release 1 channel image #2 without getting it first\n");
        exit (1);
    }
#endif
}

IplImage* mvGetScratchImage2() {
#ifndef DISABLE_SCRATCH_IMAGE
    if (instances_1channel_2 == 0) {
#endif
        instances_1channel_2++;

        // For this one, we are going to use the middle 1/3 of the 3 channel image's data array
        mvGetScratchImage_Color();
        scratch_img_1channel_2 = cvCreateImageHeader(
            cvGetSize(scratch_img_3channel),
            IPL_DEPTH_8U,
            1
        );
        scratch_img_1channel_2->imageData = scratch_img_3channel->imageData + 
                                            scratch_img_1channel_2->height * scratch_img_1channel_2->widthStep;

        return scratch_img_1channel_2;
#ifndef DISABLE_SCRATCH_IMAGE
    }
    else {
        instances_1channel_2++;
        scratch_img_1channel_2->imageData = scratch_img_3channel->imageData + 
                                            scratch_img_1channel_2->height * scratch_img_1channel_2->widthStep;
        return scratch_img_1channel_2;
    }
#endif
}

void mvReleaseScratchImage2() {
#ifndef DISABLE_SCRATCH_IMAGE
    if (instances_1channel_2 == 1) {
#endif
        mvReleaseScratchImage_Color();
        cvReleaseImageHeader(&scratch_img_1channel_2);
        instances_1channel_2 = 0;
#ifndef DISABLE_SCRATCH_IMAGE
    }
    else if (instances_1channel_2 >= 1) {
        instances_1channel_2--;
    }
    else {
        fprintf (stderr, "Tried to release 1 channel image #2 without getting it first\n");
        exit (1);
    }
#endif
}

IplImage* mvGetScratchImage3() {
#ifndef DISABLE_SCRATCH_IMAGE
    if (instances_1channel_3 == 0) {
#endif
        instances_1channel_3++;

        // For this one, we are going to use the last 1/3 of the 3 channel image's data array
        mvGetScratchImage_Color();
        scratch_img_1channel_3 = cvCreateImageHeader(
            cvGetSize(scratch_img_3channel),
            IPL_DEPTH_8U,
            1
        );
        scratch_img_1channel_3->imageData = scratch_img_3channel->imageData + 
                                            2*scratch_img_1channel_3->height * scratch_img_1channel_3->widthStep;

        return scratch_img_1channel_3;
#ifndef DISABLE_SCRATCH_IMAGE
    }
    else {
        instances_1channel_3++;
        scratch_img_1channel_3->imageData = scratch_img_3channel->imageData + 
                                            2*scratch_img_1channel_3->height * scratch_img_1channel_3->widthStep;
        return scratch_img_1channel_3;
    }
#endif
}

void mvReleaseScratchImage3() {
#ifndef DISABLE_SCRATCH_IMAGE
    if (instances_1channel_3 == 1) {
#endif
        mvReleaseScratchImage_Color();
        cvReleaseImageHeader(&scratch_img_1channel_3);
        instances_1channel_3 = 0;
#ifndef DISABLE_SCRATCH_IMAGE
    }
    else if (instances_1channel_3 >= 1) {
        instances_1channel_3--;
    }
    else {
        fprintf (stderr, "Tried to release 1 channel image #3 without getting it first\n");
        exit (1);
    }
#endif
}

IplImage* mvGetScratchImage_Color() {
#ifndef DISABLE_SCRATCH_IMAGE
    if (instances_3channel == 0) {
        assert (scratch_img_3channel == NULL);
#endif
        instances_3channel++;
        scratch_img_3channel = mvCreateImage_Color();
        return scratch_img_3channel;
#ifndef DISABLE_SCRATCH_IMAGE
    }
    else {
        instances_3channel++;
        return scratch_img_3channel;
    }
#endif
}

void mvReleaseScratchImage_Color() {
#ifndef DISABLE_SCRATCH_IMAGE
    if (instances_3channel == 1) {
#endif
        cvReleaseImage (&scratch_img_3channel);
        instances_3channel = 0;
#ifndef DISABLE_SCRATCH_IMAGE
    }
    else if (instances_3channel > 1) {
        instances_3channel--;
    }
    else {
        fprintf (stderr, "Tried to release 1 channel image without getting it first\n");
        exit (1);
    }
#endif
}
