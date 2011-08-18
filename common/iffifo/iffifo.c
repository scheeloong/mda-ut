#include "common_fifo.h"
#include <assert.h>
#include <string.h>

#define MAXPACKET 1024
char readbuf[MAXPACKET];

char* get_packet(FILE* fifo, int* num_bytes, int* packet_code, int blocking)
{
  *packet_code = -1;
  char* ret;

 r_again:
  ret = fgets(readbuf, 16, fifo);
  if(ret == NULL)
    {
      if(blocking)
	goto r_again;
      else
	return NULL;
    }
  else
    {
      int accum=0;
      printf("got string: *%s*, --- %d\n", readbuf, strlen(readbuf));
      int found = sscanf(readbuf,"%d %d", num_bytes, packet_code);
      if(found != 2)
	{
	printf("Here is the incomplete packet: %s\n", readbuf);
 	while(fgets(&(readbuf[strlen(readbuf)]), 16, fifo) != NULL);
	int found = sscanf(readbuf,"%d %d", num_bytes, packet_code);
      	assert(found == 2);
	}
      assert((*num_bytes) < MAXPACKET);
      
      printf("got packet: code %d bytes %d\n", *packet_code, *num_bytes);

      while(accum != *num_bytes)
	{
	  int tmp = fread(&(readbuf[accum]),
			  1, *num_bytes-accum, 
			  fifo);
	  if(ferror(fifo) != 0)
	    {
	      printf("received %d bytes\n", tmp);
	      accum += tmp;
	      usleep(50);
	    }
	  else
	    perror("fread");
	}
      readbuf[accum] = 0;

      printf("payload is: %s\n", readbuf);
      return readbuf;
    }
}


/*----------------------------------------------*/
FILE* server_fifo_read;
FILE* server_fifo_write;

void init_server_fifos()
{
  /* Create the FIFO if it does not exist */
  umask(0);
  mknod(FIFO_TO_SERVER, S_IFIFO|0666, 0);
  mknod(FIFO_TO_CLIENT, S_IFIFO|0666, 0);

  int tmp_fd = open(FIFO_TO_SERVER, O_NONBLOCK|O_RDONLY);
  if(tmp_fd == -1) {
    perror("open");
    exit(1);
  }
  server_fifo_read = fdopen(tmp_fd, "r");
  assert(server_fifo_read);

  /*  tmp_fd = open(FIFO_TO_CLIENT, O_NONBLOCK|O_WRONLY);
  if(tmp_fd == -1) {
    perror("open");
    exit(1);
  }

  server_fifo_write = fdopen(tmp_fd, "w");
  */
  printf("Server is waiting for client to communicate through fifos\n");
  server_fifo_write = fopen(FIFO_TO_CLIENT, "w");
  assert(server_fifo_write);


  printf("Server is ready to communicate through fifos\n");
}

char* get_client_query(int* num_bytes, int* packet_code)
{  
  return get_packet(server_fifo_read, num_bytes, packet_code, 0);
}

void send_to_client(char* str)
{
  fputs(str, server_fifo_write);
  fflush(server_fifo_write);
}

char* server_pack_lines(short* lines, int num_lines)
{
  int i, accum =0;
  printf("packing %d lines\n", num_lines);
  assert(num_lines < MAX_FIFO_LINES);

  readbuf[accum] = '\0';

  for(i=0; i<num_lines; i++)
    {
      accum += sprintf(&(readbuf[accum]), "%d %d %d %d %d ",
		       lines[i*5+0],lines[i*5+1],lines[i*5+2],lines[i*5+3],lines[i*5+4]);
      assert(accum < MAXPACKET);
    }
  printf("done packing %d lines\n", num_lines);
  return readbuf;
}

char* server_pack_blob(int blob_size, int blob_x, int blob_y, int blob_seen)
{
  sprintf(readbuf, "%d %d %d %d", 
	  blob_size, blob_x, blob_y, blob_seen);
  assert(strlen(readbuf) < MAXPACKET);
  return readbuf;
}

/*----------------------------------------*/

FILE* client_fifo_read;
FILE* client_fifo_write;

void init_client_fifos()
{
  int tmp_fd = open(FIFO_TO_CLIENT, O_NONBLOCK|O_RDONLY);
  if(tmp_fd == -1) {
    perror("open");
    printf("problem line %d\n", __LINE__);
    exit(1);
  }
  client_fifo_read = fdopen(tmp_fd, "r");
  assert(client_fifo_read);

  tmp_fd = open(FIFO_TO_SERVER, O_NONBLOCK|O_WRONLY);
  if(tmp_fd == -1) {
    perror("open");
    exit(1);
  }
  client_fifo_write = fdopen(tmp_fd, "w");
  assert(client_fifo_write);

  printf("Client is ready to communicate through fifos\n");
}

void send_to_server(char* str)
{
  fputs(str, client_fifo_write);
  fflush(client_fifo_write);
}


int received_fifo_lines;
fifo_line fifo_received_lines[MAX_FIFO_LINES];
fifo_blob fifo_received_blob;

char* get_server_reply(int* num_bytes, int* packet_code)
{
  char* rep = get_packet(client_fifo_read, num_bytes, packet_code, 1);
  char* ptr = rep;
  int decoded;
  // we got the reply, now unpack it
  switch(*packet_code)
    {
    case FIFO_GET_LINES:
      printf("unpacking lines\n");
      received_fifo_lines=0;
      do {
	int ret = 0;
	decoded = 0;
#define TTT  //printf("decoded %d, pointing to %s\n", decoded, ptr);
#define ADV if(ret == 1) decoded++; ptr++; ptr = index(ptr,' '); if(!ptr) break; 
	ret = sscanf(ptr, "%hd ", &(fifo_received_lines[received_fifo_lines].x1)); TTT; ADV;
	ret = sscanf(ptr, "%hd ", &(fifo_received_lines[received_fifo_lines].y1)); TTT; ADV;
	ret = sscanf(ptr, "%hd ", &(fifo_received_lines[received_fifo_lines].x2)); TTT; ADV;
	ret = sscanf(ptr, "%hd ", &(fifo_received_lines[received_fifo_lines].y2)); TTT; ADV;
	ret = sscanf(ptr, "%hd ", &(fifo_received_lines[received_fifo_lines].theta)); TTT; ADV;

	if(decoded == 5)
	  received_fifo_lines++;
      }while((decoded == 5) && (received_fifo_lines < MAX_FIFO_LINES));

      break;
    case FIFO_GET_BLOB:
      decoded = sscanf(rep, "%d %d %d %d", 
		       &(fifo_received_blob.size),
		       &(fifo_received_blob.x),
		       &(fifo_received_blob.y),
		       &(fifo_received_blob.seen));
      assert(decoded == 4);
      break;
    default:
      printf("got fifo packet code %d\n", *packet_code);
      assert(false);
    }

  return rep;
}


void client_set(int code, int value)
{
  char tmp[10];
  sprintf(tmp, "%d %d", code, value);
  sprintf(readbuf, "%d %d\n%s", strlen(tmp), FIFO_SET, tmp);
  send_to_server(readbuf);
}

void client_query(int code)
{
  sprintf(readbuf, "%d %d\n", 0, code);
  send_to_server(readbuf);
}

