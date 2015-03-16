# Embedded Software #

The embedded software is made up of many layers. This page will quickly describe what each layer is responsible and point where the source code is. For more detail, you can always consult the source code. The levels will progress from highest level to lowest level before the electronics.

## Netbook ##

The netbook is responsible for machine vision and making decisions based on image processing. This includes the high-level commands for navigating all the visual elements of the obstacle course. The netbook code is all contained in [software/](http://code.google.com/p/mda-ut/source/browse/#svn%2Ftrunk%2Fsoftware). For example, software/scripts/ contains API that allows the software to access the lower level elements of the submarine.

## Scripting Layer ##

The scripting code in [software/scripts/](http://code.google.com/p/mda-ut/source/browse/#svn%2Ftrunk%2Fsoftware%2Fscripts) provides examples of API that can be used to control the lower level parts of the submarine. There is a template program that demonstrates how to write scripts in this directory. This is useful to automate tests that can be used for measurements. For example, we intend to use this API to tune our motor controllers, by making it easy to automate certain tasks and gather output information. The scripts here aren't meant to be too complex. This code relies on some OS hackery in scripts.h, which piggybacks off of the FPGA UI code. init() sets up the communication channel with the FPGA and exit\_safe() closes this channel. Interrupt handlers have been added so that Ctrl+C will safely power down the submarine and exit gracefully. This allows infinite loop scripts to run until the user stops it.

For example, this is a simple script:

```
#include <unistd.h> // for sleep() functionality
#include "scripts.h"

int main()
{
  init();
  power_on();

  dyn_set_target_depth(600); // sink
  sleep(30); // let it stay underwater for 30 seconds

  exit_safe(); // The submarine will float back up because it is buoyant
}
```

## FPGA UI ##

There is a simple command line user interface (UI) that let's a user interactively control the submarine's movement as well as monitor different sensor readings. The code is run on the netbook, but communicates directly with the FPGA. This is written in [fpga/ui/](http://code.google.com/p/mda-ut/source/browse/#svn%2Ftrunk%2Ffpga%2Fui) and can be run in fpga by typing

```
make ui_term
```

This opens a terminal that is connected to the FPGA to give commands. This interface is very useful for testing the submarine. We use a laptop on dry land and connect to the ship's netbook via Ethernet cable. This is done through SSH and then running the FPGA UI on the Netbook through the SSH connection. This makes it easy to remotely power on and off the sub, give rise and sink commands, and monitor the depth readings and log useful debug messages. There won't be too much work here except to extend the API to give access to the FPGA's other interfaces. The FPGA UI, the scripting layer and the mission control all use the same library functions to communicate with the FPGA.

## FPGA Processor ##

The FPGA Processor runs C code on the NIOS processor, a [MIPS-like](http://en.wikipedia.org/wiki/MIPS_architecture) embedded processor. This processor runs directly on the FPGA and has direct access to the hardware modules on the FPGA. The executable file of the program itself is stored on external memory, a separate chip on the DE0-Nano board. The code on the FPGA is split into three main parts:

  1. Command shell and low-level interfaces
  1. Control stabilization code
  1. Sonar processing (not yet implemented)

The command shell acts like a terminal shell. It continually parses commands on each line and runs a command, possibly outputting as well. When the FPGA is downloaded, you can get direct access to this shell by:

```
cd fpga
# Program the software and hardware on the FPGA
make dl_both
# Get terminal access to the FPGA
make terminal
```

Some of the low-level tasks the FPGA does is digitize electronic sensors (depth, IMU), control power (on, off and check for power failures), and provide cycle-accurate duty cycle signals to the motors to represent power and direction of movement.

The control code on the FPGA uses PID controllers to stabilize 4 degrees of freedom: yaw, pitch, roll, and depth. More on this later...

The FPGA processor codes lives in [fpga/fpga\_sw/](http://code.google.com/p/mda-ut/source/browse/#svn%2Ftrunk%2Ffpga%2Ffpga_sw) under [src/](http://code.google.com/p/mda-ut/source/browse/#svn%2Ftrunk%2Ffpga%2Ffpga_sw%2Fsrc) (C source files) and [inc/](http://code.google.com/p/mda-ut/source/browse/#svn%2Ftrunk%2Ffpga%2Ffpga_sw%2Fsrc) (header files).

To add a new command, add a new struct to my\_cmds in [src/main.c](http://code.google.com/p/mda-ut/source/browse/trunk/fpga/fpga_sw/src/main.c). You will also need to add a new COMMAND\_ID enum for your command in [inc/utils.h](http://code.google.com/p/mda-ut/source/browse/trunk/fpga/fpga_sw/inc/utils.h). To process the command, add another case to process\_command in [src/main.c](http://code.google.com/p/mda-ut/source/browse/trunk/fpga/fpga_sw/src/main.c). Most of the code beside parsing the arguments should be another function in [src/utils.c](http://code.google.com/p/mda-ut/source/browse/trunk/fpga/fpga_sw/src/utils.c). See the other commands as a reference.

## FPGA Hardware ##

This low-level Verilog code is very minimal and should just work. It is mostly directly interfacing with the electronics (much of this code is from Quartus IP or University Program IP as much as possible) and also to provide cycle-accurate waveforms to control the motor drivers.

Find me at [fpga/fpga\_hw/top\_level/](http://code.google.com/p/mda-ut/source/browse/#svn%2Ftrunk%2Ffpga%2Ffpga_hw%2Ftop_level).

There are some mini projects to test functionality without requiring the whole system. They are at [fpga/fpga\_hw/demo\_projects/](http://code.google.com/p/mda-ut/source/browse/#svn%2Ftrunk%2Ffpga%2Ffpga_hw%2Fdemo_projects).

The reason for mini projects:

![http://imgs.xkcd.com/comics/compiling.png](http://imgs.xkcd.com/comics/compiling.png)