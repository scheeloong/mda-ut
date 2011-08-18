#include <math.h>
#include "physical_model.h"
#include <stdio.h>
#include "protocol.h"

// initial position and angle
#include "init.h"

physical_model::physical_model()
{
   // on top of a box
   reset_angle();
   reset_pos();
};

void physical_model::reset_angle()
{
   angle.pitch = A_P;
   angle.yaw = A_Y;
   angle.roll = A_R;
   CMD_HEADING_store = A_Y;
   CUR_HEADING_store = A_Y;
   if (A_P == 90)
      CMD_CAMERA_store = 1;
}

void physical_model::reset_pos()
{
   position.x = REF_X;
   position.y = REF_Y;
   position.z = REF_Z;
   CUR_DEPTH_store = REF_Y;
   CMD_DEPTH_store = REF_Y;
}

physical_model::physical_model(float x, float y, float z)
{
   // simulator coordinates
   this->position.x = x;
   this->position.y = y;
   this->position.z = z;
}

physical_model::~physical_model()
{
}

#define DEPTH_SPEED_SCALING 500000.0
#define ROTATIONSPEED .0001
#define FWD_SPEED_SCALING 5000000.0
#define SIDE_SPEED_SCALING 500000.0
void range_angle_int(int& angle)
{
   if (angle >= 180)
      angle -= 360;
   else if (angle <= -180)
      angle += 360;
}

// time past since last iteration in seconds
void physical_model::update(long delta_time)
{
   //  printf("DELTA : %f\n",delta_time);
   float distance_traveled = CMD_FWD_SPEED_store * delta_time/ FWD_SPEED_SCALING;

   position.x = position.x + sin(CUR_HEADING_store * M_PI/180) * distance_traveled;
   position.z = position.z - cos(CUR_HEADING_store * M_PI/180) * distance_traveled;


   float dz = CMD_DEPTH_store-CUR_DEPTH_store;
   position.y = position.y + dz * delta_time/DEPTH_SPEED_SCALING;

   if ((dz > 0) && (position.y > CMD_DEPTH_store))
      position.y = CMD_DEPTH_store;
   else if ((dz < 0) && (position.y < CMD_DEPTH_store))
      position.y = CMD_DEPTH_store;

   CUR_DEPTH_store = position.y;

   float side_traveled = CMD_SIDE_SPEED_store * delta_time/ SIDE_SPEED_SCALING;
   position.x = position.x + sin((CUR_HEADING_store+90) * M_PI/180) * side_traveled;
   position.z = position.z - cos((CUR_HEADING_store+90) * M_PI/180) * side_traveled;

   int absolute_cmd = CMD_HEADING_store; // was made absolute by a protocol hook in server.cpp
   range_angle_int(absolute_cmd);
   /*
     printf("current %d cmd %d abs_cmd %d\n",
     CUR_HEADING_store, CMD_HEADING_store, absolute_cmd);
   */

   if (absolute_cmd != CUR_HEADING_store)
   {
      float time_to_rotate = (absolute_cmd - CUR_HEADING_store)/ROTATIONSPEED;
      if (delta_time < time_to_rotate)
      {
         float inc = (delta_time / time_to_rotate)*(absolute_cmd - CUR_HEADING_store);
         int iinc = (int)(inc + CUR_HEADING_store);
         CUR_HEADING_store = (iinc < 1)? ((inc>0)?1:-1) : iinc;
         printf("time to rotate %f delta %d yaw inc %f yaw %d yaw prev %f, iinc %d\n",
                time_to_rotate, (int)delta_time, inc, CUR_HEADING_store, angle.yaw, iinc);
      }
      else
         CUR_HEADING_store = absolute_cmd;

      angle.yaw = CUR_HEADING_store;
   }
/* RZ
   if (!is_interactive){
      angle.pitch = (CMD_CAMERA_store==0)? 0: 90;
      printf ("angle.pitch=%d\n",angle.pitch);
   }
   */
   CUR_CAMERA_store = CMD_CAMERA_store;

#if ADD_NOISE
   int ind = (cur_pos % 2);
   if (cur_pos % 2)
      angle.roll += noise_sign[ind]*anim_noise[cur_pos];
   else
      angle.pitch += noise_sign[ind]*anim_noise[cur_pos];
   noise_sign[ind] = -noise_sign[ind];
#endif


   if (delta_time == 1)
      printf("dist %f, new pos: %f %f %f\n",
             distance_traveled, position.x, position.y, position.z);
}


void physical_model::print()
{
   printf("CUR_CAMERA %d (= 0 for forward camera)\n", CUR_CAMERA_store);
   printf("CMD_HEADING %d\tCUR_HEADING %d\n", CMD_HEADING_store, CUR_HEADING_store);
   printf("CMD_DEPTH %f\tCUR_DEPTH %f\n", CMD_DEPTH_store, CUR_DEPTH_store);
   printf("CMD_SIDE_SPEED %f\tCMD_FWD_SPEED %f\n", CMD_SIDE_SPEED_store, CMD_FWD_SPEED_store);
}

void update_readings()
{
   CUR_HEADING_store = 0;
   CUR_DEPTH_store   = 0;
}

