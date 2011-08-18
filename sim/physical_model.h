#ifndef _PHYSICALMODEL_H
#define _PHYSICALMODEL_H

#include "types.h"

class physical_model
{
public:
   world_vector position;
   orientation angle;

public:
   physical_model();
   physical_model(float x, float y, float z);
   ~physical_model();
   void update(long delta_time);
   void reset_angle();
   void reset_pos();
   void print();
};
#endif
