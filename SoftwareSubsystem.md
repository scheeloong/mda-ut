# Software Subsystem #

The software in the submarine is responsible for analyzing sensor data from the webcams, the inertial measurement unit and the hydrophones. It analyzes the input data to make decisions about how the submarine should move in the environment, ie sending commands to the motors. The software is run entirely on the netbook and the FPGA. The high-level code is written in C++ on the netbook and includes vision and mission control. The communication layer to the FPGA is written in C on the netbook. The FPGA code is written in C and Verilog (a hardware description language). C is used for higher level things such as getting information to the netbook and stabilization control. Verilog is used for cycle-accurate tasks, such as interfacing with the electronics, which includes the output of motor driving signals. Our software also includes a simulation infrastructure to test the submarine outside of the water.

## Directory Structure ##

All of the netbook code is in /software/
  * ///software/vision//        contains opencv code and vision modules
  * ///software/tasks//         contains task-specific vision and control modules, and the high-level mission
  * ///software/interface//     contains infrastructure to hook up different inputs and outputs to facilitate testing and controlling the submarine
  * ///software/sim//           contains simulator code
  * ///software/opencv\_utils//  contains opencv utility code
  * ///software/settings//      contains settings files for changing parameters without needing to re-compile code
  * ///software/ui//            contains code to interface with the FPGA
  * ///software/scripts//       contains code to communicate with the FPGA (to automate simple tests)
All of the FPGA code is in /fpga/
  * ///fpga/fpga\_hw//           contains Verilog for communicating with the submarine's electronics
  * ///fpga/fpga\_sw//           contains embedded stabilization and UI to access sensors and hardware

## Technical Details ##

For more detailed technical documentation, consult the following:

Getting Started Pages:
  * HowToGetMdaSourceCode
  * CodingPractices

Modules:
  * EmbeddedSoftware
  * Mission and Control (to come)
  * Vision (to come)