#ifndef MISSION_H
#define MISSION_H

#include "../interface/Operation.h"
#include "mda_tasks.h"

/* The real submarine implementation */
class Mission : public Operation {
  public:
    Mission(AttitudeInput *a, ImageInput *i, ActuatorOutput *o) : Operation(a, i, o) {}
    virtual ~Mission();

    virtual void work();
};

class SingleTaskMission: public Operation {
  MDA_TASK_BASE* task;

  public:
    SingleTaskMission (MDA_TASK task_enum);
    SingleTaskMission(AttitudeInput *, ImageInput *, ActuatorOutput *, MDA_TASK);
    ~SingleTaskMission();
    void work();  
};
#endif
