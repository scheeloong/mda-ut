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
    ImageInput(const char *settings_file, bool _can_display_images = true) : writer_fwd(NULL), writer_dwn(NULL), window(NULL), can_display_images(_can_display_images)
    {
      if (!settings_file) {
        return;
      }

      if (can_display_images) {
        bool continuous_display;
        read_mv_setting(settings_file, "IMAGE_CONTINUOUS_DISPLAY", continuous_display);
        if (continuous_display) {
          window = new mvWindow("Image Stream");
        }
      }

      // write to video?
      bool write_to_video;
      read_mv_setting(settings_file, "WRITE_TO_VIDEO", write_to_video);
      if (write_to_video) {
        std::string video_file;

        read_mv_setting(settings_file, "FWD_VIDEO_FILE", video_file);
        writer_fwd = new mvVideoWriter(video_file.c_str());
        read_mv_setting(settings_file, "DWN_VIDEO_FILE", video_file);
        writer_dwn = new mvVideoWriter(video_file.c_str());
      }
    }
    virtual ~ImageInput()
    {
      delete writer_fwd;
      delete writer_dwn;
      delete window;
    }

    virtual const IplImage* get_internal_image(ImageDirection dir = FWD_IMG) = 0;
    const IplImage* get_image(ImageDirection dir = FWD_IMG)
    {
      const IplImage *frame = get_internal_image(dir);
      if (!frame) {
        return frame;
      }

      if (window) {
        window->showImage(frame);
      }
      if (writer_fwd && dir == FWD_IMG) {
        writer_fwd->writeFrame(frame);
      }
      if (writer_dwn && dir == DWN_IMG) {
        writer_dwn->writeFrame(frame);
      }
      return frame;
    }

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

  protected:
    mvVideoWriter *writer_fwd, *writer_dwn;
    mvWindow *window;
    bool can_display_images;
};

/* A don't care implementation */
class ImageInputNull : public ImageInput {
  public:
    ImageInputNull() : ImageInput(NULL, false) {}
    virtual ~ImageInputNull() {}

    virtual const IplImage* get_internal_image(ImageDirection dir = FWD_IMG) {return NULL;}
};

/* Simulator implementation */
class ImageInputSimulator : public ImageInput {
  public:
    ImageInputSimulator(const char* settings_file);
    virtual ~ImageInputSimulator();

    virtual const IplImage* get_internal_image(ImageDirection dir = FWD_IMG);
};

/* Read from video file */
class ImageInputVideo : public ImageInput {
  public:
    ImageInputVideo(const char* settings_file);
    virtual ~ImageInputVideo();

    virtual const IplImage* get_internal_image(ImageDirection dir = FWD_IMG);

  private:
    mvCamera* cam_fwd;
    mvCamera* cam_dwn;
};

/* Read from webcam */
class ImageInputWebcam : public ImageInput {
  public:
    ImageInputWebcam(const char* settings_file);
    virtual ~ImageInputWebcam();

    virtual const IplImage* get_internal_image(ImageDirection dir = FWD_IMG);

  private:
    mvCamera *fwdCam;
    mvCamera *dwnCam;
};

#endif
