#ifndef AQUATUX_H
#define AQUATUX_H

#include "AttitudeInput.h"
#include "ImageInput.h"
#include "Operation.h"
#include "ActuatorOutput.h"

class AquaTux {
  public:
    AquaTux(const char *);
    void work();
    ~AquaTux();
  private:
    AttitudeInput *m_attitude_input;
    ImageInput *m_image_input;
    Operation *m_operation;
    ActuatorOutput *m_actuator_output;
};

#endif
