#include "ImageInput.h"
#include "SubmarineSingleton.h"

ImageInputSubmarine::ImageInputSubmarine()
{
  SubmarineSingleton::get_instance().increment_instances();
  // no need to register anything with the singleton
}

ImageInputSubmarine::~ImageInputSubmarine()
{
  SubmarineSingleton::get_instance().increment_instances();
}
