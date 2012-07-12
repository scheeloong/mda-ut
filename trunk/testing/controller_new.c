#include "controller_new.h"
#include "controller_settings.h"

inline
void PID_Reset (Controller_PID* PID) {
    PID->I = 0;
    PID->Const_P = 0;
    PID->Const_I = 0;
    PID->Const_D = 0;
    PID->Alpha = 0;
    PID->num_values = 0;
}

void PID_Update (Controller_PID* PID, HW_NUM value) {
    PID->P = value;
    PID->I = (1-PID->Alpha)*PID->I + value;
    
    int i; 
    for (i = PID_NUM_OLD_VALUES-1; i > 0; i--)
        PID->old_values[i] = PID->old_values[i-1];
    
    PID->old_values[0] = value;
    
    HW_NUM temp = 0;
    if (PID->num_values >= PID_NUM_OLD_VALUES-1) { // if enough values accumulated
        for (i = 1; i < PID_NUM_OLD_VALUES; i++)
            temp += PID->old_values[i-1] - PID->old_values[i];
        PID->D = (float)temp / PID_NUM_OLD_VALUES;
    }
    else {
        PID->D = 0;
        PID->num_values++; // only increment here to avoid overflow
    }
}

inline 
HW_NUM PID_Output (Controller_PID* PID) {
    return PID->Const_P*PID->P + PID->Const_I*PID->I + PID->Const_D*PID->D;    
}
        
        
        