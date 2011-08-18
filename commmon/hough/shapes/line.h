#ifndef line_H
#define line_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define MAX_THETA 512
#define MAXLINES 255
#define MAXSEG   8

typedef struct
{
  short x1, y1, x2, y2;
  short theta; /* between 0 and MAX_THETA **/

  /*short mag; */
  /*short dir_x, dir_y; */
} line ;


void process_lines(line* tlines, int NSEG);

void print_line(line* l, char* str, int id);
line* create_line(short _x1, short _y1, short _x2, short _y2); 
int cleanup(int n);
int intersect(const line* line1, const line* line2, int* meet, int loose);
void draw_point(int x, int y);
int collinear(const line* line1, const line* line2);

void magnitude(line* l);
/* returns the magnitude of the line */
/* required: begin and endpoints of a given line */

/* DIRECTION: */
/* split into x and y components of the direction VECTOR */
/* such that sqrt( (dir_x)^2 + (dir_y)^2 ) = 1 */

void direction_x(line* l);
/* returns the x-component that the line has */
/* requires: two points (begin and end) of the line */

void direction_y(line* l);
int initlines();

#define MIN2(x,y) ((x) <= (y) ? (x) : (y))
#define MAX2(x,y) ((x) >= (y) ? (x) : (y))
#ifndef MYOPS3
#define MIN3(x,y,z)  ((y) <= (z) ? \
		      MIN2(x,y)	   \
		      :		   \
		      MIN2(x,y))

#define MAX3(x,y,z)  ((y) >= (z) ? \
		      MAX2(x,y)	   \
		      :		   \
		      MAX2(x,z))
#endif

#define MAX4(v,x,y,z)  ((v) >= (x) ?			\
			MAX3(v,y,z): MAX3(x,y,z))

#define MIN4(v,x,y,z)  ((v) >= (x) ?			\
			MIN3(x,y,z): MIN3(v,y,z))

#define ANGLE_THRESH 5
#define EUCLID 80
#define THRESHOLD 10
#define MINPARALLEL 15
#define ANGLE_STRICT 2
#endif



#ifdef CPP
#define TAB printf("\t"); 
#else
#define TAB
#endif
