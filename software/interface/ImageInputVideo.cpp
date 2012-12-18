#include "ImageInput.h"

ImageInputVideo::ImageInputVideo(const char* settings_file) : ImageInput(settings_file)
{
    std::string video_fwd, video_dwn;
    read_mv_setting(settings_file, "VIDEO_FWD", video_fwd);
    read_mv_setting(settings_file, "VIDEO_DWN", video_dwn);

    cam_fwd = new mvCamera (video_fwd.c_str());
    cam_dwn = new mvCamera (video_dwn.c_str());

    printf ("Forward Video File: %s\n", video_fwd.c_str());
    printf ("Down Video File: %s\n", video_dwn.c_str());
}

ImageInputVideo::~ImageInputVideo()
{
    delete cam_fwd;
    delete cam_dwn;
}

int ImageInputVideo::ready_image (ImageDirection dir)
{
    if (dir == FWD_IMG) {
        return cam_fwd ? cam_fwd->grabFrame() : NULL;
    }
    if (dir == DWN_IMG) {
        return cam_dwn ? cam_dwn->grabFrame() : NULL;
    }
    return NULL;
}

const IplImage *ImageInputVideo::get_internal_image(ImageDirection dir)
{
    if (dir == FWD_IMG) {
        return cam_fwd ? cam_fwd->retrieveFrameResized() : NULL;
    }
    if (dir == DWN_IMG) {
        return cam_dwn ? cam_dwn->retrieveFrameResized() : NULL;
    }
    return NULL;
}
