/* ImageInput interface.

   This interface represents the eyes of the submarine. It can return a forward image
   or a below image.
*/

#ifndef IMAGE_INPUT_H
#define IMAGE_INPUT_H

#include <cv.h>

enum ImageDirection {
  FWD_IMG,
  DWN_IMG
};

/* Image Input interface */
class ImageInput {
  public:
    virtual ~ImageInput() {}

    // some method to return an image (to be determined)
    virtual const IplImage* get_image(ImageDirection dir = FWD_IMG) = 0;
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
  public:
    ImageInputVideo();
    virtual ~ImageInputVideo();

    virtual const IplImage* get_image(ImageDirection dir = FWD_IMG);
};

/* The real submarine implementation */
class ImageInputSubmarine : public ImageInput {
  public:
    ImageInputSubmarine();
    virtual ~ImageInputSubmarine();

    virtual const IplImage* get_image(ImageDirection dir = FWD_IMG);
};

#endif
