#include <unistd.h>
#include "scripts.h"

#define SINK_DEPTH 250
#define PAUSE_TIME 15

int main()
{
  init();

  power_on();

  // Assume the submarine starts on the surface
  int surface_depth = get_depth();

  while (1) {
    // surface
    dyn_set_target_depth(surface_depth);
    sleep(PAUSE_TIME);

    // sink
    dyn_set_target_depth(surface_depth + SINK_DEPTH);
    sleep(PAUSE_TIME);
  }

  power_off();
  return 0;
}
