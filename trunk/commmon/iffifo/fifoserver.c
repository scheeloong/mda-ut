/*****************************************************************************
 Excerpt from "Linux Programmer's Guide - Chapter 6"
 (C)opyright 1994-1995, Scott Burkett
 ***************************************************************************** 
 MODULE: fifoserver.c
 *****************************************************************************/
#include "common_fifo.h"

int main(void)
{
  init_server_fifos();  
  while(1)
    {
      int num_bytes, code;
      char* ret = get_client_query(&num_bytes, &code);
      if(ret != NULL)
	printf("got code %d\n", code);
    }
  return(0);
}
