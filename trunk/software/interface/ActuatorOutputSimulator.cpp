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
  switch(dir) {
    case REVERSE:
    case LEFT:
    case SINK:
      delta *= -1;
      break;
    default:
      break;
  }
  switch(dir) {
    case FORWARD:
    case REVERSE:
      set_attitude_absolute(SPEED, delta); // doesn't make sense to change the speed, set absolute
      break;
    case RIGHT:
    case LEFT:
      SimulatorSingleton::get_instance().set_target_attitude_change(delta, 0);
      set_attitude_absolute(SPEED, 0); // stop forward speed
      break;
    case RISE:
    case SINK:
      SimulatorSingleton::get_instance().set_target_attitude_change(0, delta);
      set_attitude_absolute(SPEED, 0); // stop forward speed
      break;
  }
}

void ActuatorOutputSimulator::set_attitude_absolute(ATTITUDE_DIRECTION dir, int val)
{
  switch(dir) {
    case SPEED:
      SimulatorSingleton::get_instance().set_target_accel(val);
      break;
    case YAW:
      SimulatorSingleton::get_instance().set_target_yaw(val);
      break;
    case DEPTH:
      SimulatorSingleton::get_instance().set_target_depth(val/(float)100);
      break;
  }
}

void ActuatorOutputSimulator::stop()
{
  set_attitude_absolute(SPEED, 0);

  physical_model m = SimulatorSingleton::get_instance().attitude();
  float yaw = m.angle.yaw, depth = m.position.y * 100;

  set_attitude_absolute(YAW, yaw);
  set_attitude_absolute(DEPTH, depth);
}

#define DELTA_MOVE 0.15

void ActuatorOutputSimulator::special_cmd(SPECIAL_COMMAND cmd)
{
  world_vector p = {};
  orientation o = {};
  physical_model model;

  switch(cmd) {
    case SIM_ACCEL_ZERO:
      model = SimulatorSingleton::get_instance().attitude();
      SimulatorSingleton::get_instance().zero_speed();
      break;
    case SIM_MOVE_FWD:
      model = SimulatorSingleton::get_instance().attitude();
      o = model.angle;
      p.z -= DELTA_MOVE*cos((o.yaw*PI)/180);
      p.x += DELTA_MOVE*sin((o.yaw*PI)/180);
      SimulatorSingleton::get_instance().add_position(p);
      break;
    case SIM_MOVE_REV:
      model = SimulatorSingleton::get_instance().attitude();
      o = model.angle;
      p.z += DELTA_MOVE*cos((o.yaw*PI)/180);
      p.x -= DELTA_MOVE*sin((o.yaw*PI)/180);
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
