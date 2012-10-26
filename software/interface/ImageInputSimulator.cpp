#include "ImageInput.h"
#include "SimulatorSingleton.h"

ImageInputSimulator::ImageInputSimulator()
{
  SimulatorSingleton::get_instance().increment_instances();
  // no need to register anything with the singleton
}

ImageInputSimulator::~ImageInputSimulator()
{
  SimulatorSingleton::get_instance().decrement_instances();
}
