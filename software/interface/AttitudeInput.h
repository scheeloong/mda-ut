/* AttitudeInput interface.

   This interface represents the input from the submarine's surroundings,
   most importantly its attitude: yaw, pitch, roll, depth.

   This interface may also provide the target attitude.
*/

#ifndef ATTITUDE_INPUT_H
#define ATTITUDE_INPUT_H

/* Attitude Input interface */
class AttitudeInput {
  public:
    virtual ~AttitudeInput();

    virtual int yaw() = 0;
    virtual int pitch() = 0;
    virtual int roll() = 0;
    virtual int depth() = 0;

    virtual int target_yaw() { return 0;}
    virtual int target_pitch() { return 0;}
    virtual int target_roll() { return 0;}
    virtual int target_depth() { return 0;}
};

/* A don't care implementation */
class AttitudeInputNull : public AttitudeInput {
  public:
    virtual int yaw() { return 0;}
    virtual int pitch() { return 0;}
    virtual int roll() { return 0;}
    virtual int depth() { return 0;}
}

/* Simulator implementation */
class AttitudeInputSimulator : public AttitudeInput {
  public:
    AttitudeInputSimulator();
    virtual ~AttitudeInputSimulator();

    virtual int yaw();
    virtual int pitch();
    virtual int roll();
    virtual int depth();
}

/* The real submarine implementation */
class AttitudeInputSubmarine : public AttitudeInput {
  public:
    AttitudeInputSubmarine();
    virtual ~AttitudeInputSubmarine();

    virtual int yaw();
    virtual int pitch();
    virtual int roll();
    virtual int depth();

    virtual int target_yaw();
    virtual int target_pitch();
    virtual int target_roll();
    virtual int target_depth();
}

#endif
