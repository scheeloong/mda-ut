#include "controller_new.h"
#include "controller_settings.h"
#include <stdio.h>
#include <stdlib.h>

int main () {
    Controller_PID* PID = (Controller_PID*) malloc (sizeof(Controller_PID));
    PID_Reset (PID);
    PID->Const_P = CONST_P;
    PID->Const_I = CONST_I;
    PID->Const_D = CONST_D;
    PID->Alpha = ALPHA;
    
    printf ("Const_P = %f\nConst_I = %f\nConst_D = %f\nAlpha = %f\n",PID->Const_P,PID->Const_I,PID->Const_D,PID->Alpha);
    
    HW_NUM temp;
    int i;
    
    while (1) {
        printf ("Enter a number: ");
        scanf ("%f", &temp); // this needs to be changed if HW_NUM becomes integer
        
        PID_Update (PID, temp);
        printf ("P=%f  I=%f  D=%f\t\tOld: ", PID->P,PID->I,PID->D);
        for (i = 0; i < PID_NUM_OLD_VALUES; i++)
            printf ("%f ", PID->old_values[i]);
        printf ("\n");
        
        temp = PID_Output (PID);
        printf ("Output: %f\n", temp);
    }
}