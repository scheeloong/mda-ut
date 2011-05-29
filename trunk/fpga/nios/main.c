#include <stdio.h>
#include "interrupts.h"

void init()
{
  timer_interrupt_init();
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
