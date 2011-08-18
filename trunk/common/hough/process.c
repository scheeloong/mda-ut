#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "detection.h"
#include "hough.h"
#include <sys/time.h>
#include <time.h>

short* process_image(unsigned char* orig, 
		     int width, int height,
		     int hue_min, int hue_max,
		     int only_black_white,
		     int remove_black_white,
		     int want_black,
		     int want_blob,
		     int blob_min_size,
		     int save)
{
  unsigned char* binary_im;
  extern short coord[PERLINE*MAXSEG];

  init_coord(coord);

  if(orig == NULL) return coord;

  if(save) save_image("orig.bmp", orig, width, height, 0, 0);

  /*-------------------------------------------------------*/  
  if(only_black_white)
    {
      binary_im = is_RGB_black_or_white(orig, 
					width*height, 
					want_black);
      
    }
  else 
    {
      unsigned char *image = image_rgb_to_hsv(orig, height*width);
      if(save) save_image("hsv.bmp", image, width, height, 0, 0);
     
      
      /*printf("coords are %d image is %d \n", (width*(height-50)+176)*3, (int)image);
	printf("rgb (hsv) at x %d y %d is %d %d %d\n",  
	176, height-50,
	image[(width*(height-50)+176)*3+0],
	image[(width*(height-50)+176)*3+1],
	image[(width*(height-50)+176)*3+2]
	); */
      
 
      /*-------------------------------------------------------*/  
      binary_im = colorFilter(image, 
			      height*width,
			      hue_min, hue_max,
			      only_black_white,
			      remove_black_white);
    }
  if(save) save_image("filtered.bmp", binary_im, width, height, 1, 0);

  /*-------------------------------------------------------*/  

  if(want_blob)
    {
      getCentroid(binary_im,
		  width,height,
		  blob_min_size);
      free(binary_im);
      return NULL;
    }

  /*-------------------------------------------------------*/  

  unsigned char* sob = sobelFilter(binary_im, height, width, 0 /*threshold*/);
  if(save) save_image("sobel.bmp", sob, width, height, 1, 0);

  /*-------------------------------------------------------*/  

  unsigned char*thin = thinEdges(sob, height, width);
  if(save) save_image("thin.bmp", thin, width, height, 1, 0);

  /*-------------------------------------------------------*/  
  struct timeval start, end;
  gettimeofday(&start, NULL);

  int dim = init_hough(width, height);
  init_lines(width, height);

  // impulseC starts here  
  unsigned char*trans = houghTransform(thin, width, height);
  gettimeofday(&end, NULL);
  printf("hough took %e usec\n", ((end.tv_sec-start.tv_sec)*1e6+(end.tv_usec - start.tv_usec)));

  if(save) save_image("hough.bmp", trans, N_ANGLE, dim, 2, 0);

  /*-------------------------------------------------------*/  

  int num_peaks = peak(trans, dim);
  printf("found %d peaks\n", num_peaks);

  if(save) 
    {
      unsigned char* peaks = new unsigned char[N_ANGLE*dim];
      extern int found_lines[MAX_LINES*2];
      memset(peaks, 0, N_ANGLE*dim);
      for(int i=0; i< num_peaks; i++)
	{
	  int y = found_lines[2*i]+dim/2;
	  int x = found_lines[2*i+1];
	  //printf("peak at %d,%d dim %d\n",x,y, dim);
	  peaks[y*N_ANGLE+x] = 1;
	}
      save_image("peaks.bmp", peaks, N_ANGLE, dim, 1, 0);
    }

  /*-------------------------------------------------------*/  

  unsigned char* probed = findLines(thin, num_peaks, width, height);
  if(save) save_image("probed.bmp", probed, width, height, 1, thin);


  /*-------------------------------------------------------*/  

  unsigned char* identified = new unsigned char[width*height];
  memset(identified, 0, width*height);

  /*-------------------------------*/
  // print the segments
  extern int num_seg;

  for(int i=0; i<num_seg;i++)
    {
      short Ax = coord[PERLINE*i], Ay = coord[PERLINE*i+1];
      short Bx = coord[PERLINE*i+2], By = coord[PERLINE*i+3]; 

      // clip the coordinates
      if(Ax < 0) Ax = 0;
      if(Bx < 0) Bx = 0;
      if(Ax > width) Ax = width-1;
      if(Bx > width) Bx = width-1;
      
      if(Ay < 0) Ay = 0;
      if(By < 0) By = 0;
      if(Ay > height) Ay = height-1;
      if(By > height) By = height-1;

      // re-assign them in the vector
      coord[PERLINE*i] = Ax, coord[PERLINE*i+1] = Ay;
      coord[PERLINE*i+2] = Bx, coord[PERLINE*i+3] = By; 
      
      if(save) draw_line(Ax, Ay, Bx, By, 1, identified, width, height, 0);
      
      draw_line(Ax, Ay, Bx, By, 255, orig, width, height, 1);
    }

  if(save) save_image("identified.bmp", identified, width, height, 1, 0);

#if 0
  printf("identified %d segments\n", num_seg);
  for(int i=0; i<num_seg;i++)
    {
#define TEST_RANGE(a,b) ((a<b)&&(a>=0))
	bool error = !(TEST_RANGE(coord[PERLINE*i], width) &&
	              TEST_RANGE(coord[PERLINE*i+2], width) &&
	              TEST_RANGE(coord[PERLINE*i+1], height) &&
	              TEST_RANGE(coord[PERLINE*i+3], height));
	if(1)
	{
      fprintf(stdout, "{%d,%d,  %d,%d,  %d},\n",
	      coord[PERLINE*i], coord[PERLINE*i+1],
	      coord[PERLINE*i+2],coord[PERLINE*i+3], coord[PERLINE*i+4]);
      if((   (coord[PERLINE*i]== 352) &&  (coord[PERLINE*i+1] == 287)) ||
	 (   (coord[PERLINE*i+2]== 352) &&  (coord[PERLINE*i+3] == 287)))
	   exit(0);
	}
    }
#endif
  /*-------------------------------*/

  // cleanup
  free(binary_im);
  free(sob);
  delete []thin;
  delete []trans;
  delete []probed;
  delete []identified;
 

if(0)
{
	printf("Exiting after one iteration in vision/process.c\n");
	exit(0);
}

  return coord;
}
