
#include "ManualmaticState.h"


ManualmaticState::ManualmaticState(ManualmaticConfig& config) :config(config) { }


/**
 * Read from serial and set state if message received
 */
void ManualmaticState::update(char cmd[2], char payload[30]) {
//    if ( serialMessage.available(cmd, payload) ) {  
    switch ( cmd[0] ) {
      case CMD_ABSOLUTE_POS:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          axisAbsPos[((int)cmd[1])-48] = atof(payload); //There's probably a better way than -48...   
        }
        break;
      case CMD_CURRENT_VEL:
          current_vel = atof(payload);
          setCurrentVelocities();
        break;
      case CMD_MOTION_TYPE:
        motion_type = static_cast<Motion_type_e>(cmd[1]-'0');
        setCurrentVelocities();
        break;
      case CMD_SPINDLE_RPM:
        spindleRpm = atof(payload);
        break;
      case CMD_SPINDLE_OVERRIDE:
        spindleOverride = atof(payload);
        break;
      case CMD_SPINDLE_DIRECTION:
        spindleDirection = atoi(payload);
        break;
  //    case CMD_MAX_FEED_OVERRIDE: //@TODO move to ini?
  //      feedSpeed = atof(payload);
  //      break;
      case CMD_FEED_OVERRIDE:
        feedrate = atof(payload);
        break;
      case CMD_SPINDLE_SPEED: // @TODO not used?
        spindleSpeed = atof(payload);
        break;
      case CMD_RAPID_OVERRIDE:
        rapidrate = atof(payload);
        break;
      case CMD_JOG_VELOCITY:
          jogVelocity[jogVelocityRange] = atof(payload);
        break;
      case CMD_TASK_MODE:
        setTaskMode( static_cast<Task_mode_e>(cmd[1]-'0'));
        break;
      case CMD_TASK_STATE:
        //@TODO move to control
        if ( task_state == STATE_INIT ) { //We've just started up
          //@ZZsendInit();
        }
        setTaskState(static_cast<Task_state_e>(cmd[1]-'0'));
        break;
      case CMD_INTERP_STATE:
        interpState = static_cast<Interp_e>(atoi(payload));
        break;
      case CMD_EXEC_STATE:
        exec_state = static_cast<Exec_state_e>(cmd[1]-'0');
        break;
      case CMD_PROGRAM_STATE:
        program_state = static_cast<Program_state_e>(cmd[1]-'0');
        break;      
      case CMD_G5X_INDEX:
        g5xIndex = (uint8_t)cmd[1]-'0';
        break;
      case CMD_G5X_OFFSET:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          g5xOffsets[((int)cmd[1])-'0'] = atof(payload);
        } 
        break;
      case CMD_G92_OFFSET:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          g92Offsets[((int)cmd[1])-'0'] = atof(payload);
        } 
        break;
      case CMD_TOOL_OFFSET:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          toolOffsets[((int)cmd[1])-'0'] = atof(payload);
        } 
        break;
      case CMD_DTG:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          axisDtg[((int)cmd[1])-'0'] = atof(payload);
        }
        break;
      case CMD_HOMED:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          homed[cmd[1]-'0'] = atoi(payload); 
        }  
        break;
      case CMD_ALL_HOMED:
        all_homed = cmd[1]-'0';
        break;
      case CMD_INI_VALUE:
        //Hand off cmd[1] to setIniValue
        setIniValue(cmd[1], payload);
        break;
      case CMD_FLOOD:
        flood = static_cast<Flood_e>(cmd[1]-'0');
        break;
      case CMD_MIST:
        mist = static_cast<Mist_e>(cmd[1]-'0');
        break;
      case CMD_HEARTBEAT:
        lastHeartbeatReceived = now;
        if ( iniState == INI_STATE_DISCONNECTED ) {
          onConnected();
        }
        break;
    }
  }

void ManualmaticState::setCurrentVelocities() {
  //Only set values if auto or mdi and mode type is traverse, feed or arc.
  if ( isTaskMode(MODE_AUTO) || isTaskMode(MODE_MDI) ) {
    if ( motion_type == MOTION_TYPE_TRAVERSE ) { //Rapid
      feed_vel = 0;
      rapid_vel = current_vel*60;
    } else if ( motion_type == MOTION_TYPE_FEED || motion_type == MOTION_TYPE_ARC ) { //Feed
      rapid_vel = 0;
      feed_vel = current_vel*60;
    } else { 
      feed_vel = 0;
      rapid_vel = 0;
    }
  } else {
    feed_vel = 0;
    rapid_vel = 0;    
  }    
}

bool ManualmaticState::setTaskMode(Task_mode_e mode, bool force/*=false*/) {
  if ( !force && (isTaskMode(mode) || !isTaskState(STATE_ON)) ) {
    return false;
  }

  task_mode = mode;
  currentAxis = AXIS_NONE;
  joystickAxis[0] = AXIS_NONE;
  joystickAxis[1] = AXIS_NONE;

  switch (task_mode) {    
    case MODE_MANUAL:
      setScreen(SCREEN_MANUAL);
      break;
    case MODE_AUTO:
      setScreen(SCREEN_AUTO);
      break;
    case MODE_MDI:
      setScreen(SCREEN_MDI);
      break;
    default: // MODE_UNKNOWN
      setScreen(SCREEN_INIT);
  }
  refreshDisplay = true;
  return true;
}

bool ManualmaticState::setScreen(Screen_e s ) {
  if ( !isScreen(s) ) {
    previousScreen = screen;
    screen = s;
    setButtonRow(BUTTON_ROW_DEFAULT);
    refreshDisplay = true;
    switch (s) {
      case SCREEN_OFFSET_KEYPAD:
        setButtonRow(BUTTON_ROW_G5X_OFFSET);        
        break;
      default:
        break;  
    }
    return true;
  }
  return false;
}


void ManualmaticState::setTaskState(Task_state_e s) {
  //Always set the state
  task_state = s;
  switch (task_state) {    
    case STATE_ON:
      //Should always turn on in manual mode
      setTaskMode(MODE_MANUAL, true);
      break;
    case STATE_OFF:
    case STATE_ESTOP_RESET:
      setScreen(SCREEN_ESTOP_RESET);
      break;
    case STATE_ESTOP:
      setScreen(SCREEN_ESTOP);
      break;
    default:
      setScreen(SCREEN_SPLASH);
  }
}


bool ManualmaticState::setButtonRow(ButtonRow_e b/*=BUTTON_ROW_DEFAULT*/ ) {
  if ( b == BUTTON_ROW_DEFAULT ) {
    switch ( screen ) {
      case SCREEN_MANUAL:
        b = BUTTON_ROW_MANUAL;
        break;
      case SCREEN_AUTO:
        b = BUTTON_ROW_AUTO;
        break;
      case SCREEN_MDI:
        b = BUTTON_ROW_MDI;
        break;
      default:
        b = BUTTON_ROW_NONE;
    }
  }
  if ( !isButtonRow(b) ) {
    previousButtonRow = buttonRow;
    buttonRow = b;
    return true;
  }
  return false;
}


void ManualmaticState::incrementJogIncrement(int16_t incr) {
  currentJogIncrement = min(max(0, currentJogIncrement+incr),3);
}

/**
 * @brief Set the commanded spindle speed
 * 
 * Spindle speed must not increment across 0
 * A positive increment always increases speed in both fwd or reverse
 * Apply a percentage if required.
 * 
 * @param incr Number of increments to apply
 * 
 */
void ManualmaticState::incrementSpindleSpeed(int16_t incr) {

  float spindle_increment = incr * config.spindle_increment;

  if ( config.spindle_increment < 1 ) {
      // Multiply by spindle_increment, incr times (negative incr means multiplication by 1/spindle_increment)
      spindle_increment = abs(spindleSpeed) * (powf(1 + config.spindle_increment, incr) - 1);
      // Always increment by at least +/- incr
      if (abs(spindle_increment) < abs(incr)) spindle_increment = incr;
  }
  if ( spindleSpeed > 0 ) {
    spindleSpeed = spindleSpeed + spindle_increment;
    spindleSpeed = min(max(spindleSpeed,1), (config.max_spindle_speed/spindleOverride));
  } else if ( spindleSpeed < 0 ) {    
    spindleSpeed = spindleSpeed - spindle_increment;
    spindleSpeed = max(min(spindleSpeed, -1), ((config.max_spindle_speed/spindleOverride)*-1));
  }
}

/**
 * Reset the spindle defaults (RPM & percent)
 */
void ManualmaticState::resetSpindleDefaults() {
  spindleOverride = 1;
  if ( isManual() && spindleRpm == 0 ) {
    spindleSpeed = config.default_spindle_speed;
  }
}



void ManualmaticState::setErrorMessage(ErrorMessage_e error) {
  errorMessage = error;
  errorMessageStartTime = now;
}

void ManualmaticState::setIniValue(char cmd1, char* payload) {
  switch (cmd1) {
    case INI_AXES:
      // @TODO check if axes value may be > than actual number of axes (eg XYYZ)
      // see: https://linuxcnc.org/docs/2.8/html/config/ini-config.html#_traj_section [COORDINATES]
      config.axes = atoi(payload);
      displayedAxes = config.axes;
      break;
    case INI_MAX_FEED_OVERRIDE:
      config.max_feed_override = atof(payload);
      break;
    case INI_MIN_SPINDLE_OVERRIDE:
      config.min_spindle_override = atof(payload);
      break;
    case INI_MAX_SPINDLE_OVERRIDE:
      config.max_spindle_override = atof(payload);
      break;
    case INI_DEFAULT_SPINDLE_SPEED:
      config.default_spindle_speed = atof(payload);
      if ( spindleSpeed == 0 ) {
        spindleSpeed = config.default_spindle_speed;
      }
      break;
    case INI_MAX_SPINDLE_SPEED:
      config.max_spindle_speed = atof(payload);
      break;
    case INI_SPINDLE_INCREMENT:
      config.spindle_increment = atof(payload);
      break;
//@TODO
//      self.writeToSerial('iU', format(self.linear_units)) 
//      self.writeToSerial('iu', format(self.angular_units)) 
    case INI_DEFAULT_LINEAR_VELOCITY:
      config.default_linear_velocity = atof(payload);
      //Set default & current jog velocity from ini value
      config.defaultJogVelocity[0] = (config.default_linear_velocity*60)*config.defaultJogTortoisePct/100;
      config.defaultJogVelocity[1] = config.default_linear_velocity*60;
      break;
    case INI_MAX_LINEAR_VELOCITY:
      config.max_linear_velocity = atof(payload);
      config.maxJogVelocity = config.max_linear_velocity*60;
      break;
    case INI_NO_FORCE_HOMING:
      config.noForceHoming = (atoi(payload) == 1);
      break;
    case INI_COMPLETE:
      iniState = INI_STATE_RECEIVED;
      break;
    //default:  
  }
}

/**
 * @brief Check if the machine is on and homed.
 * 
 * By default, set a message if not homed. Can be used silently by
 * passing 'false'.
 * 
 */
bool ManualmaticState::isReady(bool setMessage /*= true*/)  {
  if ( task_state != STATE_ON ) {
    return false;
  }
  if ( !config.noForceHoming && !isHomed() ) {
    if ( setMessage ) {
      setErrorMessage(ERRMSG_NOT_HOMED);
    }
    return false;
  }
  return true;
}

void ManualmaticState::onConnected() {
  iniState = INI_STATE_CONNECTED;
}

void ManualmaticState::onDisconnected() {
  iniState = INI_STATE_DISCONNECTED;
  lastHeartbeatReceived = 0;
  lastHeartbeatSent = 0;
  setScreen(SCREEN_SPLASH);
}