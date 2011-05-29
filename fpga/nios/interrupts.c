#include <stdio.h>
#include "interrupts.h"

volatile int *timer_addr = TIMER_BASE;

void interrupt_init()
{
  // enable interrupts
  wrctl(1,1);
  // enable IRQ 0
  wrctl(3,1);
}

void handle_timer_interrupt()
{
  // print 'ti' for Timer Interrupt
  puts("ti");
  // clear TO bit on status register to acknowledge
  *timer_addr = 0;
}

void interrupt_handler()
{
  int ipending = rdctl(4);
  if (ipending & 0x1) {
    handle_timer_interrupt();
  }
}

