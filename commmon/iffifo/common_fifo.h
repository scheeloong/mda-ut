#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <linux/stat.h>

#define FIFO_TO_SERVER	"/tmp/FIFO_TO_SERVER"
#define FIFO_TO_CLIENT	"/tmp/FIFO_TO_CLIENT"

// server interface
void init_server_fifos();
char* get_client_query(int* num_bytes, int* packet_code);
void send_to_client(char* str);
char* server_pack_blob(int blob_size, int blob_x, int blob_y, int blob_seen);
char* server_pack_lines(short* lines, int num_lines);

/*----------------------------------------*/
// client interface
void init_client_fifos();
void send_to_server(char* str);
char* get_server_reply(int* num_bytes, int* packet_code);
void send_to_server(char* str);
void client_set(int code, int value);
void client_query(int code);

// all packets have the following header:
// num_bytes
// packet code

/*---------------------------------------------------*/
#define FIFO_GET_LINES 1
// lines packet: 
// num lines
// 5 short int per line

typedef struct {
  short x1, y1, x2, y2;
  short theta; /* between 0 and MAX_THETA **/
} fifo_line;

#define MAX_FIFO_LINES 50
extern int received_fifo_lines;
extern fifo_line fifo_received_lines[MAX_FIFO_LINES];



/*---------------------------------------------------*/
#define FIFO_GET_BLOB 2
// blob packet: 
// 4 short int: size, x, y, seen
typedef struct {
  int size, x, y, seen;
} fifo_blob;
extern fifo_blob fifo_received_blob;




/*---------------------------------------------------*/
#define FIFO_SET 3
// set packet: 
// set code
// 1 int value for the field index (below) and 1 int for the value

#define NUM_FIFO_SETS 8
enum fifo_set_fields { FIFO_SET_MINHUE=0,
		       FIFO_SET_MAXHUE,
		       FIFO_SET_ONLY_BW,
		       FIFO_SET_RM_BW,
		       FIFO_SET_WANT_B,
		       FIFO_SET_BLOB,
		       FIFO_SET_MINBLOB,
		       FIFO_SET_NUMSEG};
/*---------------------------------------------------*/

