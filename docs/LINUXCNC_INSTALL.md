### LinuxCNC Install

One of the goals of this project was a 'plug-n-play' pendant and although it's not quite that, it is very easy to install into LinuxCNC - no hal pin configuration required.

## 1. Copy Two Files

Copy both ```Manualmatic.py``` (a Python class definition) and ```manualmatic``` (the LinuxCNC component, lowercase, no file extension) to your LinuxCNC config directory. 

Set the execute permission on the ```manualmatic``` file. (Right-click on the file in File Manager, choose the 'Permissions' tab)

## 2. Update ```custom.hal``` File

Add the following line to ```custom.hal``` (replace ```gmoccapy.ini``` with your ```ini``` file name or you *will* get an error - ask me how I know):    

```loadusr -W ./manualmatic gmoccapy.ini```

The ```custom.hal``` file will normally already exist but it may not for the example sim configurations.

If the ```custom.hal``` file does not exist, create it and add the line above and then in your configuration's ```.ini``` file (```gmocappy.ini``` in this example) add the following line: 

```HALFILE = custom.hal``` 

to the ```[HAL]``` section.

## 3. Plug in Your Manualmatic

Plug your manualmatic pendant into a USB port - make sure appears as ```/dev/ttyACM0``` (from a terminal type: ```ls -l /dev/ttyACM0```)

This port can be changed in the ```manualmatic``` file but ```ttyACM0``` is the default port for a Teensy.


## 4. Start LinuxCNC

Start LinuxCNC normally. Sometimes it takes a little while for the component to load after plugging in the USB for the first time.
