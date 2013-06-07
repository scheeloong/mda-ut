#include "ActuatorOutput.h"
#include "SimulatorSingleton.h"
#include "physical_model.h"

ActuatorOutputSimulator::ActuatorOutputSimulator()
{
  SimulatorSingleton::get_instance().register_instance();
}

ActuatorOutputSimulator::~ActuatorOutputSimulator()
{
}

bool ActuatorOutputSimulator::set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir, int delta)
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
  int current_yaw, target_yaw, current_depth, target_depth;
  switch(dir) {
    case FORWARD:
    case REVERSE:
      set_attitude_absolute(SPEED, delta); // doesn't make sense to change the speed, set absolute
      return true;
    case RIGHT:
    case LEFT:
      set_attitude_absolute(SPEED, 0); // stop forward speed
      // Only set yaw change if yaw is stable
      current_yaw = SimulatorSingleton::get_instance().attitude().angle.yaw;
      target_yaw = SimulatorSingleton::get_instance().target_yaw();
      if (abs(current_yaw - target_yaw) <= stable_yaw_threshold || abs(current_yaw - target_yaw) >= 360 - stable_yaw_threshold) {
        SimulatorSingleton::get_instance().set_target_attitude_change(delta, 0);
        return true;
      } else {
        return false;
      }
    case RISE:
    case SINK:
      set_attitude_absolute(SPEED, 0); // stop forward speed
      // Only set depth change if depth is stable
      current_depth = (int)(100 * SimulatorSingleton::get_instance().attitude().position.y);
      target_depth = SimulatorSingleton::get_instance().target_depth();
      if (abs(current_depth - target_depth) <= stable_depth_threshold) {
        SimulatorSingleton::get_instance().set_target_attitude_change(0, delta);
        return true;
      } else {
        return false;
      }
    default:
      return false;
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

int ActuatorOutputSimulator::get_target_attitude(ATTITUDE_DIRECTION dir)
{
  switch(dir) {
    case YAW:
      return SimulatorSingleton::get_instance().target_yaw();
    case DEPTH:
      return SimulatorSingleton::get_instance().target_depth();
      break;
    default:
      puts("Unsupported functionality, returning 0\n");
      return 0;
  }
}

void ActuatorOutputSimulator::stop()
{
  set_attitude_absolute(SPEED, 0);
}

#define DELTA_MOVE 0.15
#define DELTA_TURN 2.5

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
      o.yaw = -DELTA_TURN;
      SimulatorSingleton::get_instance().add_orientation(o);
      break;
    case SIM_MOVE_RIGHT:
      o.yaw = DELTA_TURN;
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
