#include <iostream>

#include "AquaTux.h"
#include "SimulatorSingleton.h"
#include "SubmarineSingleton.h"
#include "mgui.h"

using namespace std;

AquaTux::AquaTux(const char *settings_file) : m_attitude_input(NULL)
{
  // Read settings for input, control algorithm and output classes
  string attitude_input, image_input, control_algorithm, actuator_output;
  read_mv_setting(settings_file, "ATTITUDE_INPUT", attitude_input);
  read_mv_setting(settings_file, "IMAGE_INPUT", image_input);
  read_mv_setting(settings_file, "CONTROL_ALGORITHM", control_algorithm);
  read_mv_setting(settings_file, "ACTUATOR_OUTPUT", actuator_output);

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
      cout << "Warning: unrecognized image input " << image_input << ", defaulting to no image input\n";
    }
    m_image_input = new ImageInputNull();
  }

  if (control_algorithm == "JOYSTICK") {
    m_control_algorithm = new ControlAlgorithmJoystick();
  } else if (control_algorithm == "COMMAND_LINE") {
    m_control_algorithm = new ControlAlgorithmCommandLine();
  } else if (control_algorithm == "SUBMARINE") {
    m_control_algorithm = new ControlAlgorithmSubmarine();
  } else {
    if (image_input != "NULL") {
      cout << "Warning: unrecognized control algorithm " << control_algorithm << ", defaulting to no control algorithm\n";
    }
    m_control_algorithm = new ControlAlgorithmNull();
  }

  if (actuator_output == "SIMULATOR") {
    m_actuator_output = new ActuatorOutputSimulator();
  } else if (actuator_output == "SUBMARINE") {
    m_actuator_output = new ActuatorOutputSubmarine();
  } else {
    if (actuator_output != "NULL") {
      cout << "Warning: unrecognized actuator output " << actuator_output << ", defaulting to no actuator output\n";
    }
    m_actuator_output = new ActuatorOutputNull();
  }

  // Pass the inputs and outputs to the control algorithm
  m_control_algorithm->initialize(m_attitude_input, m_image_input, m_actuator_output);
}

void AquaTux::work()
{
  // Initialize singletons if necessary
  SubmarineSingleton::get_instance().create();
  SimulatorSingleton::get_instance().create();

  // start the control algorithm until mission completes
  m_control_algorithm->work();
}

AquaTux::~AquaTux()
{
  delete m_attitude_input;
}
