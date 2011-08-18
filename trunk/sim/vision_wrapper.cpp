#include <stdio.h>
#include <GL/glut.h>
#include <hough.h>
#ifdef MAXSEG
#undef MAXSEG
#endif
#include <line.h>
#include <common_fifo.h>

// this is the 'interface' to the vision routines
extern int blob_size, blob_x, blob_y, blob_seen;
// interface
extern line vision_result_lines[MAX_LINES];
extern int num_vision_result_lines;
extern int vision_result_center_x;
extern int vision_result_center_y;

extern GLubyte *pixels_cpy;
extern int window_width, window_height;

int fifo_fields[NUM_FIFO_SETS];

void run_vision()
{
#if HAS_FIFO
   FILE* file = stderr;
   int save=1;


   /*
   //memset(fifo_fields, 0, NUM_FIFO_SETS*sizeof(int));
     fifo_fields[FIFO_SET_ONLY_BW] = 1;
     fifo_fields[FIFO_SET_WANT_B] = 0;
   */


   short* coord = process_image(pixels_cpy,
                                window_width, window_height,
                                fifo_fields[FIFO_SET_MINHUE],
                                fifo_fields[FIFO_SET_MAXHUE],
                                fifo_fields[FIFO_SET_ONLY_BW],
                                fifo_fields[FIFO_SET_RM_BW],
                                fifo_fields[FIFO_SET_WANT_B],
                                fifo_fields[FIFO_SET_BLOB],
                                fifo_fields[FIFO_SET_MINBLOB],
                                save);

   if (fifo_fields[FIFO_SET_BLOB] == 0)
   {
      printf("num segments is %d\n", fifo_fields[FIFO_SET_NUMSEG]);
      process_lines((line*)coord, fifo_fields[FIFO_SET_NUMSEG]);
   }

   if (!fifo_fields[FIFO_SET_BLOB])
   {
      printf("identified %d segments\n", num_vision_result_lines);
      for (int i=0; i<num_vision_result_lines;i++)
      {
         fprintf(file, "{%d,%d,  %d,%d}, (%d)\n",
                 vision_result_lines[i].x1,
                 vision_result_lines[i].y1,
                 vision_result_lines[i].x2,
                 vision_result_lines[i].y2,
                 vision_result_lines[i].theta);
      }
      printf("last shape center %d,%d\n",
             vision_result_center_x,
             vision_result_center_y);
   }
   else
   {
      printf("blob_found size %d, {%d,%d}, seen %d\n",
             blob_size, blob_x, blob_y, blob_seen);
   }

#endif
}

void init_vision(int start_fifo)
{
#if HAS_FIFO

   // setting for redlight
   fifo_fields[FIFO_SET_MINHUE] = 230;
   fifo_fields[FIFO_SET_MAXHUE] = 0;
   fifo_fields[FIFO_SET_ONLY_BW] = 0;
   fifo_fields[FIFO_SET_RM_BW] = 0;
   fifo_fields[FIFO_SET_WANT_B] = 0;
   fifo_fields[FIFO_SET_BLOB] = 0;
   fifo_fields[FIFO_SET_MINBLOB] = 10;
   fifo_fields[FIFO_SET_NUMSEG] = 2;

   // these are the settings for the pipe
   fifo_fields[FIFO_SET_MINHUE] = 15;
   fifo_fields[FIFO_SET_MAXHUE] = 35;
   fifo_fields[FIFO_SET_ONLY_BW] = 0;
   fifo_fields[FIFO_SET_RM_BW] = 0;
   fifo_fields[FIFO_SET_WANT_B] = 0;
   fifo_fields[FIFO_SET_BLOB] = 0;
   fifo_fields[FIFO_SET_MINBLOB] = 10;

   if (start_fifo)
      init_server_fifos();
#endif
}

#define GETCH {printf("press enter\n"); getchar(); }

void service_fifo_client()
{
#if HAS_FIFO

   int packet_code, num_bytes;
   char* rep = get_client_query(&num_bytes, &packet_code);

   if (packet_code == -1)
      return;
   else
   {
      printf("Got fifo packet_code %d, bytes %d\n", packet_code, num_bytes);

      switch (packet_code)
      {
      case FIFO_GET_LINES:
      {
         // compose a reply and send it
         char reply[2000];
         char* payload = server_pack_lines( (short*)vision_result_lines,
                                            num_vision_result_lines);
         sprintf(reply,"%d %d\n%s", strlen(payload), FIFO_GET_LINES, payload);
         printf("sending:lines %s\n", reply);
         send_to_client(reply);
         break;
      }
      case FIFO_GET_BLOB:
      {
         // compose a reply and send it
         printf("returning blob to client\n");
         char reply[200];
         char *payload = server_pack_blob( blob_size, blob_x, blob_y, blob_seen);

         sprintf(reply,"%d %d\n%s", strlen(payload), FIFO_GET_BLOB, payload);
         send_to_client(reply);
         break;
      }
      case FIFO_SET:
      {
         // read the command and assign it to the matching variable
         int index, value;
         int found = sscanf(rep,"%d %d", &index, &value);
         assert(found == 2);

         printf("******** SETTING field %d to %d\n", index, value);
         fifo_fields[index] = value;

         //getchar();

         break;
      }
      }
      //if(packet_code != FIFO_SET)
      //GETCH;
   }

#endif
}
