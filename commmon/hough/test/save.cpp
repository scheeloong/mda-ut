#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "../detection.h"
#include <qimage.h>
#include "../hough.h"

// need to unpack to 32bits/pixel
void save_image(char* name, 
		unsigned char* img, int width, int height, int binary,
		unsigned char* overlayed)
{
#if QT4
  QImage zz(width, height, QImage::Format_RGB32);
#else
  QImage zz(width, height, 32); 
#endif
  unsigned char* dest = zz.bits();

  if(binary)
    {
      float scaling = 200;
      int max=0;
      if(binary ==2)
	{
	  for(int i=0; i<(height*width); i++)
	    if(img[i]>max) max = img[i];
	  
	  //printf("max is %d\n", max);
	  scaling = 250.0/max;
	}
      scaling *= 5;

      //printf("scaling %f\n",  scaling);
      
      for(int i=0; i<(height*width); i++)
	{
	  unsigned char blue;
	  float val;
	  if(binary==1)
	    val = (img[i]>0)*scaling;
	  else {
	    val = img[i]*scaling;
	    if(val > 255) val = 255;
	  }

	 /* if(i==0)
	    for(int k=0; k<16; k++)
	      printf("%x\n",  img[k]);*/

#define ZERO 0x00

	  blue = (unsigned char)((overlayed!=NULL)?(overlayed[i]>0)*200:0);

	  if(!(blue == 0 && val == 0)) {
	    dest[4*i] = (unsigned char)val; // blue
	    dest[4*i+1] = (overlayed!=NULL)?blue:ZERO; // green
	    dest[4*i+2] = ZERO; // red
	    dest[4*i+3] = 0; // alpha
	  }
	  else {  // put a white background
	    dest[4*i] = ZERO;
	    dest[4*i+1] = ZERO; // green
	    dest[4*i+2] = ZERO; // red
	    dest[4*i+3] = 0; // alpha
	  }
	}
    }
  else
    {
      int p = 0;
      for(int i=0; i<(3*height*width); i+=3)
	{
	 /* if(i==0 || i==3)
	    printf("%x %x %x\n",  img[i+2], img[i+1], img[i+0]); */

	  dest[p]     = img[i+2];
	  dest[p + 1] = img[i+1];
	  dest[p + 2] = img[i+0];
	  
	  p += 4;
	}

    }
  
  printf("saving %s\n", name);
  int ret  = zz.save(name,"BMP");
#if QT4
  assert(ret && "saved");
#endif

#if 0
  make_rif (name, img, width, height);
#endif
}
