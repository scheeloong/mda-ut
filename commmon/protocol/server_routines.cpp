#include "protocol.h"

CSocket *ccs = NULL;
extern int* img_dimx, *img_dimy, *img_encoding;
extern unsigned char** orig_img;
extern int* orig_img_size;
 
#define TERR   \
  if(t == -1) fprintf(stderr, "something went wrong in the sending of the image\n");


void pre_image_send(cmd_type dummy)
{
  int n = (*img_dimx) * (*img_dimy) * 3;

  // send image dimensions
  // carriage return is important!!!!  
  char dims[64];
  sprintf(dims, "%d %d %d\n", *img_dimx, *img_dimy, *img_encoding);
  int t = ccs->Write( dims, strlen(dims)); 
  TERR;

#if COMPRESSION
  n = lzf_compress(*orig_img, *orig_img_size, CUR_IMAGE_store, n);
  printf("compressed %d bytes to %d\n", *orig_img_size, n);
  assert(n != 0);
#else
  CUR_IMAGE_store = *orig_img;
#endif
  CUR_IMAGE_size = n;

  // write the number of bytes transfered
  sprintf(dims, "%d\n", n);
  t = ccs->Write( dims, strlen(dims));
  TERR;

  cout << "sending image" << endl;
}

item_type last_item_from_client = LAST_ITEM;

// this is on the server side
cmd_type listen_to_client(CSocket *cs)
{
cmd_type ret = NOCMD;
  ccs = cs;
  while(ccs->Peek())
    {	
      // read 1 byte for command, one byte for key
      // then dispatch to sepcialized code
      cmd_type  command = NOCMD;
      item_type item    = LAST_ITEM;

      blocking_read(cs, ((char*)&command), 1);
      if(!got_conn) return NOCMD;

      blocking_read(cs, ((char*)&item),    1);
      if(!got_conn) return NOCMD;

      last_item_from_client = item;

      printf("got item %s\n", get_name(item));

      switch(command)
	{
	case CMD_GET:  	  write_item(cs, item); 	  break;
	case CMD_PUT:     read_item(cs, item);      break;
	case NOCMD:
	default:          assert(false);
	}

      if(ret != CMD_PUT) // save the put for relaying
	      ret = command;
    }
  return ret;
}

extern char*VERB_TEXT_store; // in client_routines
void post_verb_send(cmd_type t) 
{
  if(VERB_store > 0)
  {
    assert(VERB_TEXT_store != NULL);
    blocking_write(ccs, VERB_TEXT_store, VERB_store);
  }
}


void post_cmd_heading_recv_make_absolute(cmd_type t)
{
  CMD_HEADING_store = CUR_HEADING_store + CMD_HEADING_store;
}
