#ifndef __MDA_SCRIPTS_H__
#define __MDA_SCRIPTS_H__

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../ui/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_fpga();

#ifdef __cplusplus
}
#endif

inline void int_handler(int signal)
{
  // If the power is already off, just kill child and exit
  if (!get_power()) {
    kill_child();
    exit(0);
  }


  // Calling the int_handler will kill the child process (nios2-terminal)
  // Respawn it, then exit_safe
  spawn_term(NULL);

  exit_safe();
}

inline void init_fpga()
{
  static bool initialized = false;
  if (initialized) {
    return;
  }

  // Fork a nios2-terminal for communication
  spawn_term(NULL);

  // Call int_handler on SIGINT (Ctrl+C)
  signal(SIGINT, int_handler);

  initialized = true;
}

#endif
