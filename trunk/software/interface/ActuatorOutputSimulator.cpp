#include "ActuatorOutput.h"
#include "SimulatorSingleton.h"
#include "physical_model.h"

ActuatorOutputSimulator::ActuatorOutputSimulator()
{
  SimulatorSingleton::get_instance().increment_instances();
  // no need to register anything with the singleton
}

ActuatorOutputSimulator::~ActuatorOutputSimulator()
{
  SimulatorSingleton::get_instance().decrement_instances();
}

void ActuatorOutputSimulator::set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir, int delta)
{
  // TODO: implement
}

void ActuatorOutputSimulator::set_attitude_absolute(ATTITUDE_DIRECTION dir, int val)
{
  // TODO: implement
}

#define DELTA_MOVE 0.15

void ActuatorOutputSimulator::special_cmd(SPECIAL_COMMAND cmd)
{
  world_vector p = {};
  orientation o = {};
  physical_model model;

  switch(cmd) {
    case SIM_ACCEL_FWD:
      SimulatorSingleton::get_instance().add_acceleration(1, 0, 0);
      break;
    case SIM_ACCEL_REV:
      SimulatorSingleton::get_instance().add_acceleration(-1, 0, 0);
      break;
    case SIM_ACCEL_LEFT:
      SimulatorSingleton::get_instance().add_acceleration(0, -1, 0);
      break;
    case SIM_ACCEL_RIGHT:
      SimulatorSingleton::get_instance().add_acceleration(0, 1, 0);
      break;
    case SIM_ACCEL_RISE:
      SimulatorSingleton::get_instance().add_acceleration(0, 0, 1);
      break;
    case SIM_ACCEL_SINK:
      SimulatorSingleton::get_instance().add_acceleration(0, 0, -1);
      break;
    case SIM_ACCEL_ZERO:
      model = SimulatorSingleton::get_instance().attitude();
      SimulatorSingleton::get_instance().add_acceleration(-model.accel, -model.angular_accel, -model.depth_accel);
      break;
    case SIM_MOVE_FWD:
      model = SimulatorSingleton::get_instance().attitude();
      o = model.angle;
      p.z -= DELTA_MOVE*cos((o.yaw*PI)/180)*cos((o.pitch*PI)/180);
      p.x += DELTA_MOVE*sin((o.yaw*PI)/180)*cos((o.pitch*PI)/180);
      p.y -= DELTA_MOVE*sin((o.pitch*PI)/180);
      SimulatorSingleton::get_instance().add_position(p);
      break;
    case SIM_MOVE_REV:
      model = SimulatorSingleton::get_instance().attitude();
      o = model.angle;
      p.z += DELTA_MOVE*cos((o.yaw*PI)/180)*cos((o.pitch*PI)/180);
      p.x -= DELTA_MOVE*sin((o.yaw*PI)/180)*cos((o.pitch*PI)/180);
      p.y += DELTA_MOVE*sin((o.pitch*PI)/180);
      SimulatorSingleton::get_instance().add_position(p);
      break;
    case SIM_MOVE_LEFT:
      o.yaw = -DELTA_MOVE;
      SimulatorSingleton::get_instance().add_orientation(o);
      break;
    case SIM_MOVE_RIGHT:
      o.yaw = DELTA_MOVE;
      SimulatorSingleton::get_instance().add_orientation(o);
      break;
    case SIM_MOVE_RISE:
      p.y = DELTA_MOVE;
      SimulatorSingleton::get_instance().add_position(p);
      break;
    case SIM_MOVE_SINK:
      p.y = -DELTA_MOVE;
      SimulatorSingleton::get_instance().add_position(p);
      break;
    default:
      break;
  }
}
