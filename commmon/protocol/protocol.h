#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <assert.h>
#include <stdio.h>

#ifndef PREVIEW
#include "csocket.h"
#endif

#define INT_TYPE  4
#define INT_T     int
#define INT_T_PTR 0

#define FLOAT_TYPE  4
#define FLOAT_T     float
#define FLOAT_T_PTR 0

#define IMG_TYPE  0
#define IMG_T     unsigned char*
#define IMG_T_PTR 1


#define SIM_PORT     50000
#define MISSION_PORT 60000

enum {R_T, RW_T};
enum cmd_type { 
  NOCMD,
  CMD_GET,
  CMD_PUT  };


#ifdef ONCE
#define EXTERN 
#else
#define EXTERN extern
#endif


// declare the item codes
enum item_type {

#define DEFDATA(NAME, SIZE, MODE, CF, CB) NAME,
#include "protocol.def"  
#undef DEFDATA
  
  LAST_ITEM};


//// declare the storage for all items
// this is how variables can be accessed if data is not sent through a socket
// depending on the type, the item can be a pointer to an allocated buffer
// we declare a _size variable for each item, but should only be used for pointers

#ifdef ONCE
#define DEFDATA(NAME, TYPE, MODE, CB, CF)			\
  EXTERN  TYPE NAME ## _store = 0;				\
  EXTERN  int  NAME ## _size  = 0;	
#else
#define DEFDATA(NAME, TYPE, MODE, CB, CF)			\
  EXTERN  TYPE NAME ## _store;					\
  EXTERN  int  NAME ## _size;	
#endif

#include "protocol.def"  
#undef DEFDATA

// internal 
void blocking_read(CSocket* cs, char* addr, int num);
void blocking_write(CSocket* cs, char* addr, int num);

// hooks
extern void (*pre_image_handler)(cmd_type);
extern void (*post_image_handler)(cmd_type);
extern void (*post_verb_handler)(cmd_type);
extern void (*post_heading_handler)(cmd_type);

// from protocol.cpp
cmd_type listen_to_client(CSocket* cs);
extern item_type last_item_from_client;

void read_item(CSocket* cs, item_type code);
void write_item(CSocket* cs, item_type code);
void tell_server(CSocket* cs, cmd_type command, item_type item);

// client_routines
CSocket* create_client_socket(char* ip_str, int port);

extern bool got_conn;

#undef EXTERN

// hook implementations (not attached to the hooks)
void pre_image_send(cmd_type cmd);
void pre_image_recv(cmd_type cmd);
void post_image_recv(cmd_type cmd);
void post_verb_recv(cmd_type cmd);
void post_verb_send(cmd_type cmd);
void post_cmd_heading_recv_make_absolute(cmd_type t);


const char* get_name(item_type code);
#endif // _PROTOCOL_H
