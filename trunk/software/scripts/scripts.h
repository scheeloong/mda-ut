#ifndef __MDA_SCRIPTS_H__
#define __MDA_SCRIPTS_H__

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../fpga/ui/utils.h"

inline void spawn_nios2_term()
{
  char nios2_shell_path[128];
  FILE *fp;

  fp = popen("which nios2-terminal", "r");
  fscanf(fp, "%127s", nios2_shell_path);
  pclose(fp);

  if (nios2_shell_path[0] != '/') {
    puts("nios2-terminal is not on your path, exiting");
    exit(1);
  }

  spawn_term(nios2_shell_path);
}

inline void int_handler(int signal)
{
  // Calling the int_handler will kill the child process (nios2-terminal)
  // Respawn it, then exit_safe
  spawn_nios2_term();

  exit_safe();
  exit(0);
}

inline void init()
{
  // Fork a nios2-terminal for communication
  spawn_nios2_term();

  // Call int_handler on SIGINT (Ctrl+C)
  signal(SIGINT, int_handler);
}

#endif
