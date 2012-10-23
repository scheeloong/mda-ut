#include "SimulatorSingleton.h"

/* Constructor and destructor for sim resource */

void SimulatorSingleton::create()
{
  if (instances == 0) {
    return;
  }
  created = true;

  // Start simulation (may need to use passed in data)
}

void SimulatorSingleton::destroy()
{
  if (!created) {
    return;
  }

  // Clean up resources
}
