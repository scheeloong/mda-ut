#include <iostream>
using namespace std;

/*
  set xrange [-720:720];
  set yrange [-480:480];
  set grid
*/

int main(void)
{
  while(1)
    {
      float x1,y1,x2,y2;
      cout << "enter x1 y1 x2 y2 in decimal: ";
      cin >> x1 >> y1 >> x2 >> y2;
      
      float m = (y2-y1)/(x2-x1);
      float b = y2 - m*x2;
      cout << "plot " << m << "*x + " << b << endl;
    }
  return 0;
}
