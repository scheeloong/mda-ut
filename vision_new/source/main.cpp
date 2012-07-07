/** The main function is just used to test various parts of vision. Rewrite
 *  as needed 
 */
#include <stdlib.h>
#include <stdio.h>

#include "mv.h"
#include "mgui.h"

int main () {
    char filename[] = "settings/settings.msf";
    float S1, S2;
    
    read_mv_setting (filename, "S1", S1);
    read_mv_setting (filename, "S2", S2);
    //read_mv_setting (filename, "S3", S2);
    
    printf ("S1: %f\nS2: %f\n", S1, S2);
                      
    mvWindow win1("win1");
    mvWindow win2("win2");
    mvWindow win3("win3");
    mvWindow win4("win4");
    cvWaitKey(0);
    return 0;
}
