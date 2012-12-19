#include "ImageInput.h"

ImageInputWebcam::ImageInputWebcam(const char* settings_file) : ImageInput(settings_file), fwdCam(NULL), dwnCam(NULL)
{
   int webcam_fwd, webcam_dwn;
   read_mv_setting(settings_file, "WEBCAM_FWD", webcam_fwd);
   read_mv_setting(settings_file, "WEBCAM_DWN", webcam_dwn);

   fwdCam = new mvCamera(webcam_fwd);
   if (webcam_dwn != webcam_fwd) {
      dwnCam = new mvCamera(webcam_dwn);
   }
}

ImageInputWebcam::~ImageInputWebcam()
{
   delete fwdCam;
   delete dwnCam;
}

bool ImageInputWebcam::ready_internal_image (ImageDirection dir)
{
    if (dir == FWD_IMG) {
        return fwdCam ? fwdCam->grabFrame() : false;
    }
    if (dir == DWN_IMG) {
        return dwnCam ? dwnCam->grabFrame() : false;
    }
    return false;
}

const IplImage *ImageInputWebcam::get_internal_image(ImageDirection dir)
{
   if (dir == FWD_IMG) {
      return fwdCam ? fwdCam->retrieveFrameResized() : NULL;
   }
   if (dir == DWN_IMG) {
      return dwnCam ? dwnCam->retrieveFrameResized() : NULL;
   }
   return NULL;
}
