#ifndef MISSION_H
#define MISSION_H

#include "../interface/Operation.h"

/* The real submarine implementation */
class Mission : public Operation {
  public:
    Mission(AttitudeInput *a, ImageInput *i, ActuatorOutput *o) : Operation(a, i, o) {}
    virtual ~Mission();

    virtual void work();
  private:
    bool startup();
};
#endif
