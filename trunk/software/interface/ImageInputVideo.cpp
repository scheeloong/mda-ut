#include "ImageInput.h"

ImageInputVideo::ImageInputVideo(const char* video_fwd, const char* video_dwn) :
	cam_fwd (video_fwd),
	cam_dwn (video_dwn)
{
    printf ("Foward Video File: %s\n", video_fwd);
    printf ("Down Video File: %s\n", video_dwn);
}

ImageInputVideo::~ImageInputVideo()
{
}

const IplImage *ImageInputVideo::get_image(ImageDirection dir)
{
    // we need to grab both frames, but only return the relevant one
    cam_fwd.grabFrame();
    cam_dwn.grabFrame();

    if (dir == FWD_IMG)
        return cam_fwd.retrieveFrame();
    else
        return cam_dwn.retrieveFrame();

    return NULL;
}
