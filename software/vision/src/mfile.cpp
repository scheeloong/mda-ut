#include "mgui.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

bool str_isdigit (const char str[])
// Helper function for read_mv_setting. Returns true if string is a float 
{
    unsigned len = strlen (str);
    for (unsigned i = 0; i < len; i++)
        if ((str[i] < '0' || str[i] > '9') && str[i] != '.') 
            return false;
        
    return true;
}

template <typename TYPE>
void read_mv_setting (const char filename[], const char setting_name[], TYPE &data) 
/* Reads a single setting from a file. searches through file for the setting
   and puts it into &data. Returns 0 iff successful. Settings file must have
   lines consisting of either comments (beings with '#') or in the format
   SETTING_NAME, SETTING_VALUE */

{
    char settings_file[100]; // create a string to hold the filename. First byte is zero.
    settings_file[0] = 0;
    
    // attempt to read environment var MDA_SETTINGS_DIR_ENV_VAR. If successful use the read
    // value as directory base for 
    char * env_mda_settings_dir = getenv(MDA_SETTINGS_DIR_ENV_VAR);
    if (env_mda_settings_dir != NULL)
        strcpy (settings_file, env_mda_settings_dir);
    else
        strcpy (settings_file, MDA_BACKUP_SETTINGS_DIR);
    
    strcat (settings_file, filename);
        
    FILE* fh = fopen (settings_file, "r");
    if (fh == NULL) {
        fprintf (stderr, "**** Error: read_mv_setting failed to open %s\n", settings_file); 
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
            
            float temp = atof (token);
            data = TYPE(temp);
            fclose (fh);
            return;
        }
    }
    
    fprintf (stderr, "**** ERROR: read_mv_setting: setting %s not found in file %s\n", setting_name, settings_file);
    exit (1);
}
/* these lines declare the use of int,unsigned,flot of read_mv_setting */
template void read_mv_setting<int>(const char filename[], const char setting_name[], int &data);
template void read_mv_setting<unsigned>(const char filename[], const char setting_name[], unsigned &data);
template void read_mv_setting<float>(const char filename[], const char setting_name[], float &data);