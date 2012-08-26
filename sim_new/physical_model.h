#ifndef _PHYSICALMODEL_H
#define _PHYSICALMODEL_H

#include "types.h"

#define ANGLE_INC 5.
#define POS_INC .2
#define PI 3.14159265

class physical_model
{
public:
   world_vector position;
   orientation angle;
   float speed, depth_speed, angular_speed;

public:
   physical_model();
   physical_model(float x, float y, float z);
   ~physical_model();
   void update(long delta_time);
   void reset_angle();
   void reset_pos();
   void reset_speed();
   void print();
};
#endif
