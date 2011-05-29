// ensure NIOS-only functions are ignored when not using a NIOS compiler

#ifdef NIOSCC
#define wrctl(x,y) __builtin_wrctl(x,y)
#define rdctl(x) __builtin_rdctl(x)
#endif

#ifndef NIOSCC
#define wrctl(x,y)
#define rdctl(x) 0
#endif

#define TIMER_BASE 0x0

void timer_interrupt_init();
void handle_timer_interrupt();
void interrupt_handler();
