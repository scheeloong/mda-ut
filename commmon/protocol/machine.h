
#define WIN_SERV 0
#define LIN_SERV 1
#define COMPRESSION 1

/////////////////////////////////////////////////////////

#if WIN_SERV

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include <windows.h>

#elif LIN_SERV

#include <pthread.h>

#endif

#if COMPRESSION
#include "lzf.h"
#endif
