#include "protocol.h"
#include <stdlib.h>

#define MAX_LINE 256

int* pic_dimx, *pic_dimy;
int* pic_encoding;
CSocket *pNewSocket;

unsigned char* comp_buf;
int comp_buf_size = 0;
int allocated_size_comp = 0;
int allocated_size_cur = 0;

void DieWithError(char *errorMessage)
{
  printf("%s\n", errorMessage);
  exit(-1);
}

void read_line(bool twodims)
{
  char line[MAX_LINE];
  int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() */
  char last=1;

  int sock = pNewSocket->get_socket();
  line[0] = 0;
  totalBytesRcvd = 0;
  while (last != '\n')
    {
      /* Receive 1 character at a time */
      if(totalBytesRcvd >= MAX_LINE) {
	assert("Line too long" && false); }

      if ((bytesRcvd = recv(sock, &(line[totalBytesRcvd]), 1, 0)) <= 0)
	DieWithError("recv() failed or connection closed prematurely");

      last = line[totalBytesRcvd];
      totalBytesRcvd += bytesRcvd;
    }
  line[totalBytesRcvd] = 0; // string terminator

  //printf("Received, two_dim? %d: %s", twodims, line);      /*  echo string */  

  bool success = false;
  int one_dim = 0;
  if(twodims)
    {
      int ret = sscanf(line, "%d %d %d", pic_dimx, pic_dimy, pic_encoding);
      success = (ret==3);
    }
  else
    {
      int ret = sscanf(line, "%d", &one_dim);
      success = (ret==1);
    }

  /**** check first if we got it right *******************************/
  if(!success)
    {
      printf("Parse error in the image dimensions\n");
      exit(-2);
    }
  else if(twodims)
    {
      int d = (*pic_dimx) * (*pic_dimy) * 3;
      
      if(allocated_size_cur < d)
	{
	  if(allocated_size_cur)
	    delete []CUR_IMAGE_store;
	  CUR_IMAGE_store = new unsigned char[d];
	  allocated_size_cur = d;
	}
      CUR_IMAGE_size = d; 
    }
  else 
    {
#if COMPRESSION
      if(allocated_size_comp < one_dim)
	{
	  if(allocated_size_comp)
	    delete []comp_buf;
	  
	  comp_buf = new unsigned char[one_dim];
	  allocated_size_comp = one_dim;
	}
      comp_buf_size = one_dim;
#endif

    }
}


void swap_buffers()
{
  unsigned char* tmp;
  int tmpi;

  tmp = CUR_IMAGE_store;
  tmpi = CUR_IMAGE_size;

  CUR_IMAGE_store = comp_buf;
  CUR_IMAGE_size = comp_buf_size;
  
  comp_buf = tmp;
  comp_buf_size = tmpi;
}

void pre_image_recv(cmd_type cmd) 
{
  assert(cmd == CMD_GET);
  read_line(true);
  read_line(false);

  // receive the image in the comp_buf location
  // and not the default CUR_IMAGE_store
#if COMPRESSION
  swap_buffers();
  assert(CUR_IMAGE_size <= comp_buf_size);
#endif
}

void post_image_recv(cmd_type cmd) 
{
  assert(cmd == CMD_GET);

  /* For lzf it is needed to retreive all the data before starting decompression*/ 
#if COMPRESSION  

  swap_buffers();
  assert(CUR_IMAGE_size >= comp_buf_size);

  int uncomp_size = lzf_decompress(comp_buf,comp_buf_size,   // src
				   CUR_IMAGE_store, CUR_IMAGE_size); // target
  if(uncomp_size==0)
    fprintf(stderr,"LZF : DEcompression Error\n");
  /*  else
      printf("Decompressed %d bytes to %d, max was %d\n", 
      comp_buf_size, uncomp_size, CUR_IMAGE_size);*/

  CUR_IMAGE_size = uncomp_size;
#endif
}


void tell_server(CSocket *cs, cmd_type command, item_type item)
{
  blocking_write(cs, ((char*)&command), 1);
  blocking_write(cs, ((char*)&item),    1);
  if(command == CMD_GET)
    read_item(cs, item);
  else
    write_item(cs, item);
}

CSocket* create_client_socket(char* ip_str, int port)
{
    try {
      pNewSocket = new CSocket(ip_str, port);
    } 
    catch( CSocketException se)
      {
	printf("Could not connect: %s:%d\n", ip_str, port);
	pNewSocket = NULL;
      }

    if(pNewSocket)
      printf("Client connected");

    return pNewSocket;
}

char* VERB_TEXT_store = NULL;
void post_verb_recv(cmd_type t) 
{
  if(VERB_TEXT_store != NULL)
    {
      delete []VERB_TEXT_store;
      VERB_TEXT_store = NULL;
    }

  if(VERB_store > 0)
    {
      blocking_read(pNewSocket, VERB_TEXT_store, VERB_store);
    }
}
