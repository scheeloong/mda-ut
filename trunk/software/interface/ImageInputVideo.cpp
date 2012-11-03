#include "ImageInput.h"

ImageInputVideo::ImageInputVideo(const char* settings_file)
{
    std::string video_fwd, video_dwn;
    read_mv_setting(settings_file, "VIDEO_FWD", video_fwd);
    read_mv_setting(settings_file, "VIDEO_DWN", video_dwn);

    cam_fwd = new mvCamera (video_fwd.c_str());
    cam_dwn = new mvCamera (video_dwn.c_str());

    printf ("Foward Video File: %s\n", video_fwd.c_str());
    printf ("Down Video File: %s\n", video_dwn.c_str());
}

ImageInputVideo::~ImageInputVideo()
{
    delete cam_fwd;
    delete cam_dwn;
}

const IplImage *ImageInputVideo::get_image(ImageDirection dir)
{
    // we need to grab both frames, but only return the relevant one
    int have_fwd_frame = cam_fwd->grabFrame();
    int have_dwn_frame = cam_dwn->grabFrame();

    if (dir == FWD_IMG && have_fwd_frame) 
        return cam_fwd->retrieveFrame();
    else if (have_dwn_frame)
        return cam_dwn->retrieveFrame();

    return NULL;
}
