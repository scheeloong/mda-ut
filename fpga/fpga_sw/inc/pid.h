// controller_new.h - temporary header for embedded control code
#ifndef _PID_H
#define _PID_H

#define PID_NUM_OLD_VALUES 3
typedef double HW_NUM;

typedef struct {
    // everything is here is supposed to be private
    // but for setting up the constants we'll access the members directly
    HW_NUM P, I, D;
    HW_NUM Const_P, Const_I, Const_D;
    float Alpha; // decay constant for integral term
    
    int num_values; // number of values read
    HW_NUM old_values[PID_NUM_OLD_VALUES];
} Controller_PID;

inline void PID_Reset (Controller_PID* PID);
void PID_Update (Controller_PID* PID, HW_NUM value);
inline HW_NUM PID_Output (Controller_PID* PID);

#endif
