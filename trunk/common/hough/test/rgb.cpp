#include <iostream>
#include "detection.h"
#include "hough.h"
#include <math.h>
using namespace std;

HSV rgb_to_hsv(RGB rgb);

int ss(float a)
{
  if(a < 0) return 0;
  else if(a > 255) return 255;
  else return (int)a;
}

int main(void)
{
  while(1)
    {
      int tr,tg,tb;
      cout << "enter r g b in decimal: ";
      cin >> tr >> tg >> tb;
      
      RGB rgb;
      rgb.red = tr;
      rgb.green = tg;
      rgb.blue = tb;

      HSV hsv = rgb_to_hsv(rgb); 
      cout << "HSV in decimal ";
      cout << (int)hsv.hue  << " " 
	   <<  (int)hsv.sat << " " 
	   << (int)hsv.val << endl;

      cout << "HSV normalized ";
      cout << (float)hsv.hue/255*360  << " " 
	   <<  hsv.sat*100.0/255 << " "
	   << hsv.val*100.0/255 << endl;

      cout << "RGB from YCrCb ";
      int Y = tr;
      int Cr = tg;
      int Cb = tb;      
      cout << ss(Y + 1.402*(Cr-128)) << " "
	   << ss(Y - 0.34414*(Cb-128) - 0.71414*(Cr-128)) << " " 
	   << ss(Y + 1.772*(Cb-128)) << endl;

      cout << "cos and sin red: " << cos((float)tr/N_ANGLE*M_PI) << " "
	   << " + " << sin((float)tr/N_ANGLE*M_PI) << " " << endl;


      printf("rg %x %x\n", tr, tg);
    }


  return 0;
}
