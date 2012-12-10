#include "ImageInput.h"
#include "SimulatorSingleton.h"

ImageInputSimulator::ImageInputSimulator(const char *settings_file) : ImageInput(settings_file, false)
{
  SimulatorSingleton::get_instance().increment_instances();
  // no need to register anything with the singleton
}

ImageInputSimulator::~ImageInputSimulator()
{
  SimulatorSingleton::get_instance().decrement_instances();
}

const IplImage *ImageInputSimulator::get_internal_image(ImageDirection dir)
{
  return SimulatorSingleton::get_instance().get_image(dir);
}
