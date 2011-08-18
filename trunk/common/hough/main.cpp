#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "detection.h"
#include <qimage.h>
#include "hough.h"
#include <sys/time.h>
#include <time.h>

unsigned char* convert_to_24_bits(unsigned char* img, int width, int height)
{
  unsigned char* dest = new unsigned char[height*width*3];
  int p = 0;
  for(int i=0; i<(4*height*width); i+=4)
    {
      dest[p]     = img[i+2];
      dest[p + 1] = img[i+1];
      dest[p + 2] = img[i+0];
      
      p += 3;
    }
  return dest;
}

int hue_min=150, hue_max=200;

void tweak_filter(char* name)
{
  if(strstr(name,"underwater"))
    {
      printf("changing filter\n");
      hue_min=162, hue_max=30;
    }
  if(strstr(name,"DSC00118"))  
    {
      printf("changing filter\n");
      hue_min=70, hue_max=75;
    }
  if(strstr(name,"Ecole09"))  
    {
      printf("changing filter\n");
      hue_min=99, hue_max=109;
    }
  if(strstr(name,"Duke79"))  
    {
      printf("changing filter\n");
      hue_min=99, hue_max=109;
    }
  if(strstr(name,"DSC00109"))  
    {
      printf("changing filter\n");
      hue_min=78, hue_max=83;
    }
}


int main(int argc, char** argv)
{ 

  if(argc != 2)
    {
      printf("enter name of bmp file to process\n");
      assert(argc ==2);
    }


  QImage img(argv[1]);
  int width = img.width();
  int height = img.height();
  printf("height: %d width %d\n", height, width);

  unsigned char* orig = convert_to_24_bits(img.bits(), width, height);
  tweak_filter(argv[1]);

  /*-------------------------------*/

  process_image(orig, width, height,hue_min,hue_max,0,0,1);

  /*-------------------------------*/
  // print the segments
  extern int num_seg;
  extern short coord[PERLINE*MAXSEG];
  printf("Writing to lines.txt and lines.h\n");
  FILE* file = fopen("lines.txt", "w");
  FILE* filh = fopen("lines.h", "w");
  assert(file && filh);

  fprintf(filh, "line lines[MAXLINES] = {\n");

  printf("identified %d segments\n", num_seg);
  for(int i=0; i<num_seg;i++)
    {
      fprintf(file, "%d,%d,  %d,%d\n", coord[PERLINE*i], coord[PERLINE*i+1],
	      coord[PERLINE*i+2],coord[PERLINE*i+3]);
      fprintf(filh, "{%d,%d,  %d,%d,  %d},\n", coord[PERLINE*i], coord[PERLINE*i+1],
	      coord[PERLINE*i+2],coord[PERLINE*i+3], coord[PERLINE*i+4]);
    }

  fprintf(filh, "{%d,%d,  %d,%d,  %d}\n};\n", 0,0,0,0,0);
  fclose(file);
  fclose(filh);  

  return 0;
}
