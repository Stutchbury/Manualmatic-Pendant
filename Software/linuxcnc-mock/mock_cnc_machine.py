#!/usr/bin/python
##
#
# A mock cnc machine for helping develop the Manualmatic Pendant UI
# Uses the linuxcnc_mock class.
#
# From a command prompt in this directory:
#
# $ ./mock_cnc_machine.py mock.ini
#
# Ctrl-C to exit.
#
# GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
# 
# Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
# 
##

import sys
import time
import threading


sys.path.append('../linuxcnc/python-component/')

from Manualmatic import Manualmatic, SerialInterface

exitapp = False

from linuxcnc_mock import linuxcnc_mock
c = {"estop-is-activated": 0}
linuxcnc = linuxcnc_mock()

mm = Manualmatic(linuxcnc, c, SerialInterface())


def initPendant():
  linuxcnc.task_state = 4
  mm.writeToSerial('a', '3') #number of axes   
  mm.writeToSerial('a', '3') #number of axes   
  linuxcnc.task_mode = 1
  linuxcnc.axis_mask = 7
  linuxcnc.homed[0] = 1
  linuxcnc.homed[1] = 1
  linuxcnc.homed[2] = 1



# #########################################################
def runInputThread():
  global exitapp
  while not exitapp:
    cmdMsg = input('Cmd msg: ')
    if ( len(cmdMsg) > 0 ):
      if ( cmdMsg == 'init'):
        initPendant()
      else:
        cmd = cmdMsg[0:2]
        msg = ""
        if ( len(cmdMsg) > 2 ):
          msg = cmdMsg[2:]
        if ( cmd == 'E '):
          print(msg)
          linuxcnc.task_state = int(msg)
          print(linuxcnc.task_state)
        else:
          mm.writeToSerial(cmd, format(msg))
        print(cmd)
        print(msg)


# #########################################################
def main():
  global connected
  try: 

    inThread = threading.Thread(target=runInputThread, args=())
    #inThread.daemon = True
    inThread.start()
    #inThread.join()
    time.sleep(0.5)
    linuxcnc.task_state = 2
    print("connected")
    initPendant()
    #mm.init(0.04) #spawn new thread
    mm.start() #stay in this thread

  except (Exception):
    raise
    sys.exit(1)



# #########################################################
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        exitapp = True
        raise
        sys.exit(1)




