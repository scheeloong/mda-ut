#include <iostream>

#include "AquaTux.h"
#include "SimulatorSingleton.h"
#include "SubmarineSingleton.h"
#include "mgui.h"

using namespace std;

AquaTux::AquaTux(const char *settings_file) : m_attitude_input(NULL)
{
  // Read settings for input, control algorithm and output classes
  string attitude_input, image_input;
  read_mv_setting(settings_file, "ATTITUDE_INPUT", attitude_input);
  read_mv_setting(settings_file, "IMAGE_INPUT", image_input);

  if (attitude_input == "SIMULATOR") {
    m_attitude_input = new AttitudeInputSimulator();
  } else if (attitude_input == "SUBMARINE") {
    m_attitude_input = new AttitudeInputSubmarine();
  } else {
    if (attitude_input != "NULL") {
      cout << "Warning: unrecognized attitude input " << attitude_input << ", defaulting to no attitude input\n";
    }
    m_attitude_input = new AttitudeInputNull();
  }

  if (image_input == "SIMULATOR") {
    m_image_input = new ImageInputSimulator();
  } else if (attitude_input == "SUBMARINE") {
    m_image_input = new ImageInputSubmarine();
  } else {
    if (image_input != "NULL") {
      cout << "Warning: unrecognized attitude input " << image_input << ", defaulting to no attitude input\n";
    }
    m_image_input = new ImageInputNull();
  }
}

void AquaTux::work()
{
  // Initialize singletons if necessary
  SubmarineSingleton::get_instance().create();
  SimulatorSingleton::get_instance().create();

  // start the control algorithm until mission completes
}

AquaTux::~AquaTux()
{
  delete m_attitude_input;
}
