#include "ImageInput.h"
#include "SimulatorSingleton.h"

ImageInputSimulator::ImageInputSimulator(const char *settings_file) : ImageInput(settings_file, false)
{
  SimulatorSingleton::get_instance().register_instance();
}

ImageInputSimulator::~ImageInputSimulator()
{
}

bool ImageInputSimulator::ready_internal_image(ImageDirection dir)
{
    return true;
}

IplImage *ImageInputSimulator::get_internal_image(ImageDirection dir)
{
  return SimulatorSingleton::get_instance().get_image(dir);
}
