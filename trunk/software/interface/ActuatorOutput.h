/* ActuatorOutput interface.

   This interface represents the limbs of the submarine. It provides an interface
   to actuate the submarine.
*/

#ifndef ACTUATOR_OUTPUT_H
#define ACTUATOR_OUTPUT_H

/* Actuator Output interface */
class ActuatorOutput {
  public:
    virtual ~ActuatorOutput() {}

    // some methods to actuate the output
};

/* A don't care implementation */
class ActuatorOutputNull : public ActuatorOutput {
  public:
    virtual ~ActuatorOutputNull() {}
};

/* Simulator implementation */
class ActuatorOutputSimulator : public ActuatorOutput {
  public:
    ActuatorOutputSimulator();
    virtual ~ActuatorOutputSimulator();
};

/* The real submarine implementation */
class ActuatorOutputSubmarine : public ActuatorOutput {
  public:
    ActuatorOutputSubmarine();
    virtual ~ActuatorOutputSubmarine();
};

#endif
