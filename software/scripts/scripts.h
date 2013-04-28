#ifndef __MDA_SCRIPTS_H__
#define __MDA_SCRIPTS_H__

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fpga_ui.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_fpga();

#ifdef __cplusplus
}
#endif

inline void int_handler(int signal)
{
  // ignore SIGCHLD
  if (signal == SIGCHLD) {
    return;
  }

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
  // Call on segmentations faults, broken pipes or hangups too
  signal(SIGSEGV, int_handler);
  signal(SIGHUP, int_handler);
  signal(SIGPIPE, int_handler);
  // Call int_handler on SIGCHLD as well
  signal(SIGCHLD, int_handler);

  initialized = true;
}

#endif
