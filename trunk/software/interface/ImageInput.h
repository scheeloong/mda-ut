/* ImageInput interface.

   This interface represents the eyes of the submarine. It can return a forward image
   or a below image.
*/

#ifndef IMAGE_INPUT_H
#define IMAGE_INPUT_H

#include <cv.h>
#include <highgui.h>
#include <sstream>
#include "mgui.h"

enum ImageDirection {
  FWD_IMG,
  DWN_IMG
};

/* Image Input interface */
class ImageInput {
  public:
    virtual ~ImageInput() {}

    virtual const IplImage* get_image(ImageDirection dir = FWD_IMG) = 0;
    virtual void dump_images()
    {
      static int count = 0;

      const IplImage *img_fwd = get_image();
      if (img_fwd) {
        std::ostringstream oss;
        oss << "image_fwd_" << count << ".jpg";
        cvSaveImage (oss.str().c_str(), img_fwd);
      }

      const IplImage *img_dwn = get_image(DWN_IMG);
      if (img_dwn) {
        std::ostringstream oss;
        oss << "image_dwn_" << count << ".jpg";
        cvSaveImage (oss.str().c_str(), img_dwn);
      }

      count++;
    }
};

/* A don't care implementation */
class ImageInputNull : public ImageInput {
  public:
    virtual ~ImageInputNull() {}

    virtual const IplImage* get_image(ImageDirection dir = FWD_IMG) {return NULL;}
};

/* Simulator implementation */
class ImageInputSimulator : public ImageInput {
  public:
    ImageInputSimulator();
    virtual ~ImageInputSimulator();

    virtual const IplImage* get_image(ImageDirection dir = FWD_IMG);
};

/* Read from video file */
class ImageInputVideo : public ImageInput {
  mvCamera* cam_fwd;
  mvCamera* cam_dwn;

  public:
    ImageInputVideo(const char* settings_file);
    virtual ~ImageInputVideo();

    virtual const IplImage* get_image(ImageDirection dir = FWD_IMG);
};

/* Read from webcam */
class ImageInputWebcam : public ImageInput {
  public:
    ImageInputWebcam(const char* settings_file);
    virtual ~ImageInputWebcam();

    virtual const IplImage* get_image(ImageDirection dir = FWD_IMG);

  private:
    mvCamera *fwdCam;
    mvCamera *dwnCam;
};

#endif
