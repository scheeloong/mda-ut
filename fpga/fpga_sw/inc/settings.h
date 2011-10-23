// Set some #define values to use as settings

// allow the user to interact when using commands
#define INTERACTIVE

// log state every timer interrupt
//#define LOG

#ifndef INTERACTIVE
#undef puts
#undef printf
#undef putchar
#else
#include <stdio.h>
#endif
