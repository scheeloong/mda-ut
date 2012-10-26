#include "AquaTux.h"

int main(int argc, char **argv)
{
  if (argc > 1) {
    AquaTux at = AquaTux(argv[1]);
    at.work();
  } else {
    AquaTux at = AquaTux("default_aquatux.csv");
    at.work();
  }

  return 0;
}
