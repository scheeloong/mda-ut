#define ONCE
#include "protocol.h"

bool     got_conn;


#define LAST_SWITCH   case LAST_ITEM: default:   assert(false);


// should we test for disconnected socket here?
void blocking_read(CSocket *cs, char* addr, int num)
{
  int rcvd = 0;							
  int iLen;

  assert(num != 0);

  while(num != 0) {
    iLen = cs->Read(&(addr[rcvd]), num);
    if(iLen < 0) { got_conn = false; return; }
    num -= iLen;							
    rcvd += iLen;							
  }						
}

// should we test for disconnected socket here?
void blocking_write(CSocket *cs, char* addr, int num)
{
  int sent = 0;								
  int iLen;								
  while(num != 0) {							
    iLen = cs->Write(&(addr[sent]), num);
    if(iLen < 0) { got_conn = false; return; }
    num -= iLen;							
    sent += iLen;							
  }			
}

/************************************************************************************/

// get the string for an item
const char* get_name(item_type code)
{
  switch(code) {

#define DEFDATA(NAME, SIZE, MODE, CF, CB)		\
  case NAME: return #NAME;
#include "protocol.def"  
#undef DEFDATA

    LAST_SWITCH; return NULL;
  };
}

// todo: distinguish between pointers and non-pointers
void read_item(CSocket *cs, item_type code)
{
  void (*f)(cmd_type);
  const char* str = get_name(code);
  //printf("reading %s\n", str);

  switch(code) {

#define DEFDATA(NAME, SIZE, MODE, CF, CB)					\
    case NAME: {							\
      int tmp = SIZE ## YPE;						\
      bool is_ptr = (SIZE ## _PTR == 1);				\
      if(MODE == R_T) { printf("Read-only field: %s\n",str); assert(false); } \
      f = CF; if(f != NULL) f(CMD_GET);					\
      /* must be after the call to CF */				\
      char* addr = (is_ptr)?(char*)((int)(NAME ## _store)):((char*)&(NAME ## _store)); \
      if(!is_ptr)							\
	blocking_read(cs, addr, tmp);					\
      else								\
	blocking_read(cs, addr, (NAME ## _size));				\
      f = CB; if(f != NULL) f(CMD_GET);					\
      break;								\
  }

#include "protocol.def"  
#undef DEFDATA


    LAST_SWITCH;
  };
}


void write_item(CSocket *cs, item_type code)
{
  void (*f)(cmd_type);
  const char* str = get_name(code);
  //printf("writing %s\n", str);

  switch(code) {

#define DEFDATA(NAME, SIZE, MODE, CF, CB)					\
    case NAME: {							\
      int tmp = SIZE ## YPE;						\
      bool is_ptr = (SIZE ## _PTR == 1);				\
      f = CF; if(f != NULL) f(CMD_PUT);					\
      /* must be after the call to CF */				\
      char* addr = (is_ptr)?(char*)((int)(NAME ## _store)):((char*)&(NAME ## _store)); \
      if(!is_ptr)							\
	blocking_write(cs, addr, tmp);					\
      else								\
	blocking_write(cs, addr, (NAME ## _size));				\
      f = CB; if(f != NULL) f(CMD_PUT);					\
      break;								\
  }

#include "protocol.def"  
#undef DEFDATA

    LAST_SWITCH;
  };
}


