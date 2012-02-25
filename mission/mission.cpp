#include "mission.h"
#include <math.h>

void Mission::move(MOVE_DIR dir, int speed)
{
   switch(dir) { // Ritchie: changed relative speeds
   case FORWARD:
      model->speed = speed;
      break;
   case REVERSE:
      model->speed = -speed;
      break;
   case RISE:
      model->depth_speed = 3*speed;
      break;
   case SINK:
      model->depth_speed = -3*speed;
      break;
   case RIGHT:
      model->angular_speed = 5*speed;
      break;
   case LEFT:
      model->angular_speed = -5*speed;
      break;
   case STOP:
      model->speed = 0;
      model->depth_speed = 0;
      model->angular_speed = 0;
      break;
   }
}

void Mission::translate(MOVE_DIR dir)
{
   switch(dir) {
   case FORWARD:
      model->position.z -= POS_INC*cos((model->angle.yaw*PI)/180)*cos((model->angle.pitch*PI)/180);
      model->position.x += POS_INC*sin((model->angle.yaw*PI)/180)*cos((model->angle.pitch*PI)/180);
      model->position.y -= POS_INC*sin((model->angle.pitch*PI)/180);
      break;
   case REVERSE:
      model->position.z += POS_INC*cos((model->angle.yaw*PI)/180)*cos((model->angle.pitch*PI)/180);
      model->position.x -= POS_INC*sin((model->angle.yaw*PI)/180)*cos((model->angle.pitch*PI)/180);
      model->position.y += POS_INC*sin((model->angle.pitch*PI)/180);
      break;
   case RISE:
      model->position.y += POS_INC;
      break;
   case SINK:
      model->position.y -= POS_INC;
      break;
   case RIGHT:
      model->angle.yaw += ANGLE_INC/2;
      break;
   case LEFT:
      model->angle.yaw -= ANGLE_INC/2;
      break;
   case STOP:
      // do nothing
      break;
   }
}
