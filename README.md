# OpenNintendoProCon
## Open source Linux driver for the Nintendo Switch Pro Controller

## Author: ChrisB

The goal of this project is to provide an open source driver for the Nintendo Switch Pro Controller,
so that a Linux desktop user is able to use a pro controller in a manner similar to other computer peripherals.
(e.g mouse devices, keyboards).

![Nintendo Switch Pro Controller](resources/pro_controller.jpg)

### Tested Environments
- Fedora 30 (Workstation Edition) x86_64 w/ Kernel **5.6.13-100.fc30.x86_64**


### Dependencies
- Linux Joystick kernel module
  - Fedora 30 installation Example: `dnf install joystick-support`
  - See Joystick API @ the following links: 
    - [Kernel Text File Documentation](https://www.kernel.org/doc/Documentation/input/joystick-api.txt)
    - [Sphinx Style Documentation](https://www.kernel.org/doc/html/latest/input/joydev/joystick.html)
- libXtst-1.2.3-9
- libXtst-devel-1.2.3-9


### Notes
- The **jstest** tool can be used to view controller input events
  - Example: `jstest --event /dev/input/js1`



