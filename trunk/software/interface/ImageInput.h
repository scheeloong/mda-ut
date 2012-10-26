/* ImageInput interface.

   This interface represents the eyes of the submarine. It can return a forward image
   or a below image.
*/

#ifndef IMAGE_INPUT_H
#define IMAGE_INPUT_H

/* Image Input interface */
class ImageInput {
  public:
    virtual ~ImageInput() {}

    // some method to return an image (to be determined)
};

/* A don't care implementation */
class ImageInputNull : public ImageInput {
  public:
    virtual ~ImageInputNull() {}
};

/* Simulator implementation */
class ImageInputSimulator : public ImageInput {
  public:
    ImageInputSimulator();
    virtual ~ImageInputSimulator();
};

/* The real submarine implementation */
class ImageInputSubmarine : public ImageInput {
  public:
    ImageInputSubmarine();
    virtual ~ImageInputSubmarine();
};

#endif
