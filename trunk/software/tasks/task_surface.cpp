#include "mda_tasks.h"

MDA_TASK_SURFACE:: MDA_TASK_SURFACE (AttitudeInput* a, ImageInput* i, ActuatorOutput* o) :
    MDA_TASK_BASE (a, i, o)
{
}


MDA_TASK_SURFACE:: ~MDA_TASK_SURFACE ()
{
}

MDA_TASK_RETURN_CODE MDA_TASK_SURFACE:: run_task() {
    set(DEPTH, MDA_TASK_BASE::starting_depth);

    return TASK_DONE;
}
