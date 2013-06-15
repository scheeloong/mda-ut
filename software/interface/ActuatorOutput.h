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
  SUB_STARTUP_SEQUENCE,
  SUB_POWER_OFF,
  SIM_MOVE_FWD,
  SIM_MOVE_REV,
  SIM_MOVE_LEFT,
  SIM_MOVE_RIGHT,
  SIM_MOVE_RISE,
  SIM_MOVE_SINK,
  SIM_ACCEL_ZERO
};

#define DEFAULT_ATTITUDE_CHANGE 1

/* Actuator Output interface */
class ActuatorOutput {
  public:
    virtual ~ActuatorOutput() {}

    // some methods to actuate the output
    virtual bool set_attitude_change(ATTITUDE_CHANGE_DIRECTION, int) = 0; // return if attitude was actually changed (won't change unless the attitude is stable)
    virtual void set_attitude_absolute(ATTITUDE_DIRECTION, int) = 0;
    virtual int get_target_attitude(ATTITUDE_DIRECTION) = 0;
    virtual void stop() = 0;
    virtual void special_cmd(SPECIAL_COMMAND) = 0;

    bool set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir) { return set_attitude_change(dir, DEFAULT_ATTITUDE_CHANGE); }
  protected:
    static const int stable_yaw_threshold = 5;
    static const int stable_depth_threshold = 15;
};

/* A don't care implementation */
class ActuatorOutputNull : public ActuatorOutput {
  public:
    virtual ~ActuatorOutputNull() {}

    virtual bool set_attitude_change(ATTITUDE_CHANGE_DIRECTION dir, int delta) { return false; }
    virtual void set_attitude_absolute(ATTITUDE_DIRECTION dir, int val) {}
    virtual int get_target_attitude(ATTITUDE_DIRECTION) { return 0; }
    virtual void stop() {}
    virtual void special_cmd(SPECIAL_COMMAND cmd) {}
};

/* Simulator implementation */
class ActuatorOutputSimulator : public ActuatorOutput {
  public:
    ActuatorOutputSimulator();
    virtual ~ActuatorOutputSimulator();

    virtual bool set_attitude_change(ATTITUDE_CHANGE_DIRECTION, int);
    virtual void set_attitude_absolute(ATTITUDE_DIRECTION, int);
    virtual int get_target_attitude(ATTITUDE_DIRECTION);
    virtual void stop();
    virtual void special_cmd(SPECIAL_COMMAND);
};

/* The real submarine implementation */
class ActuatorOutputSubmarine : public ActuatorOutput {
  public:
    ActuatorOutputSubmarine();
    virtual ~ActuatorOutputSubmarine();

    virtual bool set_attitude_change(ATTITUDE_CHANGE_DIRECTION, int);
    virtual void set_attitude_absolute(ATTITUDE_DIRECTION, int);
    virtual int get_target_attitude(ATTITUDE_DIRECTION);
    virtual void stop();
    virtual void special_cmd(SPECIAL_COMMAND);

    // Specific commands
    void set_depth_constants(double, double, double, double);
    void set_pitch_constants(double, double, double, double);
    void set_roll_constants(double, double, double, double);
    void set_yaw_constants(double, double, double, double);
};

#endif
