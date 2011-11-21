#include "types.h"
#include "protocol.h"
#include "physical_model.h"

physical_model model;

extern bool verbose_camera;

extern world_vector position;	/* should be deleted*/
extern orientation angle;	/* should be deleted*/

/* this is the interface of the simulator with the real world */

void init_model()
{
   model.update(0);
   position = model.position;
   angle = model.angle;
}

void update_model(long delta_time)
{
   model.position = position;
   model.update(delta_time);
   position = model.position;
   angle = model.angle;
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

void set_model(int speed)
{
   CMD_FWD_SPEED_store = (float)speed;
   CMD_HEADING_store = (int)angle.yaw;
}

bool is_stationary()
{
   return (CMD_FWD_SPEED_store == 0);
}
