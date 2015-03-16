# Pool Testing #

## General Logistics ##

Our large pool tests are in the 25-yard Benson Pool in the AC. The small pool tests are done in an outdoor backyard pool...

## General Checklists ##

  1. DryTestingChecklist covers a checklist of things to test 1-2 days before each water
  1. WetTestingChecklist covers a checklist of what to bring to each water test

## Technical Objectives ##

These technical objectives will be specific for each water test. Each will need to also ensure the full functionality required by the DryTestingChecklist.

  * Wednesday, April 24
    * Schedule
      * Meet in MDA room at 8pm for WetTestingChecklist
      * Pool testing in the Benson Pool from 9pm to midnight
    * Requirements to be completed before the water test
      * Build spherical and cylindrical buoy objects (mech)
      * Vision + Control integration with good responsiveness (software)
      * Video playback debugging framework (software)
    * Water test goals:

| **Task** | **Description** | **Estimated Time** | **Water Operators** | **Driver** | **Notes** |
|:---------|:----------------|:-------------------|:--------------------|:-----------|:----------|
| Remote Control Buoy | Use the UI to manually control the submarine to touch the buoy | 9:10-9:30 (20m) | sign up and sign up | Victor | The logged video will be valuable for the vision team to debug for later tests |
| Gate Control Task | Go forward in a straight line (and repeat) | 9:30-9:50 (20m) | sign up and sign up | Victor | Try the length of the shallow end |
| Sinking Control Task | Sink to the bottom of the deep end | 9:50-10:20 (30m) | Victor and sign up | David | Maintain positive buoyancy |
| Autonomous Buoy Task | Use vision and control code | 10:20-11:00 (40m) | sign up and sign up | Ritchie | Turn to face the buoy, approach the buoy... |
| Autonomous Path Task | Use vision and control code | 11:00-11:40 (40m) | sign up and sign up | Ritchie | Align with the path's direction, centre above the path... |
| Remote Control Video Logging | Take videos of pool noodles and other objects | 11:40-11:55 (15m) | sign up and sign up | Victor | Can be shortened to a few minutes, time permitting |

Note: The water operators will monitor the submarine in the water and record underwater videos when possible. The driver operates the submarine through the Ethernet connection.

  * Saturday, April 13
    * Schedule
      * Meet in MDA room at 8:15am for WetTestingChecklist
      * Pool testing in the Benson Pool from 9am to noon
    * Requirements to be completed before the water test
      * IMU measurements working with yaw and depth controllers (Victor)
      * IMU connected to FPGA robustly (David)
      * Vision/Task tests prepared (Ritchie) with 2 simultaneous webcam usage optional
      * Obtain/create spherical target and cylindrical physical object (Ritchie)
      * Mech requirements? (Ameen)
    * Water test goals
      * Update ballast (Mech)
      * Depth/roll/pitch controllers working as before (Embedded)
      * Yaw controller (Embedded)
      * Underwater video logging (Software)
    * Water test stretch goals (time permitting)
      * Repetitive loop task to stress all controllers (Embedded)
      * Vision detection (Software)
      * Buoy task (Software)
      * Path task (Software)
      * Mission with path + buoy (Software)

## How to Test the Sub ##

Sealing the sub properly is the most important part of testing the sub, but this section will focus on how to use the software to test the sub and how to debug problems that may arise.

First on the laptop that is connected to Ethernet, set up a wired connection:

Go to Network Connections and create a new Wired connection.

IPv4 settings: <br>
Method: Manual <br>
Address: 192.168.1.20 <br>
Netmask: 255.255.255.0 <br>
Gateway: 255.255.255.0 <br>
Uncheck "Require IPv4 addressing for this connection to complete"<br>
<br>
<ol><li>Connect the Ethernet cable and SSH into the netbook<br>
<ul><li>Turn on the Wired Connection by clicking the Wireless icon on the panel and selecting the Manual Ethernet connection<br>
</li><li>Run: ssh -X mda@192.168.1.21<br>
</li></ul></li><li>Run: cd mda-ut/fpga && make dl_both, which will program the FPGA and the soft processor on the FPGA<br>
</li><li>Run: cd ..<br>
</li><li>Run: ./sub_test.sh<br>
<ul><li>This will run the command-line UI to control the sub, and also log the video and text output to tests/(Day of Year)<code>&lt;HH&gt;</code>:<code>&lt;MM&gt;</code> after quitting<br>
</li></ul></li><li>Press '^' to power on the sub. The depth and yaw readings should start to make sense. If there is an error, the UI will print out 'power failed' after the target yaw/depth readings<br>
</li><li>Press '%' to turn on the sub's controllers<br>
<ul><li>Do this on deck first to check the controllers are working. Plug the depth sensor with a finger to get some motors to move, and press 'a' or 'd' to go left/right for the yaw motors to move<br>
</li><li>On a real test, only do this once the sub is in the water and the ballast is balanced<br>
</li></ul></li><li>Press '$' to power off the sub if necessary. Press 'q' or Ctrl+C to exit the UI.<br>
</li><li>If you are in a task or in vision mode, you need to press enter after a command for it to be flushed. ie, to exit a task, press 'q' then enter.<br>
</li><li>To check the logged videos, you can run: mplayer tests/(Day of Year)<code>&lt;HH&gt;</code>:<code>&lt;MM&gt;</code>/sub.avi (or sub_dwn.avi). You may need to run ls/tests to see the time of the latest run.</li></ol>

<h2>Fixing random issues with testing the sub</h2>

If you run into any weird problems with the UI such as the Ethernet connection dropping or strange error messages, try the following:<br>
<br>
<ul><li>Did the Ethernet disconnect while a program was still running?<br>
<ul><li>Yes -> Flip the kill switch! Once you regain the Ethernet connection, run: pkill -2 aquatux<br>
</li><li>Still have problems? -> Run: pkill -9 aquatux; pkill -9 nios2-terminal<br>
</li></ul></li><li>Does the error message say "No longer connected to nios2-terminal, exiting"?<br>
<ul><li>Yes -> Flip the kill switch, then run: make -C ~/mda-ut/fpga dl_both (the FPGA needs to be reprogrammed)<br>
</li></ul></li><li>Is there some weird message about the display being incorrect?<br>
<ul><li>Yes -> Start a new SSH connection with: ssh -X mda@192.168.1.21<br>
</li></ul></li><li>Need to copy files from the netbook or update code on the netbook?<br>
<ul><li>Yes -> Use scp (copy over SSH), ie scp -r mda@192.168.1.21:~/mda-ut/tests . (this will recursively copy all the testing data from the Netbook to the connected laptop)<br>
</li></ul></li><li>Your problem not covered? -> You're on your own then...