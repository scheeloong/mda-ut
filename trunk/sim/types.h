/**
* @file mda2009/sim/types.h
*
* @brief Contains all the constants and data structures for the underwater sim
*
* Serves as a header for .cpp files without headers
*/

// structures
#include "machine.h"

#define LONGV

#define BLINK 1

#ifndef MATH_TYPES_GL_H__
#define MATH_TYPES_GL_H__

// CIF format
// http://fr.wikipedia.org/wiki/QCIF
#define WINDOW_SIZE_X 352
#define WINDOW_SIZE_Y 288

/** Flag for whether the program is in interactive mode*/
extern char is_interactive;

/** Random number to modify the sim layout*/
extern unsigned int randNum;

/** Current sub position in the sim*/
typedef struct
{
   float x;
   float y;
   float z;
} world_vector;

/** Sub orientation in the sim*/
typedef struct
{
   float pitch;	/* spin around x axis in degrees */
   float yaw;		/* spin around y axis in degrees */
   float roll;		/* spin around z axis in degrees */
} orientation;


// for site.cpp
void init_fog();
void init_site();
void draw();
void destroy();

// for main.cpp
void screenshot();
void set_camera();

// for glQuaternion.cpp
void quat_camera(float roll, float pitch, float yaw);

// for server.cpp
#if WIN_SERV
DWORD WINAPI create_server(  LPVOID pdata);
#elif LIN_SERV
void* create_server(  void* pdata);

extern pthread_mutex_t mutex1;
extern pthread_cond_t cond;
extern int waiting_for_redraw;

#endif
void terminate_server();


// for virtual_io.cpp
void update_model(long delta_time);
void reset_angle();
void reset_pos();
void update_angle_from_model();
void update_pos_from_model();

// for main.cpp
void range_angle(int& angle);

#endif

