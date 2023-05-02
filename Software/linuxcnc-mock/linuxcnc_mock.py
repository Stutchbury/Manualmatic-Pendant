##
# A *very* crude mock of the linuxcnc python interface used to help
# develop the Manualmatic Pendant UI. Run this via mock_cnc_machine.py
# to fool the pendant into believing it is talking to the linuxcnc 
# Python interface.
#
# GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
# 
# Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
# 
##


# #########################################################
  # command

class command_mock():

  def __init__(self, ls):
    self.ls = ls

  ls = None
  

# ##########################
  def abort(self):
    print("Abort sent!");
    self.ls.state = self.ls.RCS_DONE
    self.ls.task_paused = 0

# ##########################
  def state(self, taskState):
    #@TODO - when turning off, state goes from on -> off -> to estop reset?
    #self.task_state = taskState
    if ( (self.ls.task_state == self.ls.STATE_ESTOP_RESET
    or self.ls.task_state == self.ls.STATE_OFF )
    and taskState == self.ls.STATE_ON):
      print("turning on")
      self.ls.task_state = self.ls.STATE_ON
      self.ls.estop = 1
      self.ls.enabled = 1
    elif (self.ls.task_state == self.ls.STATE_ON
    and taskState == self.ls.STATE_OFF ):
      print("turning off")
      self.ls.task_state = self.ls.STATE_OFF
      self.ls.task_state = self.ls.STATE_ESTOP_RESET
    elif ( taskState == self.ls.STATE_ESTOP ):
      self.ls.task_state = self.ls.STATE_ESTOP
    elif ( taskState == self.ls.STATE_ESTOP_RESET ):
      self.ls.task_state = self.ls.STATE_ESTOP_RESET


  def spindle(self, dir, rpm, spindle=0):
    self.ls.spindle[0]['speed'] = rpm*dir

# ##########################
  def mode(self, m):
    self.ls.task_mode = m
# ##########################
  def teleop_enable(self, en):
    None

# ##########################
  def wait_complete(self):
    None

# ##########################
  def set_spindle_override(self, onOff, spindle):
    self.ls.spindle_override = onOff


# ##########################
  def spindleoverride(self, incr, spindle):
    self.ls.spindle[0]["override"] = incr

# ##########################
  def jog(self, JOG_INCREMENT, jjogmode, axis, vel, distance):
    self.ls.actual_position[axis] += distance

# ##########################
  def set_feed_override(self, b):
    self.ls.feed_override_enabled = b

# ##########################
  def feedrate(self, f):
    self.ls.feedrate = f

# ##########################
  def rapidrate(self, f):
    self.ls.rapidrate = f

# ##########################
  def flood(self, onOff):
    self.ls.flood = onOff

# ##########################
  def mist(self, onOff):
    self.ls.mist = onOff

# ##########################
  def auto(self, action, line=1):
    print('auto: ' + format(action))
    if ( action == self.ls.AUTO_RUN ):
      self.ls.state = self.ls.RCS_EXEC
      self.ls.task_paused = 0
    elif ( action == self.ls.AUTO_PAUSE ):
      self.ls.state = self.ls.RCS_EXEC
      self.ls.task_paused = 1
    elif ( action == self.ls.AUTO_STEP ):
      self.ls.state = self.ls.RCS_EXEC
      self.ls.task_paused = 1
    if ( action == self.ls.AUTO_RESUME ):
      self.ls.state = self.ls.RCS_EXEC
      self.ls.task_paused = 0


# #########################################################
# #########################################################
# #########################################################
# state

class linuxcnc_mock():

  def __init__(self):
    self.spindle[0] = {}
    self.spindle[0]['override_enabled'] = 0
    self.spindle[0]['override'] = 1
    self.spindle[0]['speed'] = 0
    self.lc = command_mock(self)

  STATE_ESTOP=1
  STATE_ESTOP_RESET=2
  STATE_OFF=3
  STATE_ON=4

  MODE_UNKNOWN=0
  MODE_MANUAL=1
  MODE_AUTO=2
  MODE_MDI=3


  MOTION_TYPE_NONE=0
  MOTION_TYPE_TRAVERSE=1
  MOTION_TYPE_FEED=2
  MOTION_TYPE_ARC=3
  MOTION_TYPE_TOOLCHANGE=4
  MOTION_TYPE_PROBING=5
  MOTION_TYPE_INDEXROTARY=6


  INTERP_IDLE=1 

  TRAJ_MODE_TELEOP=3

  JOG_INCREMENT=1

  SPINDLE_OFF=0
  SPINDLE_FORWARD=1
  SPINDLE_REVERSE=-1

  FLOOD_ON=0
  FLOOD_OFF=1
  MIST_ON=0
  MIST_OFF=1

  RCS_DONE=0
  RCS_EXEC=1
  RCS_ERROR=2

  AUTO_RUN=0
  AUTO_STEP=1
  AUTO_PAUSE=2
  AUTO_RESUME=3

  PROGRAM_STATE_NONE=0
  PROGRAM_STATE_RUNNING=1
  PROGRAM_STATE_PAUSED=2
  PROGRAM_STATE_STOPPED=3



  lc = None

  # #########################################################
  # state
  estop = 0
  enabled = 0
  task_state = 0
  task_mode = 0
  interp_state = INTERP_IDLE
  spindle_speed = None
  spindle_override = None # 0 off, 1 on
  spindle_override_val = 1 #%
  homed = [0] * 9
  actual_position = [0.0]*9
  g5x_offset = [0.0]*9
  dtg = [0.0] * 9
  g5x_index = 1
  feedrate = 1
  feed_override_enabled = 0
  rapidrate = 1
  jogVelocity = 50
  spindle = [None, None, None, None, None, None, None, None]
  settings = [None, None, None]
  max_velocity = 0
  velocity = 0
  current_vel = 0
  motion_type = 0

  axes = 3
  joints = 3
  motion_mode = 1

  flood=0
  mist=0

  paused = None
  task_paused = None
  file = "test.file"
  state = RCS_DONE
  program_state = None



  # #########################################################
  # Properly fakin' it...

  def stat(self):
    return self
  def command(self):
    return self.lc


  def poll(self):
    None
    #print("polling...")

  def ini(self, file_name):
    print("(not) reading ini ...")
    return None







