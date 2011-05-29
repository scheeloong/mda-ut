#include <stdio.h>
#include "interrupts.h"

void init()
{
  interrupt_init();
}

int main()
{
  init();
  while (1) {
    puts("Hello World!");
    while(getchar() != '\n') ;
  }
  return 0;
}
