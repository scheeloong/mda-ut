#include <iostream>

#include "AquaTux.h"
#include "SimulatorSingleton.h"
#include "SubmarineSingleton.h"
#include "mgui.h"

using namespace std;

AquaTux::AquaTux(const char *settings_file) : m_attitude_input(NULL)
{
  // Read settings for input, control algorithm and output classes
  string attitude_input, image_input, operation, actuator_output;
  read_mv_setting(settings_file, "ATTITUDE_INPUT", attitude_input);
  read_mv_setting(settings_file, "IMAGE_INPUT", image_input);
  read_mv_setting(settings_file, "OPERATION", operation);
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

  if (operation == "JOYSTICK") {
    m_operation = new JoystickOperation();
  } else if (operation == "COMMAND_LINE") {
    m_operation = new CommandLineOperation();
  } else if (operation == "MISSION") {
    m_operation = new Mission();
  } else {
    if (image_input != "NULL") {
      cout << "Warning: unrecognized operation algorithm " << operation << ", defaulting to no operation algorithm\n";
    }
    m_operation = new OperationNull();
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
  m_operation->initialize(m_attitude_input, m_image_input, m_actuator_output);
}

void AquaTux::work()
{
  // Initialize singletons if necessary
  SubmarineSingleton::get_instance().create();
  SimulatorSingleton::get_instance().create();

  // start the control algorithm until mission completes
  m_operation->work();
}

AquaTux::~AquaTux()
{
  delete m_attitude_input;
}
