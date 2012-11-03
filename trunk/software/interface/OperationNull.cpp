#include "Operation.h"

void OperationNull::work()
{
  mvWindow win("window");

  for (;;) {
    const IplImage *frame = image_input->get_image(DWN_IMG);
    if (frame == NULL) {
      break;
    }

    win.showImage(frame);
    
    char c = cvWaitKey(30);
    if (c == 'q') {
      break;
    }
  }
}
