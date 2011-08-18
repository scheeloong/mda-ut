#include "hough.h"
#include <stdio.h>

// maybe we shouldn't save rho/angle pairs to memory
// it might be better to just find end points right away
int found_lines[MAX_LINES*2];

int peak(unsigned char* buf, int dim)
{
  int i=0, rho, angle; 
  int num_lines = 0;
  int thres;
    
  int total = dim*N_ANGLE;
  int max = 0;
  for( i=0; i<total; i++)
    if( buf[i]>max) max = buf[i];

  thres = max*3/7; // can we refine this?
  printf("peak threshold %d\n", thres);

  i=0;
  for( rho=-dim/2; rho<dim/2; rho++)
    for( angle=0; angle<N_ANGLE; angle++)
      {
	if(buf[i] > thres)
	  {
	    int take = 1;
	    // skip point if there is a better one next to it horizontally
	    if((angle < (N_ANGLE-1)) && (buf[i+1] > buf[i]))
	      {
		//printf("%d is better than %d, %d vs %d\n", i+1, i, buf[i+1], buf[i]);
		take = 0;
	      }
	    if((angle > 0) && (buf[i-1] > buf[i]))
	      {
		//printf("%d is better than %d, %d vs %d\n", i-1, i, buf[i-1], buf[i]);
		take = 0;
	      }
	    // vertically
	    if((rho > -dim/2) && (buf[i-N_ANGLE] > buf[i]))
	      take = 0;

	    if((rho < (dim/2-1)) && (buf[i+N_ANGLE] > buf[i]))
	      take = 0;

	    // diagonals
	    if((angle < (N_ANGLE-1)&&(rho < (dim/2-1))) && (buf[i+1+N_ANGLE] > buf[i]))
	      take = 0;
	    if((angle < (N_ANGLE-1)&&(rho > (-dim/2))) && (buf[i+1-N_ANGLE] > buf[i]))
	      take = 0;
	    if((angle > 0)&&(rho > (-dim/2)) && (buf[i-1-N_ANGLE] > buf[i]))
	      take = 0;
	    if((angle > 0)&&(rho < (dim/2-1)) && (buf[i-1+N_ANGLE] > buf[i]))
	      take = 0;


	    if(take && (num_lines < (MAX_LINES-1)))
	      {
		// this is a local maximum
		found_lines[num_lines*2] = rho;
		found_lines[num_lines*2+1] = angle;
		num_lines++;
	      }
	  }
	
	i++;
      }
  
  return num_lines;
}
