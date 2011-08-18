#include "protocol.h"
#include <iostream>
#include <assert.h>
#include <stdlib.h>

using namespace std;

//bool got_conn = false;
CSocket * pSocket = NULL;
CSocket *new_socket= NULL;

void (*post_verb_handler)(cmd_type) = post_verb_send;

void get_connection()
{
   while (1)
   {
      cout << "Waiting for client to connect\n";
      new_socket = pSocket->Accept();
      if ( !new_socket)
         continue;
      got_conn = true;
      break;
   }
}


#if WIN_SERV
DWORD WINAPI create_server(  LPVOID pdata)
#elif LIN_SERV
void* create_server(  void* pdata)
#endif
{
   cout << "Opening listen socket on port " << SIM_PORT;
   try
   {
      pSocket = new CSocket(SIM_PORT);
      cout << " : Done\n";
   }
   catch ( CSocketException se)
   {
      cout << " : Exception => " << se.getText() << "\n";
      exit(0);
      return NULL;
   }
   cout << "Entering server mode\n";
   cout.flush();

   while (1)
   {
      get_connection();
      cout << "Got connection\n";
      while (got_conn)
      {
         if (listen_to_client(new_socket) == CMD_PUT)
         {
            //update_model is already bound to the idle function
         }
      };
      cout << "Lost connection\n";
      got_conn = false;
   }
   return 0;
}

void terminate_server()
{
   delete pSocket;
}

#include <GL/glut.h>

extern int window_width, window_height;
GLubyte *pixels_cpy = NULL;
GLubyte *buf_out    = NULL;
int cur_pix_size=0;


// setup the protocol handlers
int dum=0;
int* img_dimx = &window_width, *img_dimy = &window_height;
int* img_encoding = &dum;

unsigned char** orig_img = &pixels_cpy;
int* orig_img_size = &cur_pix_size;

extern void pre_image_send(cmd_type cmd);

void (*pre_image_handler)(cmd_type) = pre_image_send;
void (*post_image_handler)(cmd_type)= NULL;
void (*post_heading_handler)(cmd_type)= post_cmd_heading_recv_make_absolute;
