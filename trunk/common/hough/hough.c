#include <math.h>
#include "hough.h"
#include <assert.h>
#include <stdio.h>

#define EMBED 0
#if !EMBED 
  typedef float  t_trig;
  #define SCALING 1
  #define __SCALING 
#else
  typedef int  t_trig;
  #define SCALING (1<<15)
  #define __SCALING >> 15
#endif


t_trig t_sin[N_ANGLE];
t_trig t_cos[N_ANGLE];

unsigned char* hough_dest;
int hough_dim;

void init_sin_cos()
{
  int i;
  for(i=0; i< N_ANGLE; i++)
    {
      t_sin[i] = (t_trig)(sin((float)i/N_ANGLE*M_PI) * SCALING);
      t_cos[i] = (t_trig)(cos((float)i/N_ANGLE*M_PI) * SCALING);
    }
}

int init_hough(int width, int height)
{
  init_sin_cos();

  // this should be statically allocated
  hough_dim = (int)sqrt(width*width/4 + height*height/4)*2;
  hough_dest = new unsigned char[hough_dim*N_ANGLE];
  
  return hough_dim;
}

unsigned char* houghTransform(unsigned char* buf, int width, int height)
{
  int i,x,y;
  int angle;

  // clear the accumulator
  for(i=0; i<(hough_dim*N_ANGLE); i++)
    hough_dest[i] = 0;

  // outer loop because the sine/cos table is slow to access
  for(angle=0; angle<N_ANGLE; angle++) // width of hough transform
    {
      i=0;
      for(y = -height/2; y < height/2; y++)
	{
	  for(x = -width/2; x < width/2; x++)
	    {
	      int pix = buf[i];
	      int rho;
	      
	      if(pix)
		{
		  unsigned char* d;
		  rho = (int)((x*t_cos[angle] + y*t_sin[angle]) __SCALING);

		  // i think we don't need to do this
		  //int offset = (int)((width/2*t_cos[angle]+height/2*t_sin[angle])/SCALING);
		  //rho += offset;

		  rho += hough_dim/2;
		  if(rho<0) rho = 0;
		  
		  assert(rho < hough_dim);
		  
		  if(x == -2 && y == 206 && angle == 146)
		    printf("x %d,y %d %d\n", x, y, rho);

		  d = &hough_dest[rho*N_ANGLE+angle];
		  if(*d < 255)
		    *d = *d + 1;
		}     

	      i++;
	    }
	}
    }
  return hough_dest;
}



