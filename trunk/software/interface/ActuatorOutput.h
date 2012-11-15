/* ActuatorOutput interface.

   This interface represents the limbs of the submarine. It provides an interface
   to actuate the submarine.
*/

#ifndef ACTUATOR_OUTPUT_H
#define ACTUATOR_OUTPUT_H

enum ATTITUDE_CHANGE_DIRECTION {
  FORWARD,
  REVERSE,
  LEFT,
  RIGHT,
  SINK,
  RISE
};

enum ATTITUDE_DIRECTION {
  SPEED,
  YAW,
  DEPTH
};

enum SPECIAL_COMMAND {
  SUB_POWER_ON,
  SUB_POWER_OFF,
  SIM_MOVE_FWD,
  SIM_MOVE_REV,
  SIM_MOVE_LEFT,
  SIM_MOVE_RIGHT,
  SIM_MOVE_RISE,
  SIM_MOVE_SINK,
  SIM_ACCEL_ZERO,
  SIM_RESET_POS,
  SIM_RESET_ALL
};

#define DEFAULT_ATTITUDE_CHANGE 1

/* Actuator Output interface */
class ActuatorOutput {
  public:
    virtual ~ActuatorOutput() {}

    // some methods to actuate the output
    virtual void set_attitude_change(ATTITUDE_CHANGE_DIRECTION, int) = 0;
    virtual void set_attitude_absolute(ATTITUDE_DIRECTION, int) = 0;
    virtual void set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir) {set_attitude_change(dir, DEFAULT_ATTITUDE_CHANGE);}
    virtual void stop() = 0;
    virtual void special_cmd(SPECIAL_COMMAND) = 0;
};

/* A don't care implementation */
class ActuatorOutputNull : public ActuatorOutput {
  public:
    virtual ~ActuatorOutputNull() {}

    virtual void set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir, int delta) {}
    virtual void set_attitude_absolute(ATTITUDE_DIRECTION dir, int val) {}
    virtual void stop() {}
    virtual void special_cmd(SPECIAL_COMMAND cmd) {}
};

/* Simulator implementation */
class ActuatorOutputSimulator : public ActuatorOutput {
  public:
    ActuatorOutputSimulator();
    virtual ~ActuatorOutputSimulator();

    virtual void set_attitude_change(ATTITUDE_CHANGE_DIRECTION, int);
    virtual void set_attitude_absolute(ATTITUDE_DIRECTION, int);
    virtual void stop();
    virtual void special_cmd(SPECIAL_COMMAND);
};

/* The real submarine implementation */
class ActuatorOutputSubmarine : public ActuatorOutput {
  public:
    ActuatorOutputSubmarine();
    virtual ~ActuatorOutputSubmarine();

    virtual void set_attitude_change(ATTITUDE_CHANGE_DIRECTION, int);
    virtual void set_attitude_absolute(ATTITUDE_DIRECTION, int);
    virtual void stop();
    virtual void special_cmd(SPECIAL_COMMAND);
};

#endif
