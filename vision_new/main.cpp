/** The main function is just used to test various parts of vision. Rewrite
 *  as needed 
 */
#include <stdlib.h>
#include <stdio.h>

#include "lib/mv.h"
#include "lib/mgui.h"

int main () {
    char filename[] = "settings/settings.msf";
    float S1, S2;
    
    read_mv_setting (filename, "S1", S1);
    read_mv_setting (filename, "S2", S2);
    
    printf ("S1: %f\nS2: %f\n", S1, S2);
                      
    return 0;
}