/*****************************************************************************
 Excerpt from "Linux Programmer's Guide - Chapter 6"
 (C)opyright 1994-1995, Scott Burkett
 ***************************************************************************** 
 MODULE: fifoclient.c
 *****************************************************************************/

#include "common_fifo.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int num_bytes, packet_code;

	init_client_fifos();
	client_query(FIFO_GET_LINES); 
	(void)get_server_reply(&num_bytes, &packet_code);
        return(0);
}
