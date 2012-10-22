/* Constructor and destructor for sim resource */

SimulatorSingleton::create()
{
  if (instances == 0) {
    return;
  }
  created = true;

  // Start simulation (may need to use passed in data)
}

SimulatorSingleton::destroy()
{
  if (!created) {
    return;
  }

  // Clean up resources
}
