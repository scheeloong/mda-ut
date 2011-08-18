#include "types.h"
#include "protocol.h"
#include "physical_model.h"

physical_model model;

extern bool verbose_camera;

extern world_vector position;	/* should be deleted*/
extern orientation angle;	/* should be deleted*/

bool lights_on;


/* this is the interface of the simulator with the real world */

void update_model(long delta_time)
{
   model.update(delta_time);

   // print any change
   if ((position.x != model.position.x) ||
         (position.y != model.position.y) ||
         (position.z != model.position.z) ||
         (angle.roll != model.angle.roll) ||
         (angle.pitch != model.angle.pitch) ||
         (angle.yaw != model.angle.yaw) )
   {
      model.print();

#warning deactivating speed for debugging purposes
      CMD_FWD_SPEED_store = 0;
      CMD_SIDE_SPEED_store = 0;
      verbose_camera = true;
   }


   position = model.position;
   angle = model.angle;

#if BLINK
   lights_on = !lights_on;
#endif
}

void aaa()
{
   verbose_camera = true;
   printf("resuming\n");
}

void reset_pos()
{
   model.reset_pos();
}

void reset_angle()
{
   model.reset_angle();
}
void update_angle_from_model()
{
   angle = model.angle;
}
void update_pos_from_model()
{
   position = model.position;
}
