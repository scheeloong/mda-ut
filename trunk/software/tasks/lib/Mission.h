#ifndef MISSION_H
#define MISSION_H

#include "../../interface/Operation.h"

/* The real submarine implementation */
class Mission : public Operation {
  public:
    virtual ~Mission();

    virtual void work();
};

#endif
