#include "../lib/mgui.h"
#include <string.h>
#include <stdio.h>

bool str_isdigit (const char str[])
// Helper function for read_mv_setting. Returns true if string is a float 
{
    unsigned len = strlen (str);
    for (unsigned i = 0; i < len; i++)
        if ((str[i] < '0' || str[i] > '9') && str[i] != '.') 
            return false;
        
    return true;
}

int read_mv_setting
/* Reads a single setting from a file. searches through file for the setting
   and puts it into &data. Returns 0 iff successful. Settings file must have
   lines consisting of either comments (beings with '#') or in the format
   SETTING_NAME, SETTING_VALUE */
(
    const char filename[], 
    const char setting_name[], 
    float &data
) 
{
    FILE* fh = fopen (filename, "r");
    if (fh == NULL) {
        fprintf (stderr, "**** Error: read_mv_setting failed to open %s\n", filename); 
        exit (1);
    }
    
    char line[LINE_LEN+1];
    char* token;
    
    // read the file line by line
    while (fgets(line, LINE_LEN, fh)) {
        // extract first token and match to setting_name
        token = strtok (line, " ,");
        if (token[0] == COMMENT_CHAR) 
            continue;

        if (!strcmp(token, setting_name)) { // found the right line
            token = strtok (NULL, " ,\n"); // get next token           
            if (!str_isdigit(token)) {
                fprintf (stderr, "**** ERROR: read_mv_setting: invalid value for %s\n", setting_name);
                exit (1);
            }
            
            data = atof (token);
            return 0;
        }
    }
    
    fprintf (stderr, "**** ERROR: read_mv_setting: setting %s not found \
             in file %s\n", setting_name, filename);
    exit (1);
}