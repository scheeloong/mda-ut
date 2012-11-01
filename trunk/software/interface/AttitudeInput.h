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
    virtual ~AttitudeInput() {}

    virtual int yaw() {return 0;}
    virtual int pitch() {return 0;}
    virtual int roll() {return 0;}
    virtual int depth() = 0;
};

/* A don't care implementation */
class AttitudeInputNull : public AttitudeInput {
  public:
    virtual ~AttitudeInputNull() {}

    virtual int depth() {return 0;}
};

/* Simulator implementation */
class AttitudeInputSimulator : public AttitudeInput {
  public:
    AttitudeInputSimulator();
    virtual ~AttitudeInputSimulator();

    virtual int yaw();
    virtual int pitch();
    virtual int roll();
    virtual int depth();
};

/* The real submarine implementation */
class AttitudeInputSubmarine : public AttitudeInput {
  public:
    AttitudeInputSubmarine();
    virtual ~AttitudeInputSubmarine();

    virtual int depth();
};

#endif
