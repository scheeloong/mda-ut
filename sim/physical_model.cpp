#include <math.h>
#include "physical_model.h"
#include <stdio.h>

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
}

void physical_model::reset_pos()
{
   position.x = REF_X;
   position.y = REF_Y;
   position.z = REF_Z;
}

physical_model::physical_model(float x, float y, float z)
{
   // simulator coordinates
   position.x = x;
   position.y = y;
   position.z = z;
}

physical_model::~physical_model()
{
}

#define DEPTH_SPEED_SCALING 500000.0
#define ROTATIONSPEED .0001
#define FWD_SPEED_SCALING 50000.0
#define SIDE_SPEED_SCALING 50000.0
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
   float distance_traveled = speed * delta_time/ FWD_SPEED_SCALING;

   position.x = position.x + sin(angle.yaw * M_PI/180) * distance_traveled;
   position.z = position.z - cos(angle.yaw * M_PI/180) * distance_traveled;


   float dy = 0;
   position.y = position.y + dy * delta_time/DEPTH_SPEED_SCALING;

   float side_traveled = 0 * delta_time/ SIDE_SPEED_SCALING;
   position.x = position.x + sin((angle.yaw + 90) * M_PI/180) * side_traveled;
   position.z = position.z - cos((angle.yaw + 90) * M_PI/180) * side_traveled;

#if ADD_NOISE
   int ind = (cur_pos % 2);
   if (cur_pos % 2)
      angle.roll += noise_sign[ind]*anim_noise[cur_pos];
   else
      angle.pitch += noise_sign[ind]*anim_noise[cur_pos];
   noise_sign[ind] = -noise_sign[ind];
#endif
}
