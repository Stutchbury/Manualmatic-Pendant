# Arduino/Teensy Manualmatic MPG/Pendant for LinuxCNC


My Christmas project for 2021 - the aim being to be able to perform the majority of routine LinuxCNC setup, manual and automatic functions without having to step away to a PC monitor.

Proof of concept:
![pendant proof of concept](images/manualmatic-2022-01-31.jpeg)

Kicked off [here](https://forum.linuxcnc.org/18-computer/44682-arduino-teensy-python-interface-manualmatic-mpg-pendant) on the LinuxCNC forum.

This still a work in progress but has already resulted in me releasing a few Arduino libraries that will hopefully help in other projects too. 

The currently released code implements almost all the functionality for both manual and auto control.

Auto mode screen (uses the [DisplayUtils](https://github.com/Stutchbury/DisplayUtils) library):
![auto screen](images/manualmatic-auto.jpg)


I'm currently building all the pendant software using [Teensyduino](https://www.pjrc.com/teensy/td_download.html) via the Arduino IDE - as this has (I think) greater user coverage. That may change to PlatformIO if it gets too knarly.


Setting offsets (uses the [TouckKeypad](https://github.com/Stutchbury/TouchKeypad) library):
![offset screen](images/manualmatic-offset-keypad.jpg)


The communication beween the [Teensy 4.1](https://www.pjrc.com/store/teensy41.html) and the [LinuxCNC Python Interface](https://www.linuxcnc.org/docs/html/config/python-interface.html) is over USB using a very simple protocol.


While the pendant prioritises working with LinuxCNC, in principle it is a fairly agnostic UI to control anything that can read & write the serial protocol.



## Getting Started

This was always intended to be "Almost Plug'n'Play"&trade; but does require the Arduino sketch to be uploaded to the Teensy and two files copied to your LinuxCNC config directory.

- Clone or download this repository and extract somewhere convenient (not usually on your LinuxCNC machine).

### Arduino/Teensy Install
- Install Teensyduino in the Arduino IDE, available [here](https://www.pjrc.com/teensy/teensyduino.html). Note the specific versions of the Arduino IDE that are supported.
- Using the Arduino Library Manager, install the EncoderButton, DisplayUtils and TouchKeypad libraries (you do not need to download these from github).
- Open the ```manualmatic-pendant.ino``` in the Arduino IDE
- Upload to the Teesny 4.1.

### LinuxCNC Install

- Copy both ```Manualmatic.py``` (the class def) and ```manualmatic``` (the component, lowercase, no file extension) to your LinuxCNC config directory.
- Add the following line to custom.hal (replace ```gmoccapy.ini``` with your ini file name):    
```loadusr -W ./manualmatic gmoccapy.ini```
- Plug your manualmatic pendant into a USB port - make sure appears as ```/dev/ttyACM0```
- Start LinuxCNC, preferably from the command line (to see any errors).







