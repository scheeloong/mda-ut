#include "line.h"

#define TIGHT 0
extern line* lines;

inline static void swap(short* a, short* b)
{
  short tmp = *a;
  *a = *b;
  *b = tmp;
}

inline static void print_case(int meet)
{
#if CPP
  switch(meet)
    {
    case 0:
      printf("S_S");
      break;
	
    case 1:
      printf("S_E");
      break;
	
    case 2:
      printf("E_S");
      break;
	
    case 3:
      printf("E_E");
      break;
	
    default:
      printf("unkown case");
    }
#endif
}

inline static void set(line* l, short x1, short y1, short x2, short y2)
{
  l->x1 = x1;
  l->x2 = x2;
  l->y1 = y1;
  l->y2 = y2;
}

/* lines are collinear and have a meeting point */
/* take the line that is the maximum of both lines */
void merge_lines(line* line1, line* line2, short meet)
{
  short xmax = MAX4(line1->x1, line1->x2, line2->x1, line2->x2);
  short xmin = MIN4(line1->x1, line1->x2, line2->x1, line2->x2);
  short spanx = xmax - xmin;
  short ymax = MAX4(line1->y1, line1->y2, line2->y1, line2->y2);
  short ymin = MIN4(line1->y1, line1->y2, line2->y1, line2->y2);
  short spany = ymax - ymin;
  line* both[2];

  /* put the meeting ends in the first position */
  switch(meet)
    {
    case 1:
      swap(&line2->x1, &line2->x2);
      swap(&line2->y1, &line2->y2);
      break;
    case 2:
      swap(&line1->x1, &line1->x2);
      swap(&line1->y1, &line1->y2);
      break;
    case 3:
      swap(&line1->x1, &line1->x2);
      swap(&line1->y1, &line1->y2);
      swap(&line2->x1, &line2->x2);
      swap(&line2->y1, &line2->y2);
      break;
    default: ;
    }

#ifdef CPP
  print_case(meet);
  printf(" spanx? %d, after swapping:\n", (spanx > spany));
#endif

  print_line(line1, "line1", 1);
  print_line(line2, "line2", 2);

  both[0] = line1;
  both[1] = line2;

  if(spanx > spany)
    {
      /* meeting point is between the other 2 points in x */
      if(((line2->x2 >= line2->x1) && (line1->x2 <= line2->x1)) ||
	 ((line2->x2 >= line2->x1) && (line1->x2 <= line2->x1))) {
	set( line1, line1->x2, line1->y2, line2->x2, line2->y2 );
	return;
      }

      /* meeting point has smaller x */
      if(line2->x1 <= line2->x2) 
	{
	  int min = (line1->x1 < line2->x1) ? 0:1;
	  int max = (line1->x2 > line2->x2) ? 0:1;

	  set(line1, both[min]->x1,  both[min]->y1,
	      both[max]->x2, both[max]->y2);
	  return;
	}
      else /* meeting point has largest x */     
	{
	  int max = (line1->x1 > line2->x1) ? 0:1;
	  int min = (line1->x2 < line2->x2) ? 0:1;

	  set(line1, both[min]->x1,  both[min]->y1,
	      both[max]->x2, both[max]->y2);
	  return;
	}
    }
  else
    {
      /* meeting point is between the other 2 points in y */
      if(((line2->y2 >= line2->y1) && (line1->y2 <= line2->y1)) ||
	 ((line2->y2 >= line2->y1) && (line1->y2 <= line2->y1))) {
	set( line1, line1->y2, line1->y2,
	     line2->y2, line2->y2 );
	return;
      }

      /* meeting point has smaller y */
      if(line2->y1 <= line2->y2) 
	{
	  int min = (line1->y1 < line2->y1) ? 0:1;
	  int max = (line1->y2 > line2->y2) ? 0:1;

	  set(line1, both[min]->x1,  both[min]->y1,
	      both[max]->x2, both[max]->y2);
	  return;
	}
      else /* meeting point has largest y */      
	{
	  int max = (line1->y1 > line2->y1) ? 0:1;
	  int min = (line1->y2 < line2->y2) ? 0:1;

	  set(line1, both[min]->x1,  both[min]->y1,
	      both[max]->x2, both[max]->y2);
	  return;
	}
    }

}


int cleanup(int n)
{
  int i, j;
  for (i = 0; i < n; i++)
    {
      for (j = i + 1; j < n; j++)
	{
	  if (  collinear(&lines[i], &lines[j]) )
	    {
	      int meet;
	      if (intersect(&lines[i], &lines[j], &meet, TIGHT))
		{
		  int k;

		  print_line(&lines[i],"i", i);
                  print_line(&lines[j],"j", j);

		  merge_lines(&lines[i], &lines[j], meet);                  
		  print_line(&lines[i],"result", i);

		  for (k = j; k < n; k++)
		    lines[k] = lines[k+1];
		  n--;
		}
	    }
	}
    }


  for (i = 0; i < n; i++)
    {
      /* take out lines that are too small */
      int len1 = abs(lines[i].x1-lines[i].x2)+abs(lines[i].y1-lines[i].y2);
      if(len1 < MINPARALLEL)
	{
	  int k;
	  for (k = i; k < n; k++)
	    lines[k] = lines[k+1];
	  n--;
	  i--;
	  continue;
	}
    }

  return n;
}
