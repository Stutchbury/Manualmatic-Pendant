##
#
# Manualmatic helper class for the LinuxCNC userpace component.
# Periodically polls the linuxcnc interface, forwards changes over 
# USB and listens for & forwards commands from pendant.
#
# GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
# 
# Copyright (c) 2021-2024 Philip Fletcher <philip.fletcher@stutchbury.com>
# 
##

import glob
import os
import serial
import sys
import time
import threading
import re
import subprocess

# https://www.linuxcnc.org/docs/html/gui/GStat.html
#from hal_glib import GStat
#import gobject

# https://github.com/LinuxCNC/linuxcnc/issues/2273#issuecomment-1398532401
from qtvcp import logger
LOG = logger.initBaseLogger('MANUALMATIC', log_file=None, log_level=logger.INFO)


dump_serial_comms = False

def fmtround5(value):
  return "%0.5f" % (value,)

# If trailing %, remove and divide by 100
def convert_if_percent(s):
  # This:
  # return s.replace('%', 'e-2')
  # or this:
  s = str(s)
  if ( '%' not in s): 
    return float(s.replace(',', '.'))
  s = s.replace(',', '.')
  return float(s.strip('%'))/100

class Commands:
  # Valid values for cmd[0]
  CMD_ABSOLUTE_POS = 'A' #Absolute position
  CMD_SPINDLE_SPEED = 'S' #Commanded spindle speed (not RPM unless override is 100%) IN/OUT 
  CMD_SPINDLE_OVERRIDE = 's' #Spindle override - combined with speed results in RPM IN/OUT
  CMD_SPINDLE_RPM = 'R' #Actual RPM of spindle as a result of SPINDLE_SPEED * OVERRIDE OUT
  CMD_SPINDLE_DIRECTION = 'G' #Spindle direction OUT
  CMD_FEED_OVERRIDE = 'f' #Feed override IN/OUT
  CMD_RAPID_OVERRIDE = 'r' #Rapid override IN/OUT
  CMD_JOG = 'J' #Jog
  CMD_JOG_VELOCITY = 'j' #Jog Velocity IN/OUT (sorta)
  CMD_JOG_CONTINUOUS = 'N' #Jog continuous (nudge)
  CMD_JOG_STOP = 'n' #no Jog
  CMD_TASK_MODE = 'M' #mode IN/OUT
  CMD_TASK_STATE = 'E' #task_state (Estop and On/Off) IN/OUT
  CMD_INTERP_STATE = 'I' #interp_state
  CMD_CURRENT_VEL = 'v' #current_vel
  CMD_MOTION_TYPE = 't' #motion_type
  CMD_G5X_INDEX = 'W' #g5x_index (WCS) IN OUT
  CMD_G5X_OFFSET = '5' #g5x_offset OUT: current offset
  CMD_G92_OFFSET = '9' #g92_offset OUT
  CMD_TOOL_OFFSET = 'T' #Tool_offset OUT
  CMD_DTG = 'D' #DTG
  CMD_HOMED = 'h' #homed
  CMD_ALL_HOMED = 'H' #all axes homed
  CMD_INI_VALUE = 'i' #ini value
  CMD_FLOOD = 'C' #Coolant
  CMD_MIST = 'c' #Little Coolant
  CMD_EXEC_STATE = 'e'
  CMD_PROGRAM_STATE = 'p' #OUT: Running, paused, stepping
  CMD_AUTO = 'a' #IN: RUN, PAUSE, RESUME, STEP progam
  CMD_HEARTBEAT = 'b' #Heartbeat

  # Valid values for cmd[1] when cmd[0] is CMD_INI_VALUE
  INI_AXES = 'a' #Number of axes 
  INI_MAX_FEED_OVERRIDE ='F'
  INI_MIN_SPINDLE_OVERRIDE = 's'
  INI_MAX_SPINDLE_OVERRIDE = 'S'
  INI_SPINDLE_INCREMENT = 't'
  INI_DEFAULT_SPINDLE_SPEED = 'r' #r for RPM
  INI_MAX_SPINDLE_SPEED = 'R' #R for RPM
  INI_LINEAR_UNITS = 'U'
  INI_ANGULAR_UNITS = 'u'
  INI_DEFAULT_LINEAR_VELOCITY = 'v'
  INI_MAX_LINEAR_VELOCITY = 'V'
  INI_NO_FORCE_HOMING = 'h'
  INI_COMPLETE = '.'

class SerialInterface:
  STX = b"\x02"
  ETX = b"\x03"

  RX_STATE_BEGIN=0
  RX_STATE_COMMAND=1

  def __init__(self, port=None, speed=115200, read_timeout=0.02, connect_retry_time=3):
    self.owner = None
    self.port = port
    self.speed = speed
    self.read_timeout = read_timeout
    self.connect_retry_time = connect_retry_time
    self.connection = None
    self.startConnecting()
    self.rx_state = self.RX_STATE_BEGIN
    self.rx_state_count = 0
    self.rx_buffer = None

  # #########################################################
  def setOwner(self, owner):
    self.owner = owner

  # #########################################################
  def disconnect(self):
    if self.connection:
      self.connection.close()
    self.startConnecting()

  # #########################################################
  def startConnecting(self):
    self.retry = 0
    self.connection = None
    self.last_connect_attempt = 0
    self.rx_state = self.RX_STATE_BEGIN

  def detectTeensy(self):
    if os.path.exists("/dev/serial/by-id"):
      files = glob.glob("/dev/serial/by-id/usb-Teensyduino_USB_Serial_*")
      if files:
        return files[0]
    files = glob.glob("/dev/ttyACM*")
    for f in files:
      f = os.path.join("/dev", f)
      props = os.popen("udevadm info -q property " + f, "r").readlines()
      vid = None
      pid = None
      for line in props:
        key, value = line.rstrip("\n").split("=", 2)
        if key == "ID_USB_VENDOR_ID":
          vid = value
        elif key == "ID_USB_MODEL_ID":
          pid = value
      if vid == "16c0" and pid == "0483":
        return f
    return None

  # #########################################################
  # Do another attempt at connecting if it's not too soon after
  # the last attempt
  def attemptConnecting(self):
    if time.time() - self.last_connect_attempt < self.connect_retry_time:
      return
    self.last_connect_attempt = time.time()
    self.retry += 1
    if ( self.retry < 5 or self.retry%5 == 0 ):
      LOG.info("Connection attempt %d" % (self.retry, ))
    try:
      port = self.port
      if port is None:
        port = self.detectTeensy()
        if not port:
          if ( self.retry < 5 or self.retry%5 == 0 ):
            LOG.warning("Manualmatic/Teensy device not found")
          return
      self.connection = serial.Serial(port, self.speed, timeout=self.read_timeout)
    except serial.SerialException as e:
      LOG.error("Connection failed - %s" % (str(e),))
      return
    # Connected, let it settle...
    LOG.info("Connected")
    time.sleep(0.1)
    self.owner.onConnected()

  # #########################################################
  def inputError(self, error):
    self.owner.serialError(error)
    self.rx_state = self.RX_STATE_BEGIN

  # #########################################################
  # Feed a single character to the input state machine
  def processCharacter(self, c):
    if c == self.STX:
      if self.rx_state != self.RX_STATE_BEGIN:
        self.serialError("Possible truncated packet")
      self.rx_state = self.RX_STATE_COMMAND
      self.rx_state_count = 0
      self.rx_buffer = bytearray(22)
    elif c == self.ETX:
      if self.rx_state == self.RX_STATE_COMMAND:
        if self.rx_state_count < 1:
          self.inputError("Command too short")
        else:
          # Note: we're not sending any proper Unicode characters from the
          # pendant right now, so let's use a simple 8-bit encoding here to
          # avoid failing on potential illegal multi-byte sequences due
          # to serial line corruption.
          data = self.rx_buffer.decode('iso-8859-1')
          self.owner.processCmd(data[0:2], data[2:].strip('\00'))
          self.rx_state = self.RX_STATE_BEGIN
          return True
      else:
        self.inputError("Unexpected ETX")
    elif self.rx_state == self.RX_STATE_COMMAND:
      if self.rx_state_count >= 22:
        self.inputError("Command too long")
      else:
        self.rx_buffer[self.rx_state_count] = ord(c)
        self.rx_state_count += 1
    elif self.rx_state == self.RX_STATE_BEGIN:
      self.inputError("Unexpected input character:" + repr(c))
    else:
      assert False, "Invalid state: " + str(self.rx_state)
    return False

  # #########################################################
  # Process all the characters in the input buffer until an input
  # timout happens.
  def handleSerialInput(self):
    while True:
      if self.connection is None and not self.attemptConnecting():
        return False
      try:
        c = self.connection.read(1)
        if not c: # timeout
          return False
        if self.processCharacter(c): # a command has been executed
          return True
      except serial.SerialException as e:
        LOG.error("Exception in Manualmatic.readFromSerial()", e)
        self.owner.onDisconnected()
        self.startConnecting()
        return False

  def writeCommand(self, cmd, payload):
    if (dump_serial_comms and cmd != 'b '):
      LOG.debug("Outgoing(" + repr(cmd) + ", " + repr(payload) + ")")
    return self.write(self.STX + cmd.encode() + payload.encode() + self.ETX)

  def write(self, data):
    if self.connection is None:
      return False
    try:
      self.connection.write(data)
      return True
    except serial.SerialException as se:
      LOG.error("Exception in Manualmatic.write ToSerial()", str(se))
      self.owner.onDisconnected()
      return False

class MachineStateValue:
  def __init__(self, cmd, getter, formatter=format):
    self.cmd = cmd
    self.getter = getter
    self.formatter = formatter
    self.last_value = None
    self.dirty = True
  def forceRefresh(self):
    self.dirty = True
  def update(self, owner):
    try:
      current_value = self.getter()
      if current_value != self.last_value:
        self.last_value = current_value
        self.dirty = True
      if self.dirty:
        if self.send(owner):
          self.dirty = False
    except:
      print ("Context:", repr(self.cmd))
      raise
  def send(self, owner):
    return owner.writeToSerial(self.cmd, self.formatter(self.last_value))

# For commands like E or H where the argument is part of the command name
class MachineStateCommand(MachineStateValue):
  def send(self, owner):
    return owner.writeToSerial(self.cmd + self.formatter(self.last_value))

# This must be a function in order to ensure a correct index value
def machineStateValueForIndex(cmd, getter, formatter, index):
  return MachineStateValue(cmd + str(index), lambda: getter(index), formatter)

class MachineStateArray:
  def __init__(self, cmd, indexes, getter, formatter=format):
    self.indexes = indexes
    self.values = [ machineStateValueForIndex(cmd, getter, formatter, index) for index in indexes ]
  def forceRefresh(self):
    for i in self.values:
      i.forceRefresh()
  def update(self, owner):
    for i in self.values:
      i.update(owner)

# ##########################################################################################
# start of Manualmatic class definition
#
class Manualmatic(Commands):

  # Used to feed back running, paused or stopped state for auto 
  PROGRAM_STATE_NONE=0
  PROGRAM_STATE_RUNNING=1
  PROGRAM_STATE_PAUSED=2
  PROGRAM_STATE_STOPPED=3

  # Heartbeat timeout in seconds
  HEARTBEAT_MAX=10

  linuxcnc = None
  hal = None #hal
  mmc = None #Component
  ls = None #linuxcnc stat
  lc = None # command
  le = None # error
  #gstat = None
  inifile = None
  error = None
  
  running = False

  last_heartbeat = 0

  task_state = None
  interp_state = None
  interpreter_errcode = None
  #actual_position = [None, None, None, None, None, None, None, None]
  #max_velocity = 0
  #velocity = 0
  motion_type = None # 1, 2, or 3 (so far)
  #command = None
  #axis_velocity = [None, None, None, None, None, None, None, None]
  current_vel = 0
  # We hold jogVelocity as uom per minute as this is what is diplayed 
  # LinuxCNC Python interface requires uom per second 
  jog_velocity = 1000
  paused = None
  task_paused = None
  file = None
  state = None
  program_state = None

  #Used to map 0-8 axis id to axis name for MDI
  axes = 0 #Number of configured axes
  axesMap = 'XYZABCUVW'
  
  #https://linuxcnc.org/docs/2.8/html/config/ini-config.html
  #ini values
  display = 'gmoccapy' #May be used to read different ini values
  max_feed_override = 1.5
  min_spindle_override = 0.5
  max_spindle_override = 1.5
  default_spindle_speed = 1000
  spindle_increment = 100
  max_spindle_speed = 3000.0
  spindle_speed_cmd = 0 # Commanded spindle RPM
  # Actual spindle RPM (including override)
  # - either that requested by LinuxCNC as 'spindle.0.speed-out'
  # - or from hal pin specified in spindle_rpm_pin 
  spindle_speed_out = 0 
  spindle_rpm_pin = "spindle.0.speed-out"
  spindle_last_update = 0

  linear_units = 'mm'
  angular_units = 'degree'
  default_linear_velocity = 25 # ij (jog mm/sec) 
  #max_linear_velocity = 166 #iJ (jog max)
  max_linear_velocity = 42 #iJ (jog max)
  #default_linear_acceleration = 2.0 #default value in ini docs
  #max_linear_acceleration = 20.0

  no_force_homing = 0
  
  def __init__(self, _linuxcnc, _hal, _mmc, _serial_intf):
    self.linuxcnc = _linuxcnc
    self.ls = self.linuxcnc.stat()
    self.lc = self.linuxcnc.command()
    self.hal = _hal
    self.mmc = _mmc
    self.serial_intf = _serial_intf
    self.serial_intf.setOwner(self)
    # Slightly frustrating we cannot use the error channel but understand why.
    # (if we pick the error off the queue here, it won't show in the main UI)
    # Could do with access to 'last error' instead...?
    # https://www.linuxcnc.org/docs/html/config/python-interface.html#_reading_the_error_channel
    # self.le = self.linuxcnc.error_channel()
    # Will use interpreter_errcode instead for those errors.
    # Maybe investigate this more later
    # self.gstat = GStat()
    #
    # At some point we will also derive pendant setup from a [MANUALMATIC] section in the ini file
    #
    self.inifile = self.linuxcnc.ini(sys.argv[1])
    # Caution: will return true even if inifile is a directory but (obviously) will not
    # return any ini values. Ask me how I know.
    if ( self.inifile ):
      self.readIniFileValues()

  # #########################################################
  def is_homed(self):
    #self.ls.poll() Currently never called without poll() being called first
    return self.ls.homed.count(1) == self.ls.joints

  # #########################################################
  def ok_for_mdi(self):
    self.ls.poll()
    return not self.ls.estop and self.ls.enabled and self.is_homed() and (self.ls.interp_state == self.linuxcnc.INTERP_IDLE)

  # #########################################################
  # As a general rule, prioritise values set in the [DISPLAY] section as we are a UI
  # except max spindle speed where we use the lowest value
  # and where used, the [MANUALMATIC] section will take priority
  # https://www.linuxcnc.org/docs/stable/html/nb/config/ini-config.html
  def readIniFileValues(self):
    if ( self.inifile ):
      # Feed Override
      self.max_feed_override = self.inifile.find('DISPLAY', 'MAX_FEED_OVERRIDE') or 1.5
      # Min Spindle Override
      self.min_spindle_override = self.inifile.find('DISPLAY', 'MIN_SPINDLE_OVERRIDE') or None 
      if ( self.min_spindle_override is None ):
        self.min_spindle_override = self.inifile.find('DISPLAY', 'MIN_SPINDLE_0_OVERRIDE') or 0.5 # QtVCP
      # Max Spindle Override
      self.max_spindle_override = self.inifile.find('DISPLAY', 'MAX_SPINDLE_OVERRIDE') or None
      if ( self.max_spindle_override is None ):
        self.max_spindle_override = self.inifile.find('DISPLAY', 'MAX_SPINDLE_0_OVERRIDE') or 1.5 # QtVCP
      # Default Spindle Speed
      self.default_spindle_speed = self.inifile.find('DISPLAY', 'DEFAULT_SPINDLE_SPEED') or None
      if ( self.default_spindle_speed is None ):
        self.default_spindle_speed = self.inifile.find('DISPLAY', 'DEFAULT_SPINDLE_0_SPEED') or 1001
      # Max Spindle Speed
      max_spindle_velocity = []
      max_spindle_velocity.append(self.inifile.find('SPINDLE_0', 'MAX_FORWARD_VELOCITY') or None) #official?
      max_spindle_velocity.append(self.inifile.find('SPINDLE_0', 'MAX_VELOCITY') or None) #pncconf generated
      max_spindle_velocity.append(self.inifile.find('DISPLAY', 'MAX_SPINDLE_0_SPEED') or None) #qtvcp only, deprecated?
      max_spindle_velocity.append(self.inifile.find('DISPLAY', 'MAX_SPINDLE_SPEED') or None) #In SIMs, deprecated?`
      self.max_spindle_speed = min([float(rpm) for rpm in max_spindle_velocity if rpm is not None], default=0) or 0

      # Spindle Increment
      # See: https://forum.linuxcnc.org/38-general-linuxcnc-questions/54327-ini-file-clarification-roadmap#313606
      # and: https://github.com/LinuxCNC/linuxcnc/issues/2335
      # and: https://github.com/LinuxCNC/linuxcnc/issues/3171
      self.spindle_increment = self.inifile.find('MANUALMATIC', 'SPINDLE_INCREMENT') or None
      if ( self.spindle_increment is None ):
        self.spindle_increment = self.inifile.find('DISPLAY', 'SPINDLE_INCREMENT') or None #wrongly deprecated?
        if ( self.spindle_increment is None ):
          self.spindle_increment = self.inifile.find('SPINDLE_0', 'INCREMENT') or 100 #official but incorrect
      self.spindle_increment = convert_if_percent(self.spindle_increment)

      self.spindle_rpm_pin = self.inifile.find('MANUALMATIC', 'SPINDLE_RPM_PIN') or "spindle.0.speed-out"

      self.linear_units = self.inifile.find('TRAJ', 'LINEAR_UNITS') or 'mm'
      self.angular_units = self.inifile.find('TRAJ', 'ANGULAR_UNITS') or 'degree'
      
      # Default linear/jog velocity
      self.default_linear_velocity = self.inifile.find('DISPLAY', 'DEFAULT_LINEAR_VELOCITY') or None
      if ( self.default_linear_velocity is None ):
        self.default_linear_velocity = self.inifile.find('TRAJ', 'DEFAULT_LINEAR_VELOCITY') or 20
      self.jog_velocity = self.default_linear_velocity*60

      # Max linear velocity
      self.max_linear_velocity = self.inifile.find('DISPLAY', 'MAX_LINEAR_VELOCITY') or None
      if ( self.max_linear_velocity is None ):
        self.max_linear_velocity = self.inifile.find('TRAJ', 'MAX_LINEAR_VELOCITY') or 30

      self.no_force_homing = self.inifile.find('TRAJ', 'NO_FORCE_HOMING') or 0
      
      self.log_ini_file_values()
      

    
  # Send ini values to serial - not just those read from the ini file
  # cmd[0] = self.CMD_INI_VALUE for ini, cmd[1] lowercase for default or min, upper case for max
  def sendIniValues(self):
    
    LOG.info('Sending ini values...')
    self.writeIniValueToSerial(self.INI_AXES, self.axes)
    self.writeIniValueToSerial(self.INI_MAX_FEED_OVERRIDE, self.max_feed_override)
    self.writeIniValueToSerial(self.INI_MIN_SPINDLE_OVERRIDE, self.min_spindle_override)
    self.writeIniValueToSerial(self.INI_MAX_SPINDLE_OVERRIDE, self.max_spindle_override)
    self.writeIniValueToSerial(self.INI_DEFAULT_SPINDLE_SPEED, self.default_spindle_speed)
    self.writeIniValueToSerial(self.INI_MAX_SPINDLE_SPEED, self.max_spindle_speed)
    self.writeIniValueToSerial(self.INI_SPINDLE_INCREMENT, self.spindle_increment)
    self.writeIniValueToSerial(self.INI_LINEAR_UNITS, self.linear_units)
    self.writeIniValueToSerial(self.INI_ANGULAR_UNITS, self.angular_units)
    self.writeIniValueToSerial(self.INI_DEFAULT_LINEAR_VELOCITY, self.default_linear_velocity)
    self.writeIniValueToSerial(self.INI_MAX_LINEAR_VELOCITY, self.max_linear_velocity)
    self.writeIniValueToSerial(self.INI_NO_FORCE_HOMING, self.no_force_homing)
    self.writeToSerial(self.CMD_INI_VALUE+self.INI_COMPLETE)


  # #########################################################
  # Used for debug
  def log_ini_file_values(self):
    LOG.info('INI_MAX_FEED_OVERRIDE = {}'.format(self.max_feed_override) )
    LOG.info('INI_MIN_SPINDLE_OVERRIDE = {}'.format(self.min_spindle_override))
    LOG.info('INI_MAX_SPINDLE_OVERRIDE = {}'.format(self.max_spindle_override))
    LOG.info('INI_DEFAULT_SPINDLE_SPEED = {}'.format(self.default_spindle_speed))
    LOG.info('INI_MAX_SPINDLE_SPEED = {}'.format(self.max_spindle_speed))
    LOG.info('INI_SPINDLE_INCREMENT = {}'.format(self.spindle_increment))
    LOG.info('INI_SPINDLE_RPM_PIN = {}'.format(self.spindle_rpm_pin))
    LOG.info('INI_LINEAR_UNITS = {}'.format(self.linear_units))
    LOG.info('INI_ANGULAR_UNITS = {}'.format(self.angular_units))
    LOG.info('INI_DEFAULT_LINEAR_VELOCITY = {}'.format(self.default_linear_velocity))
    LOG.info('INI_MAX_LINEAR_VELOCITY = {}'.format(self.max_linear_velocity))
    LOG.info('INI_NO_FORCE_HOMING = {}'.format(self.no_force_homing))

  # #########################################################
  # Used for debug
  def log_axis(self, i):
      LOG.debug('g5x_offset ', format(i), ': ', format(self.ls.g5x_offset[i]))
      LOG.debug('abs ', format(i), ': ', format(self.ls.actual_position[i]));
      LOG.debug('g92_offset ', format(i), ': ', format(self.ls.g92_offset[i]))
      LOG.debug('tool_offset ', format(i), ': ', format(self.ls.tool_offset[i]))

  # #########################################################
  # Used for debug
  def log_stat(self):
    self.ls.poll()
    for x in dir(self.ls):
        if not x.startswith("_"):
            LOG.debug(x, getattr(self.ls,x))
            #LOG.debug("\n")

  # #########################################################
  # Used for debug
  def log_inifile(self):
    for x in dir(self.inifile):
        if not x.startswith("_"):
            LOG.debug(x, getattr(self.inifile,x))
            LOG.debug("\n")

  # #########################################################
  # Used for debug
  def log_interp_state(self):
    if (self.ls.interp_state == self.linuxcnc.INTERP_IDLE):
      LOG.debug('interp_state: ' + 'INTERP_IDLE')
    elif (self.ls.interp_state == self.linuxcnc.INTERP_READING):
      LOG.debug('interp_state: ' + 'INTERP_READING')
    elif (self.ls.interp_state == self.linuxcnc.INTERP_PAUSED):
      LOG.debug('interp_state: ' + 'INTERP_PAUSED')
    elif (self.ls.interp_state == self.linuxcnc.INTERP_WAITING):
      LOG.debug('interp_state: ' + 'INTERP_WAITING')
    else:
      LOG.debug('interp_state: ' + format(self.ls.interp_state) + '???')
        
  def log_exec_state(self):
    None
    if ( self.ls.state == self.linuxcnc.RCS_DONE ) :
      LOG.debug('exec_state: ' + format(self.ls.state) + ': RCS_DONE')
    elif ( self.ls.state == self.linuxcnc.RCS_EXEC ) :
      LOG.debug('exec_state: ' + format(self.ls.state) + ': RCS_EXEC')
    elif ( self.ls.state == self.linuxcnc.RCS_ERROR ) :
      LOG.debug('exec_state: ' + format(self.ls.state) + ': RCS_ERROR')
    else:
      LOG.debug('exec_state: ' + format(self.ls.state) + '???')
      
  def get_program_state(self):
      if (self.ls.task_mode != self.linuxcnc.MODE_AUTO or
          self.ls.file is None or len(self.ls.file) == 0):
          return self.PROGRAM_STATE_NONE      
      if (self.ls.state == self.linuxcnc.RCS_DONE or 
          self.ls.state == self.linuxcnc.RCS_ERROR):
          return self.PROGRAM_STATE_STOPPED
      elif (self.ls.state == self.linuxcnc.RCS_EXEC):
          if (self.ls.task_paused == 1 and self.ls.current_vel == 0 ):
              return self.PROGRAM_STATE_PAUSED
          else:
              return self.PROGRAM_STATE_RUNNING
        
  # #########################################################
  # Format an INI value and send it to the pendant
  def writeIniValueToSerial(self, key, value):
    self.writeToSerial(Commands.CMD_INI_VALUE + key, format(value))

  # #########################################################
  # Write a message to the serial port
  def writeToSerial(self, cmd, payload=""):
    #LOG.debug(cmd)
    #LOG.debug(payload)
    #Pad cmd if required
    if len(cmd) == 1:
      cmd = cmd + ' '
    if not self.serial_intf.writeCommand(cmd, payload):
      self.serial_intf.disconnect()
      return False
    return True
  
  def serialError(self, error):
    print ("Serial error:", error)

  def resend_positions(self):
    self.g5x_index.forceRefresh()
    self.g5x_values.forceRefresh()
    self.g92_values.forceRefresh()
    self.tool_values.forceRefresh()
    self.absolute_pos_values.forceRefresh()
      
  # #########################################################
  # Initialise or reset the state of the pendant
  def resetState(self):
    self.writeToSerial(self.CMD_TASK_STATE + format(self.ls.task_state))
    self.task_state = self.ls.task_state
    self.axes = 0
    self.axes_list = []
    for i in range(9):
      if self.ls.axis_mask & (1<<i): 
        self.axes_list.append(i)
        self.axes +=1
    self.sendIniValues()
    # Store these for later use
    self.g5x_index =  MachineStateCommand(Commands.CMD_G5X_INDEX, lambda: self.ls.g5x_index)
    self.g5x_values = MachineStateArray(Commands.CMD_G5X_OFFSET, self.axes_list, lambda i: self.ls.g5x_offset[i], formatter=fmtround5)
    self.g92_values = MachineStateArray(Commands.CMD_G92_OFFSET, self.axes_list, lambda i: self.ls.g92_offset[i], formatter=fmtround5)
    self.tool_values = MachineStateArray(Commands.CMD_TOOL_OFFSET, self.axes_list, lambda i: self.ls.tool_offset[i], formatter=fmtround5)
    self.absolute_pos_values = MachineStateArray(Commands.CMD_ABSOLUTE_POS, self.axes_list, lambda i: self.ls.actual_position[i], formatter=fmtround5)
    self.on_state_values = [
      self.g5x_index,
      self.g5x_values,
      self.g92_values,
      self.tool_values,
      self.absolute_pos_values,
      MachineStateArray(Commands.CMD_DTG, self.axes_list, lambda i: self.ls.dtg[i], formatter=fmtround5),
      MachineStateValue(Commands.CMD_SPINDLE_OVERRIDE, lambda: self.ls.spindle[0]["override"]),
      #MachineStateValue(Commands.CMD_SPINDLE_RPM, lambda: self.ls.spindle[0]["speed"]),
      MachineStateValue(Commands.CMD_SPINDLE_DIRECTION, lambda: self.ls.spindle[0]["direction"]),
      MachineStateValue(Commands.CMD_FEED_OVERRIDE, lambda: self.ls.feedrate),
      MachineStateValue(Commands.CMD_RAPID_OVERRIDE, lambda: self.ls.rapidrate),
      MachineStateCommand(Commands.CMD_TASK_MODE, lambda: self.ls.task_mode),
      MachineStateArray(Commands.CMD_HOMED, self.axes_list, lambda i: self.ls.homed[i]),
      MachineStateCommand(Commands.CMD_ALL_HOMED, lambda: 1 if self.is_homed() else 0),
      MachineStateValue(Commands.CMD_INTERP_STATE, lambda: self.ls.interp_state),
      MachineStateValue(Commands.CMD_CURRENT_VEL, lambda: self.ls.current_vel),
      MachineStateCommand(Commands.CMD_MOTION_TYPE, lambda: self.ls.motion_type),
      MachineStateCommand(Commands.CMD_FLOOD, lambda: self.ls.flood),
      MachineStateCommand(Commands.CMD_MIST, lambda: self.ls.mist),
    ]
    self.writeToSerial(self.CMD_JOG_VELOCITY, format(self.jog_velocity))

  # #########################################################
  # Called every pollRate seconds to compare the current linuxcnc
  # state with sent state and write changes to the serial port
  # #TODO Investigate https://www.linuxcnc.org/docs/html/gui/GStat.html
  def poll(self):
    # update linuxcnc state
    self.ls.poll()
    
    # Check spindle speed out pin
    # Pin value is changed when spindle is started by either Manualmatic or GUI
    # This is not available via the Python Interface
    # Pin value is the requested rpm of spindle, including override percent
    # This is not available via the Python Interface
    # spindle.0.speed-out caps itself at [SPINDLE_0]MAX_FORWARD_VELOCITY
    speed_out = self.hal.get_value(self.spindle_rpm_pin)
    # Slow down the spindle updates.
    current_ms = int(time.time()*1000)
    if ( speed_out != self.spindle_speed_out and current_ms > (self.spindle_last_update+333) ):
      self.spindle_last_update = current_ms
      #LOG.debug("Spindle Speed Out: " + str(speed_out) )
      #LOG.debug("ls.spindle[0][speed]: " + format(self.ls.spindle[0]["speed"]))
      #LOG.debug("ls.spindle[0][override]: " + format(self.ls.spindle[0]["override"]))
      #rpm = self.ls.spindle[0]["speed"] * self.ls.spindle[0]["override"]
      #LOG.debug("Calculated RPM: " + format(rpm))
      #LOG.debug("self.hal.get_value(spindle.0.speed-out: " + str(self.hal.get_value("spindle.0.speed-out")) )
      #if ( speed_out != 0 ):
      self.writeToSerial(self.CMD_SPINDLE_RPM, format(speed_out))
      self.spindle_speed_out = speed_out
    
    
    # Compare and send
    if (self.task_state != self.ls.task_state):
      #LOG.debug('task state changed", self.ls.task_state)
      # linuxcnc.STATE_ESTOP: 1, STATE_ESTOP_RESET: 2, STATE_ON: 4
      # STATE_OFF: 3? Appears to not be used
      self.resetState()

    # Skip anything that is not required when machine is not on
    if ( self.ls.task_state == self.linuxcnc.STATE_ON ):

      for value in self.on_state_values:
        value.update(self)


      # state ######################
      if ( self.program_state != self.get_program_state()):
        self.program_state = self.get_program_state()
        self.writeToSerial(self.CMD_PROGRAM_STATE + format(self.program_state))
        if ( self.program_state == self.PROGRAM_STATE_NONE ):
            LOG.debug('program_state: PROGRAM_STATE_NONE')
        elif ( self.program_state == self.PROGRAM_STATE_RUNNING ):
            LOG.debug('program_state: PROGRAM_STATE_RUNNING')
        elif ( self.program_state == self.PROGRAM_STATE_PAUSED ):
            LOG.debug('program_state: PROGRAM_STATE_PAUSED')
        elif ( self.program_state == self.PROGRAM_STATE_STOPPED ):
            LOG.debug('program_state: PROGRAM_STATE_STOPPED')


  # #########################################################
  # Called if a valid message is received
  def processCmd(self, cmd, payload):
    if (dump_serial_comms and cmd != 'b '):
      LOG.debug("Incoming(" + repr(cmd) + ", " + repr(payload) + ")")

    # Abort
    if ( cmd[0] == '!'):
      self.lc.abort();
      LOG.debug("Received abort!");

    # Heartbeat
    elif ( cmd[0] == self.CMD_HEARTBEAT ):
      self.last_heartbeat = time.time()
      #LOG.debug("<B...");
      #Bounce it right back
      self.writeToSerial(self.CMD_HEARTBEAT)

    # Jog
    elif ( cmd[0] == self.CMD_JOG_STOP and self.ls.axis_mask & (1<<int(cmd[1])) ):
      #LOG.debug("Jog Stop: " + self.axesMap[int(cmd[1])])
      if (self.ls.motion_mode != self.linuxcnc.TRAJ_MODE_TELEOP):
        self.lc.teleop_enable(True)
        self.lc.wait_complete()
      try:
        self.lc.jog(self.linuxcnc.JOG_STOP, False, int(cmd[1]))
      except:
        None
    elif ( cmd[0] == self.CMD_JOG and self.ls.axis_mask & (1<<int(cmd[1])) ):
      if (self.ls.motion_mode != self.linuxcnc.TRAJ_MODE_TELEOP):
        self.lc.teleop_enable(True)
        self.lc.wait_complete()
      try:
        self.lc.jog(self.linuxcnc.JOG_INCREMENT, False, int(cmd[1]), (self.jog_velocity/60), float(payload))
      except:
        None

    elif ( cmd[0] == self.CMD_JOG_CONTINUOUS and self.ls.axis_mask & (1<<int(cmd[1])) ):
      #LOG.debug("Jog Continuous: " + self.axesMap[int(cmd[1])])
      if (self.ls.motion_mode != self.linuxcnc.TRAJ_MODE_TELEOP):
        self.lc.teleop_enable(True)
        self.lc.wait_complete()
      try:
        if ( float(payload) == 0 ):
          self.lc.jog(self.linuxcnc.JOG_STOP, False, int(cmd[1]))
        else:
          self.lc.jog(self.linuxcnc.JOG_CONTINUOUS, False, int(cmd[1]), (float(payload)/60))
      except:
        None

    # Turn on or off
    elif ( cmd[0] == self.CMD_TASK_STATE and '1234'.find(cmd[1]) != -1 ):
      if ( (self.ls.task_state == self.linuxcnc.STATE_ESTOP_RESET
      or self.ls.task_state == self.linuxcnc.STATE_OFF )
      and int(cmd[1]) == self.linuxcnc.STATE_ON):
        LOG.info("Turning machine on")
        self.lc.state(self.linuxcnc.STATE_ON)
        self.resend_positions()
      elif (self.ls.task_state == self.linuxcnc.STATE_ON
      and int(cmd[1]) == self.linuxcnc.STATE_OFF ):
        LOG.info("Turning machine off")
        self.lc.state(self.linuxcnc.STATE_OFF)
      if (int(cmd[1]) == self.linuxcnc.STATE_ESTOP ):
        LOG.info("ESTOP")
        #Set the hal pin
        self.mmc['estop-is-activated'] = 1
        self.lc.state(self.linuxcnc.STATE_ESTOP) # Shouldn't be necessary unless estop_latch is not comfigured
      if (int(cmd[1]) == self.linuxcnc.STATE_ESTOP_RESET ):
        LOG.info("ESTOP_RESET")
        #Set the hal pin 
        self.mmc['estop-is-activated'] = 0
        if (self.ls.task_state == self.linuxcnc.STATE_ESTOP ): #Will likely only happen if estop_latch is configured
          self.lc.state(self.linuxcnc.STATE_ESTOP_RESET)
    
    # Jog velocity
    elif ( cmd[0] == self.CMD_JOG_VELOCITY ):
      self.jog_velocity = float(payload)
      # Write directly to serial - jog_velocity is not part of linuxcnc interface
      #self.gstat.set_jograte(float(payload)/60) #@TODO Work out why this dowsn't update gmoccapy
      self.writeToSerial(self.CMD_JOG_VELOCITY, format(self.jog_velocity))

    # Spindle on/off
    elif ( cmd[0] == self.CMD_SPINDLE_SPEED ):
      #@TODO check current task_mode
      #@TODO Check if we need override_enabled for manual control of spindle
      #if ( self.ls.spindle[0]["override_enabled"] == 0):
      #self.lc.set_spindle_override(1, 0)
      #self.lc.wait_complete()
      rpm = float(payload)
      #LOG.debug("rpm: ", rpm)
      if ( rpm > 0 ):
        #LOG.debug("forward")
        # Never allow spindle over speed
        rpm = min(rpm, int(self.max_spindle_speed))
        if ( self.ls.spindle[0]["direction"] == 0 ):
          #Hit it twice or it'll start at gmoccapy default!!!
          self.lc.spindle(self.linuxcnc.SPINDLE_FORWARD, float(rpm), 0)
          time.sleep(0.12)
        self.lc.spindle(self.linuxcnc.SPINDLE_FORWARD, float(rpm), 0)
      elif ( rpm < 0 ):
        # Never allow spindle over speed
        rpm = max(rpm, int(self.max_spindle_speed)*-1)
        #LOG.debug("reverse")
        if ( self.ls.spindle[0]["direction"] == 0 ):
          #Hit it twice or it'll start at gmoccapy default!!!
          self.lc.spindle(self.linuxcnc.SPINDLE_REVERSE, abs(rpm), 0)
          time.sleep(0.12)
        self.lc.spindle(self.linuxcnc.SPINDLE_REVERSE, abs(rpm), 0)
      else:
        self.lc.spindle(self.linuxcnc.SPINDLE_OFF, 0)

    #Spindle override
    elif ( cmd[0] == self.CMD_SPINDLE_OVERRIDE ):
      #@TODO check current task_mode
      if ( self.ls.spindle[0]["override_enabled"] == 0):
        self.lc.set_spindle_override(1, 0)
        self.lc.wait_complete()
      incr = float(payload)
      #LOG.debug("set spindle override: " + str(incr) )
      self.lc.spindleoverride(incr, 0)

    # Feed override
    elif ( cmd[0] == self.CMD_FEED_OVERRIDE ):
        if ( self.ls.feed_override_enabled == 0):
          self.lc.set_feed_override(1)
          self.lc.wait_complete()
        incr = float(payload)
        self.lc.feedrate(incr)

    # Rapid override
    elif ( cmd[0] == self.CMD_RAPID_OVERRIDE ):
        #if ( self.ls.rapid_override_enabled == 0):
        #  self.lc.set_rapid_override(1)
        #  self.lc.wait_complete()
        incr = float(payload)
        self.lc.rapidrate(incr)

    # Mode
    elif ( cmd[0] == self.CMD_TASK_MODE and '123'.find(cmd[1]) != -1 ):
      #@TODO check before attempting to change
      self.lc.mode(int(cmd[1]))

    # G5X offset
    elif ( cmd[0] == self.CMD_G5X_OFFSET and self.ls.axis_mask & (1<<int(cmd[1])) ):
      if ( self.ok_for_mdi() ):
          self.lc.mode(self.linuxcnc.MODE_MDI)
          self.lc.wait_complete()
          i = int(cmd[1])
          # have to round offset or we may get an 'e' within format'd string
          offset = round(float(payload), 5)
          # KF: should that not be the g5x_offset instead?
          # PF: Depends on G90 or G91: https://www.cnccookbook.com/g10-g-code-fanuc-cnc-machining-haas/
          # Using P0 - will set the current work offset.
          # Using L20 rather than L2 calculates based on G90 or 91
          # http://linuxcnc.org/docs/html/gcode/g-code.html#gcode:g10-l20
          mdi_cmd = 'G10 L20 P0 ' + self.axesMap[i] + format(offset)
          self.lc.mdi(mdi_cmd)
          self.lc.wait_complete()
          self.lc.mode(self.linuxcnc.MODE_MANUAL)
          self.lc.wait_complete()
          # must resend *all* offset and absolute positions
          self.resend_positions()

    # Flood
    elif ( cmd[0] == self.CMD_FLOOD ):
        self.lc.flood(int(cmd[1]))

    # Mist
    elif ( cmd[0] == self.CMD_MIST ):
        self.lc.mist(int(cmd[1]))

    # Auto
    elif ( cmd[0] == self.CMD_AUTO ):
        if ( cmd[1] == '0' ):
          self.lc.auto(self.linuxcnc.AUTO_RUN, 1)
        elif ( cmd[1] == '1' ):
          self.lc.auto(self.linuxcnc.AUTO_STEP)
        elif ( cmd[1] == '2' ):
          self.lc.auto(self.linuxcnc.AUTO_PAUSE)
        elif ( cmd[1] == '3' ):
          self.lc.auto(self.linuxcnc.AUTO_RESUME)



    # Debug
    elif ( cmd == 'DD' ):
      LOG.debug('Debug: ' + payload )


  def checkHeartbeat(self):
    if self.last_heartbeat > 0 and self.last_heartbeat + self.HEARTBEAT_MAX < time.time():
      LOG.warning("no heartbeat, stopping...")
      self.serial_intf.disconnect()

  # #########################################################
  # Called on successful opening of the serial port
  def onConnected(self):
    self.last_heartbeat = 0
    #Start the heartbeat
    self.writeToSerial(self.CMD_HEARTBEAT)
    self.resetState()

  # #########################################################
  # Called on when the serial port has been disconnected
  def onDisconnected(self):
    # @TODO Do not send jog stop if machine is off
    if ( self.ls.task_state == self.linuxcnc.STATE_ON and self.ls.motion_mode == self.linuxcnc.TRAJ_MODE_TELEOP ):
      #LOG.debug("stop jog on disconnect")
      for axis in self.axes_list:
        self.lc.jog(self.linuxcnc.JOG_STOP, False, axis)

  # #########################################################
  # Run the loop to check incoming serial data and
  # linuxcnc state every pollRate seconds
  # Possibly provide two different update rates? Do we need
  # to poll state as often as listen to the serial port?
  # poll every nth read? or rather, read every pollRate/nth
  def run(self):
    while self.running:
      self.serial_intf.handleSerialInput()
      if self.serial_intf.connection:
        self.checkHeartbeat()
        self.poll()
      else:
        self.ls.poll()
        time.sleep(0.25)

  def isRunning(self):
    return self.running

  # #########################################################
  # Stop run(ning)
  def stop(self):    
    LOG.info("Stopping Manualmatic...")
    self.running = False


  # #########################################################
  # Start running in the current thread
  def start(self):
    LOG.info("Starting Manualmatic...")
    self.running = True
    self.run()

  # #########################################################
  # Start running in a new thread - not sure this is necessary
  # in the context of a user space component. Currently not used.
  def init(self, pollRate=0.04):    
    statusThread = threading.Thread(target=self.run, args=[pollRate])
    self.running = True
    #statusThread.daemon = True
    statusThread.start()
    #statusThread.join()

# end of Manualmatic class definition
# ##########################################################################################
