#include <math.h>
#include <stdlib.h>
#include "line.h"

/* using namespace std; */

extern line* lines;

int intersect(const line* line1, const line* line2, int* meet, int loose)
{ 
  int tr = THRESHOLD;
  if(loose) tr = EUCLID;
  
  if (( abs( line1->x1 - line2->x1 ) + 
	abs( line1->y1 - line2->y1 ) ) < tr )
    {
      /* begin of line1 intersects begin of line2 */
      *meet = 0;
      return 1;
    }

  if (( abs( line1->x1 - line2->x2 ) + 
	abs( line1->y1 - line2->y2 ) ) < tr )
    {
      /* begin of line1 intersects begin of line2 */
      *meet = 1;
      return 1;
    }

  if (( abs( line1->x2 - line2->x1 ) + 
	abs( line1->y2 - line2->y1 ) ) < tr )
    {
      /* end of line2 intersects with begin of line1 */
      *meet = 2;
      return 1;
    }

  if (( abs( line1->x2 - line2->x2 ) + 
	abs( line1->y2 - line2->y2 ) ) < tr )
    {
      /* end of line2 intersects with end of line2 */
      *meet = 3;
      return 1;
    }

  return 0;
}

int collinear(const line* line1, const line* line2)
{
  int tr = ANGLE_THRESH;
  /* if(loose) tr = ANGLE_STRICT; */

  /* compare 100*cosines */
  /* short diff = abs(line1->dir_x - line2->dir_x); */
  int diff = abs(line1->theta - line2->theta);
  if(diff < tr)
    return 1;

  /* theta is periodic at MAX_THETA */
  if((line1->theta < tr/2) && 
     ((MAX_THETA-line1->theta) < tr/2))
    return 1;

  if((line2->theta < tr/2) && 
     ((MAX_THETA-line2->theta) < tr/2))
    return 1;
  
  return 0;
}

int initlines()
{
  int i;
  for(i=0; i<MAXLINES; i++)
    {
      print_line(&lines[i], "init", i);
      if(!(lines[i].x1 || lines[i].x2 || lines[i].y1 || lines[i].y2))
	break;
    }
  return i;
}
		
