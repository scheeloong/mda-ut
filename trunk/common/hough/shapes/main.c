#define MAIN
#include "line.h"

/* start and end points meeting for 2 lines */

void print_line(line* l, char* str, int id)
{
  return;
  printf("%s(%d) coords: %d %d %d %d\n",
	 str, id, l->x1, l->y1, l->x2, l->y2);
}
void print_point(char* str, int id, int x, int y)
{
  return;
  printf("%s(%d) coords: %d %d\n",
	 str, id, x, y);
}

// interface
line vision_result_lines[MAXLINES];
int num_vision_result_lines;
int vision_result_center_x;
int vision_result_center_y;

char used[MAXLINES];
int num_shapes;
int max_num_shapes;
#define LOOSE 1

// globals
int num_lines;
int nseg;
line* lines;

void save_accum(int sides, line* accum)
{
  int k, x=0, y=0;
  num_shapes++;
  
  for (k = 0; k < sides; k++)
    {
      print_line(&accum[k], ", k", k);
      
      x += accum[k].x1 + accum[k].x2;
      y += accum[k].y1 + accum[k].y2;

      memcpy(&(vision_result_lines[num_vision_result_lines]),
	     &(accum[k]),
	     sizeof(line));
      num_vision_result_lines++;
    }

  /* find center */
  x = x/sides/2;
  y = y/sides/2;

  vision_result_center_x = x;
  vision_result_center_y = y;
  print_point("center", 1, x, y);
}


int test_accum(int sides, line* accum)
{
  int n,k;
  int meet;
  int pass =1;
  int num_inter[MAXSEG];
      
  for(n=0; n<sides; n++) num_inter[n] = 0;

  /* check that the sides make up a shape */
  /* ensures that each line intersects with exactly two other lines */
  for (n = 0; n < sides; n++)
    {
      for (k = (n+1); k < sides; k++)
	{
	  if (intersect(&accum[n], &accum[k], &meet, LOOSE))
	    {  num_inter[n]++; num_inter[k]++; }
	}
    }

  for (n = 0; n < sides; n++)  
    if(num_inter[n] != 2) {pass = 0; break; }

  if (pass)
    save_accum(sides, accum);

  return pass;
}

int pile(int start, int cur_accum, line* accum)
{
  int meet;
  int ret = 0;

  used[start] = 1;
  accum[cur_accum] = lines[start];
  cur_accum++;

  if(cur_accum == nseg)  /* we now have a candidate shape */
    {
      ret = test_accum(nseg, accum);
    }
  else 
    {
      int k, t;
      
      for(k=(start+1); k<num_lines; k++)
	{
	  if (used[k]) continue;

	  for(t=0; t<cur_accum; t++) 
	    {
	      if (intersect(&accum[t], &lines[k], &meet, LOOSE) )
		{
		  int u;
#if VERBOSE
		  printf("s%d these lines touch (%d):\n", cur_accum, k);
		  TAB print_line(&lines[k], "k", start);
		  TAB print_line(&accum[t], "t", t);
#endif

		  u = pile( k, cur_accum, accum);

		  /* nothing more can be accomplished because
		     the accumulator is all used up */
		  if(u) 
		    return u;
		}
#if VERBOSE
	      else
		{
		  printf("these lines don't touch:\n");
		  TAB print_line(&lines[k], "k", start);
		  TAB print_line(&accum[t], "t", t);
		}
#endif

	    }
	} /* for */
    }


  /* undo the last addition */
  if(!ret) used[start] = 0;
#if VERBOSE
  if(!ret) printf("undo line %d\n", start);
#endif

  return ret;
}


static inline int isPolygon()
{
  line cur_shape[MAXSEG];
  int start;

  num_shapes = 0;
  max_num_shapes = num_lines/nseg;  /* there can't be more shapes  */
  
  if(max_num_shapes == 0)
    return 0;

  for(start=0; start < num_lines; start++)
    {
      (void)pile( start, 0, cur_shape);

      if(num_shapes >= max_num_shapes)
	break;
    }

  return num_shapes;
}

int maxParallel()
{
  int i,j;
  int maxpair1=0, maxpair2=0, maxlen=0;
  int found=0;
  int n = num_lines;

  for(i=0; i<n; i++)
    {
      int len1 = abs(lines[i].x1-lines[i].x2)+abs(lines[i].y1-lines[i].y2);

      for(j=(i+1); j<n; j++)
	{
	  int len2, min;
	  if(! collinear(&lines[i], &lines[j]))
	    {
#if VERBOSE
	      printf(" %d and %d are not collinear\n", i, j);
#endif
	      continue;
	    }
	  
	  len2 = abs(lines[j].x1-lines[j].x2)+abs(lines[j].y1-lines[j].y2);
	  min = (len1<len2)?len1:len2;
	  
	  if((len2 > MINPARALLEL) && (min > maxlen))
	    {
	      maxlen = min;
	      maxpair1=i;
	      maxpair2=j;
	      found=1;
	    }
	  else
	    {
#if VERBOSE
	      printf(" %d is not long enough\n", j);
#endif
	    }
	}
      if ((found == 0) &&
	  //(abs(lines[i].y1-lines[i].y2) > abs(lines[i].x1-lines[i].x2)) &&
	  (len1 > MINPARALLEL)) 
	{
	  maxlen = len1;
	  maxpair1=i;
	  maxpair2=-1;
	  found=-1;
#if VERBOSE
	  printf("still taking %d\n", i);
#endif
	}
    }

  if(found == 1)  // found 2 segments
    {
#if VERBOSE
      printf("lines %d and %d\n", maxpair1, maxpair2);
#endif 

      print_line(&lines[maxpair1], "l", 1);
      print_line(&lines[maxpair2], "l", 2); 


      memcpy(&(vision_result_lines[0]),
	     &(lines[maxpair1]),
	     sizeof(line));
      memcpy(&(vision_result_lines[1]),
	     &(lines[maxpair2]),
	     sizeof(line));
      vision_result_center_x = (lines[maxpair1].x1+lines[maxpair1].x2+
				lines[maxpair2].x1+lines[maxpair2].x2)/4;
      vision_result_center_y = (lines[maxpair1].y1+lines[maxpair1].y2+
				lines[maxpair2].y1+lines[maxpair2].y2)/4;
      num_vision_result_lines=2;
    } 
  else if (found == -1)  // only found 1 segment
    {      
#if VERBOSE
      printf("lines %d and %d\n", maxpair1, maxpair2);
#endif 
      print_line(&lines[maxpair1], "l", 1);
      
      memcpy(&(vision_result_lines[0]),
	     &(lines[maxpair1]),
	     sizeof(line));
	     
      vision_result_center_x = (lines[maxpair1].x1+lines[maxpair1].x2)/2;
      vision_result_center_y = (lines[maxpair1].y1+lines[maxpair1].y2)/2;
      
      num_vision_result_lines=1;
    }
  return found;
}

void process_lines(line* tlines, int NSEG)
{
  int i;
  int old_n;

  lines = tlines;
  nseg = NSEG;
  num_vision_result_lines = 0;

  old_n = initlines();
  num_lines = old_n;
      
  nseg = NSEG;
      
  if(nseg > MAXSEG) nseg = MAXSEG;
  else if(nseg <= 0) nseg = 1;
      
      
  /* clear used flags */
  for(i=0; i<num_lines; i++) used[i] = 0;

  /* we need to merge lines that overlap
     find cases of lines extending each other? */
  num_lines = cleanup(num_lines);


  printf("%d lines remaining out of %d\n\n", num_lines, old_n);

  for(i=0; i<num_lines; i++) {
    TAB print_line(&lines[i], "i", i);
  }

  printf("nseg is %d\n\n", nseg);
  switch (nseg) 
    {
    case 2:  i = maxParallel(); break; 
    default:
      {
	i = isPolygon();
	// if you didn't find the shape requested, return a parallel pair
	if(num_vision_result_lines == 0)
	  i = maxParallel();
      }
    }
}

#if 0
int main(int argc, char** argv)
{
  line mylines[MAXLINES] = {
    {419,50,  629,173,  342},
    {115,279,  416,50,  150},
    {115,278,  340,108,  151},
    {365,89,  418,50,  151},
    {345,522,  620,276,  137},
    {137,379,  331,523,  360},
    {0,0,  0,0,  0}
  };

  process_lines(mylines, 4);
  return 0;
}
#endif
