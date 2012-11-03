#ifndef MISSION_H
#define MISSION_H

#include "../interface/Operation.h"
#include "mda_tasks.h"

/* The real submarine implementation */
class Mission : public Operation {
  public:
    virtual ~Mission();

    virtual void work();
};

class SingleTaskMission: public Operation {
  MDA_TASK_BASE* task;

  public:
    SingleTaskMission (MDA_TASK task_enum);
    ~SingleTaskMission();
    void work();  
};
#endif
