#include <stdio.h>
#include <math.h>

#define WIDTH 720
#define HEIGHT 480
#define pos 196.0
#define angle (pos/N_ANGLE*180)
#define ANGLE (angle*M_PI/180.0)
#define xx 233
#define yy 405
#define HALF 1
#if HALF
#define X (xx-WIDTH/2)
#define Y (yy-HEIGHT/2)
#else
#define X (xx)
#define Y (yy)
#endif
#define N_ANGLE 512
int main(void)
{
  printf("using half %d\n", HALF);

  printf("angle %f for pos_angle %f\n", (float)angle, pos);

  //  float rho = (X*cos(ANGLE) + Y*sin(ANGLE));	  
  float rho = 39;
#if 0
  int offset = (int)((WIDTH/2*cos(ANGLE)+HEIGHT/2*sin(ANGLE)));
  rho += offset;
#endif

  printf("rho %f\n", rho);
  
  // if x=0
  int yb = (int)((rho-(-WIDTH/2*cos(ANGLE)))/sin(ANGLE));
  
  // if y=0
  int xb = (int)((rho -(-HEIGHT/2*sin(ANGLE)))/cos(ANGLE));
  
#if HALF
  yb += HEIGHT/2;
  xb +=  WIDTH/2;
  printf("(%d,%d) and (%d,%d)\n", xb ,yb, 0,0);
#else
  printf("(%d,0) and (0,%d)\n", xb,yb);
#endif


  return 0;
}
