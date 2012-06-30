/** FPAG User Inteface Utilities
 *  This file contains functions that communicate with the fpga that the main
 *  program will call.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define MAX_TOKENS 3
#define LINE_START "fpga> "

int main () {
    char cmd[51];               // stores the command that was entered
    char* token[MAX_TOKENS];    // points to parts of the command
    unsigned i;
    
    for (;;) {
        printf (LINE_START);
        cmd_ok = 0;
        fgets (cmd, 50, stdin);
        
        // use strtok to grab each part of the command 
        token[0] = strtok (cmd, " \n");
        for (i = 1; i < MAX_TOKENS; i++)
            token[i] = strtok (NULL, " \n");
        
        if (token[0] == NULL) 
            help ();
        else if (!strcmp(token[0], "q")) 
            return 0;
        else if (!strcmp(token[0], "help")) {
            if (token[1] == NULL) 
                help ();
            else if (!strcmp(token[1],"motor") || !strcmp(token[1],"m"))
                help_motor();
            else if (!strcmp(token[1],"dyn") || !strcmp(token[1],"d"))
                help_dyn();
            else if (!strcmp(token[1],"power") || !strcmp(token[1],"p"))
                help_power();
        }
        
        if (cmd_ok != 1) 
            cmd_error();
    }
}

