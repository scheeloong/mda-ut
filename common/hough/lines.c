#include <math.h>
#include "hough.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// PURE should be set to 1 if the data type in houg.c and lines.c match
#define EMBED 0
#define PURE (0|EMBED)
#if !EMBED 
  typedef float  t_trig;
  #define ISCALING 1.0
#if PURE
  #define I__SCALING 
#else
  #define RAD ((float)angle/N_ANGLE*M_PI)
#endif
#else
  typedef int  t_trig;
  #define ISCALING (1<<15)
  #define I__SCALING >> 15
#endif

#define DISCONTINUITY 20

t_trig t_isin[N_ANGLE];
t_trig t_icos[N_ANGLE];

extern t_trig t_sin[N_ANGLE];
extern t_trig t_cos[N_ANGLE];

// for debugging
unsigned char* probed;

void init_lines(int width, int height)
{
  int i;
  for(i=0; i< N_ANGLE; i++)
    {
      t_isin[i] = (t_trig)(1.0/sin((float)i/N_ANGLE*M_PI) * ISCALING);
      t_icos[i] = (t_trig)(1.0/cos((float)i/N_ANGLE*M_PI) * ISCALING);
    }

  probed = new unsigned char[width*height];
  memset(probed, 0, width*height);
}


int num_seg=0;
short coord[PERLINE*MAXSEG];

#define MIN_LENGTH 4

int global_theta;
int last_matched=0;
int discontinuity=0;
int started=0;
int startx, starty;
int endx, endy;
int term = 0;
int length_so_far = 0;

unsigned char getpix(unsigned char* buf, int x, int y, int width)
{
  probed[y*width+x]++;
  return buf[y*width+x];
}

#define PIX(x,y) getpix(buf,x,y,width)

void test_point(unsigned char* buf, int x, int y, int width, int height)
{
  if((x < width) && (y < (height-3)) && (x > 0) && (y > 0))
    {
      // test a bit wider than just the exact pixel
      bool pixel_is_set = PIX(x,y) || PIX(x,(y+1)) || PIX(x,y+2) || PIX(x,(y+3));

      if(!pixel_is_set && x > 2)
	{
	  pixel_is_set = PIX((x-1),y) || PIX((x-2),y);
	}
      if(!pixel_is_set && x <(width-2))
	{
	  pixel_is_set = PIX((x+1),y) || PIX((x+2),y);
	}
      
      if(pixel_is_set)
	{
	  length_so_far++;
	  if(!last_matched)
	    {
	      discontinuity=0;
	      if(!started) // start a segment
		{
		  startx = x;
		  starty = y;
		  started = 1;
		}
	    }
	  else // save this pixel in case it is the end of the segment
	    {
	      endx = x;
	      endy = y;
	    }
	  last_matched=1;
	}
      else // pixel is off
	{
	  if(started)
	    {
	      discontinuity++;
	      if(discontinuity >= DISCONTINUITY)
		{
		  started = 0;
		  discontinuity = 0;
		  // save the segment
		  if((num_seg < (MAXSEG-1)) && (length_so_far > MIN_LENGTH))
		    {
		      coord[num_seg*PERLINE] = startx;
		      coord[num_seg*PERLINE+1] = starty;
		      coord[num_seg*PERLINE+2] = endx;
		      coord[num_seg*PERLINE+3] = endy;
		      coord[num_seg*PERLINE+4] = global_theta;
		      num_seg++;

		      /*fprintf(stderr, "saving x:%d y:%d x:%d y:%d\n", 
			      coord[num_seg*PERLINE], coord[num_seg*PERLINE+1], 
			      coord[num_seg*PERLINE+2], coord[num_seg*PERLINE+3]
			      );*/
		    }
		  length_so_far = 0;
		}
	    }
	  last_matched = 0;
	}
    }
  else
    last_matched = 0;
}

// probe line instead of drawing it
// based on www.geocities.com/mtahirshahzad
void probe_line(unsigned char* buf, 
		int width, int height,
		const int x_1,const int y_1,
		const int x_2,const int y_2)
{
  
  //fprintf(stderr, "testing on line connecting %d,%d and %d,%d width %d\n", 
  //        x_1,y_1,x_2,y_2, width);
  

  int x1=x_1;
  int y1=y_1;
  
  int x2=x_2;
  int y2=y_2;
  
  if(x_1>x_2)
    {
      x1=x_2;
      y1=y_2;
      
      x2=x_1;
      y2=y_1;
    }
  
  int dx=(x2-x1)<0?(x1-x2):(x2-x1);
  int dy=(y2-y1)<0?(y1-y2):(y2-y1);
  int inc_dec=((y2>=y1)?1:-1);
  int x,y;

  started = 0;
  last_matched = 0;
  discontinuity = 0;
  
  if(dx>dy)
    {
      int two_dy=(2*dy);
      int two_dy_dx=(2*(dy-dx));
      int p=((2*dy)-dx);
      
      x=x1;
      y=y1;
      
      while(x<=x2)
	{
	  //fprintf(stderr, "x:%d y:%d p:%d\n", x,y, p);

#ifndef TONLY
	  test_point(buf,x,y, width, height);
#endif
	  x++;
	  
	  if(p<0)
	    p+=two_dy;
	  
	  else
	    {
	      y+=inc_dec;
	      p+=two_dy_dx;
	    }
	}
    }
  
  else
    {
      int two_dx=(2*dx);
      int two_dx_dy=(2*(dx-dy));
      int p=((2*dx)-dy);
      
      x=x1;
      y=y1;
      
      while(y!=y2)
	{
	  //fprintf(stderr, "x:%d y:%d p:%d\n", x,y, p);

#ifndef TONLY
	  test_point(buf,x,y, width, height);
#endif
	  y+=inc_dec;
	  
	  if(p<0)
	    p+=two_dx;
	  
	  else
	    {
	      x++;
	      p+=two_dx_dy;
	    }
	}
    }

#ifndef TONLY
  // if we are still on the line after finishing the segment
  if(started && (num_seg < MAXSEG) && (length_so_far > MIN_LENGTH))
    {
      coord[num_seg*PERLINE] = startx;
      coord[num_seg*PERLINE+1] = starty;
      coord[num_seg*PERLINE+2] = last_matched?x:endx;
      coord[num_seg*PERLINE+3] = last_matched?y:endy;
      coord[num_seg*PERLINE+4] = global_theta;
      /*fprintf(stderr, "saving x:%d y:%d x:%d y:%d curx:%d cury:%d\n", 
	      coord[num_seg*PERLINE], coord[num_seg*PERLINE+1], 
	      coord[num_seg*PERLINE+2], coord[num_seg*PERLINE+3], 
	      x,y
	      );*/
      num_seg++;
    }
#endif

}


void init_coord(short* cur_coord)
{
  // a NULL lines marks the end of the lines found
  cur_coord[0] = 0;
  cur_coord[1] = 0;
  cur_coord[2] = 0;
  cur_coord[3] = 0;
  cur_coord[4] = 0;
}

extern int found_lines[MAX_LINES*2];
#define INIFINITY 143500

unsigned char* findLines(unsigned char* buf, int num, int width, int height)
{
  int i;
  num_seg = 0;


  for(i=0; i<num; i++)
    {
      int rho = found_lines[2*i];
      int angle = found_lines[2*i+1];

      int xbase=0;
      int ybase=0;

      global_theta = angle;

      // find 2 points to draw a line
      // if x=0
#if PURE
      int yb = (int)(((rho-((-width/2*t_cos[angle]) I__SCALING))*
		      t_isin[angle]) I__SCALING);
#else
      int yb = (int)(((rho-((-width/2*cos(RAD))))/sin(RAD)));
#endif

      yb += height/2;

      if(yb < 0)
	{
#if PURE
	  yb = (int)(((rho-((width/2*t_cos[angle]) I__SCALING))*
		      t_isin[angle]) I__SCALING);
#else
	  yb = (int)((rho-(width/2*cos(RAD)))/ sin(RAD));
#endif
	  yb += height/2;
	  xbase=width;
	}

      // if y=0
#if PURE
      int xb = (int)(((rho-((-height/2*t_sin[angle]) I__SCALING))*
		      t_icos[angle]) I__SCALING);
#else
      int xb = (int)((rho-(-height/2*sin(RAD))) / cos(RAD));
#endif
      xb +=  width/2;
      if(xb < 0)
	{
#if PURE
	  xb = (int)(((rho-((height/2*t_sin[angle])  I__SCALING) )*
		      t_icos[angle]) I__SCALING);
#else
	  xb = (int)(((rho-((height/2*sin(RAD))) ) / cos(RAD)));
#endif
	  xb +=  width/2;
	  ybase = height;
	}      

      //printf("(%d,%d) \n", xb,yb /*, -t_icos[angle]/t_isin[angle]*/);

      //printf("(%d,%d) angle %f deg\n", rho, angle, (float)angle/N_ANGLE*180);

      if((yb > INIFINITY) || (yb < -INIFINITY)) // assume vertical line
	probe_line(buf, 
		   width, height,
		   xb,height, xb,0);
      else if((xb > INIFINITY) || (xb < -INIFINITY)) // assume horizontal line
	probe_line(buf, 
		   width, height,
		   0,yb, width,yb);
      else
	{
	  probe_line(buf, 
		     width, height,
		     xbase,yb, xb, ybase); 
	}
    }

  if(num_seg < (MAXSEG-1))
    {
      init_coord(&(coord[num_seg*5]));
    }

  return probed;
}




#ifdef TONLY 
int found_lines[MAX_LINES*2];
int main(void)
{
  probe_line(NULL, 
	     0, 0,
	     720,199,
	     336,0);


  return 0;
}
#endif
