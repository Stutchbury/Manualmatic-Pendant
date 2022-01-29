/**
 * While the State_s struct holds the state and has a few methods to interrogate
 * state, these are most of the functions that either change state of take action 
 * based on state changes.
 * 
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */

/**
 * 
 */
void incrementJogIncrement(int16_t incr) {
  state.currentJogIncrement = min(max(0, state.currentJogIncrement+incr),3);
}



/**
 * All these setButton* functions should probably reside in ButtonRowKeypad class
 */
void unsetButtonRow() {
  for (uint8_t col=0; col<5; col++ ) {
    brkp.key(0,col).enable(false);
    brkp.key(0,col).setUserId(BUTTON_NONE);
    setRowButtonType(col, BUTTON_NONE);
  }
}

/**
 * 
 */
void setButtonRowManual(ButtonRow_e b) {
  unsetButtonRow();
  brkp.setUserId(b);
  brkp.key(0,0).enable();
  brkp.key(0,2).enable();
    
  brkp.key(0,0).setUserId(BUTTON_TOUCHOFF);
  brkp.key(0,2).setUserId(BUTTON_COOLANT);
  
  setRowButtonType(0, BUTTON_TOUCHOFF);
  //setRowButtonType(1, BUTTON_HALF);
  setRowButtonType(2, BUTTON_COOLANT);
  //Enable the keypad
  brkp.enable();
}

/**
 * 
 */
void setButtonRowAuto(ButtonRow_e b) {
// Halt, None, Coolant, One Step, Play/Pause
  unsetButtonRow();
  brkp.setUserId(b);
  brkp.key(0,0).enable();
  brkp.key(0,2).enable();
  brkp.key(0,3).enable();
  brkp.key(0,4).enable();


  brkp.key(0,0).setUserId(BUTTON_HALT);
  brkp.key(0,2).setUserId(BUTTON_COOLANT);
  brkp.key(0,3).setUserId(BUTTON_ONE_STEP);
  brkp.key(0,4).setUserId(BUTTON_PLAY);

  setRowButtonType(0, BUTTON_HALT);
  setRowButtonType(2, BUTTON_COOLANT);
  setRowButtonType(3, BUTTON_ONE_STEP);
  setRowButtonType(4, BUTTON_PLAY);

  //Enable the keypad
  brkp.enable();
}

void setButtonRowCancelOrStart() {  
//  state.rowButton[0] = BUTTON_CANCEL;
//  state.rowButton[4] = BUTTON_PLAY;  
}


void setButtonRowCancelOrStop(ButtonRow_e b) {
  unsetButtonRow();
  brkp.setUserId(b);
  brkp.key(0,0).enable();
  brkp.key(0,4).enable();
  brkp.key(0,0).setUserId(BUTTON_CANCEL);
  brkp.key(0,4).setUserId(BUTTON_STOP);
  setRowButtonType(0, BUTTON_CANCEL);
  setRowButtonType(4, BUTTON_STOP);
  //Enable the keypad
  brkp.enable();
}

void setButtonRowCancelOrTick(ButtonRow_e b) {
  unsetButtonRow();
  brkp.setUserId(b);
  brkp.key(0,0).enable();
  brkp.key(0,4).enable();
  brkp.key(0,0).setUserId(BUTTON_CANCEL);
  brkp.key(0,4).setUserId(BUTTON_TICK);
  setRowButtonType(0, BUTTON_CANCEL);
  setRowButtonType(4, BUTTON_TICK);
  //Enable the keypad
  brkp.enable();
}

void setButtonRowCancelOrTickG5x(ButtonRow_e b) {  
  unsetButtonRow();
  //Only Set the physical buttons.
  setRowButtonType(0, BUTTON_CANCEL);
  setRowButtonType(4, BUTTON_TICK);
}

/**
 * 
 */
void setButtonRowCancelOrPlay(ButtonRow_e b) {
  unsetButtonRow();
  brkp.setUserId(b);
  brkp.key(0,0).enable();
  brkp.key(0,4).enable();

  brkp.key(0,0).setUserId(BUTTON_CANCEL);
  brkp.key(0,4).setUserId(BUTTON_PLAY);
  setRowButtonType(0, BUTTON_CANCEL);
  setRowButtonType(4, BUTTON_PLAY);

  //Enable the keypad
  brkp.enable();

}




//void setRowButtonState(ButtonType_e t, ButtonState_e s) {
//  for ( uint8_t b=0; b<5; b++ ) {
//    if ( state.rowButton[b] == t ) { state.rowButtonState[b] = s; }
//  }
//}

//void setButtonAxisState() {
//  setRowButtonState(BUTTON_TOUCHOFF, state.currentAxis == AXIS_NONE ? BUTTON_STATE_INACTIVE : BUTTON_STATE_ACTIVE);
//  setRowButtonState(BUTTON_HALF, state.currentAxis == AXIS_NONE ? BUTTON_STATE_INACTIVE : BUTTON_STATE_ACTIVE);  
//}

bool setButtonRow(ButtonRow_e b=BUTTON_ROW_DEFAULT ) {
  brkp.enable(false);
  if ( b == BUTTON_ROW_DEFAULT ) {
    switch ( state.screen ) {
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
  if ( !state.isButtonRow(b) ) {    
    //Reset @TODO maybe not required now?
//    for ( uint8_t b=0; b<5; b++ ) {
//      state.rowButton[b] = BUTTON_NONE;
//      state.rowButtonState[b] = BUTTON_STATE_ACTIVE; //default
//    }
    state.previousButtonRow = state.buttonRow;
    state.buttonRow = b;

    //
    switch ( b ){
      case BUTTON_ROW_MANUAL:
        setButtonRowManual(b);
        break;
      case BUTTON_ROW_AUTO:
        setButtonRowAuto(b);
        break;
      case BUTTON_ROW_MDI:
        break;
      case BUTTON_ROW_SPINDLE_START:
        setButtonRowCancelOrTick(b);
        break;
      case BUTTON_ROW_SPINDLE_STOP:
        setButtonRowCancelOrStop(b);
        break;
      case BUTTON_ROW_PROGRAM_START:
        setButtonRowCancelOrPlay(b);                
        break;
      case BUTTON_ROW_NONE:
        break;
      case BUTTON_ROW_DEFAULT:
        break;
      case BUTTON_ROW_G5X_OFFSET:
        setButtonRowCancelOrTickG5x(b);
      }
    return true;
  }
  return false;
}

void previousButtonRow() {
  setButtonRow(state.previousButtonRow);  
}



bool setScreen(Screen_e s ) {
  if ( !state.isScreen(s) ) {
    state.previousScreen = state.screen;
    state.screen = s;
    setButtonRow(BUTTON_ROW_DEFAULT);
    state.refreshDisplay = true;
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

void previousScreen() {
  state.screen = state.previousScreen;  
}

bool setTaskMode(Task_mode_e mode, bool force=false) {
  if ( !force && (state.isTaskMode(mode) || !state.isTaskState(STATE_ON)) ) {
    return false;
  }
  state.task_mode = mode;
  state.currentAxis = AXIS_NONE;
  //state.spindleArmed = false;
  switch (state.task_mode) {    
    case MODE_MANUAL:
      setScreen(SCREEN_MANUAL);
      //setButtonAxisState();
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
  state.refreshDisplay = true;
  return true;
}


void setTaskState(Task_state_e s) {
  //Always set the state
  state.task_state = s;
  switch (state.task_state) {    
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

/**
 * Set the displayed spindle RPM (manual mode)
 */
void setSpindleRpm(int16_t incr) {
  state.spindleRpm = state.spindleRpm + (incr * 10);
  if ( state.currentSpindleDir == 1 ) {
    state.spindleRpm = max(state.spindleRpm,0);
  } else if ( state.currentSpindleDir == -1 ) {
    state.spindleRpm = min(state.spindleRpm, 0);
  }
}


/**
 * Calculate the displayed values for feed and rapid based on motion_type & current_vel
 */
void setCurrentVelocities() {
  //Only set values if auto or mdi and mode type is traverse, feed or arc.
  if ( state.isTaskMode(MODE_AUTO) || state.isTaskMode(MODE_MDI) ) {
    if ( state.motion_type == MOTION_TYPE_TRAVERSE ) { //Rapid
      state.feed_vel = 0;
      state.rapid_vel = state.current_vel*60;
    } else if ( state.motion_type == MOTION_TYPE_FEED || state.motion_type == MOTION_TYPE_ARC ) { //Feed
      state.rapid_vel = 0;
      state.feed_vel = state.current_vel*60;
    } else { 
      state.feed_vel = 0;
      state.rapid_vel = 0;
    }
  } else {
    state.feed_vel = 0;
    state.rapid_vel = 0;    
  }    
}

void setIniValue(char cmd1, char* payload) {
  switch (cmd1) {
    case INI_AXES:
      // @TODO check if axes value may be > than actual number of axes (eg XYYZ)
      // see: https://linuxcnc.org/docs/2.8/html/config/ini-config.html#_traj_section [COORDINATES]
      state.axes = atoi(payload);
      state.displayedAxes = state.axes;
      break;
    case INI_MAX_FEED_OVERRIDE:
      state.max_feed_override = atof(payload);
      break;
    case INI_MIN_SPINDLE_OVERRIDE:
      state.min_spindle_override = atof(payload);
      break;
    case INI_MAX_SPINDLE_OVERRIDE:
      state.max_spindle_override = atof(payload);
      break;
    case INI_DEFAULT_SPINDLE_SPEED:
      state.default_spindle_speed = atof(payload);
      state.spindleRpm = state.default_spindle_speed;
      break;
//@TODO
//      self.writeToSerial('iU', format(self.linear_units)) 
//      self.writeToSerial('iu', format(self.angular_units)) 
    case INI_DEFAULT_LINEAR_VELOCITY:
      state.default_linear_velocity = atof(payload);
      //Set default & current jog velocity from ini value
      state.defaultJogVelocity[0] = (state.default_linear_velocity*60)/20;
      state.defaultJogVelocity[1] = state.default_linear_velocity*60;
      state.jogVelocity[0] = state.defaultJogVelocity[0];
      state.jogVelocity[1] = state.defaultJogVelocity[1];
      break;
    case INI_MAX_LINEAR_VELOCITY:
      state.max_linear_velocity = atof(payload);
      state.maxJogVelocity = state.max_linear_velocity*60;
      break;
    //default:  
  }
}


/**
 * Read from serial and set state if message received
 */
void updateState() {
  char cmd[2];
  char payload[30];
    if ( serialMessage.available(cmd, payload) ) {  
    switch ( cmd[0] ) {
      case CMD_ABSOLUTE_POS:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          state.axisAbsPos[((int)cmd[1])-48] = atof(payload); //There's probably a better way than -48...   
        }
        break;
      case CMD_CURRENT_VEL:
          state.current_vel = atof(payload);
          setCurrentVelocities();
        break;
      case CMD_MOTION_TYPE:
        state.motion_type = static_cast<Motion_type_e>(cmd[1]-'0');
        setCurrentVelocities();
        break;
      case CMD_SPINDLE_SPEED:
        state.spindleSpeed = atof(payload);
        break;
      case CMD_SPINDLE_OVERRIDE:
        state.spindleOverride = atof(payload);
        break;
  //    case CMD_MAX_FEED_OVERRIDE: //@TODO move to ini?
  //      state.feedSpeed = atof(payload);
  //      break;
      case CMD_FEED_OVERRIDE:
        state.feedrate = atof(payload);
        break;
      case CMD_RAPID_SPEED: // @TODO not used?
        state.rapidSpeed = atof(payload);
        break;
      case CMD_RAPID_OVERRIDE:
        state.rapidrate = atof(payload);
        break;
      case CMD_JOG_VELOCITY:
          state.jogVelocity[state.jogVelocityRange] = atof(payload);
        break;
      case CMD_TASK_MODE:
        //setTaskMode( static_cast<Task_mode_e>(atoi(payload)));
        setTaskMode( static_cast<Task_mode_e>(cmd[1]-'0'));
        break;
      case CMD_TASK_STATE:
        if ( state.task_state == STATE_INIT ) { //We've just started up
          sendInit();
        }
        setTaskState(static_cast<Task_state_e>(cmd[1]-'0'));
        break;
      case CMD_INTERP_STATE:
        state.interpState = static_cast<Interp_e>(atoi(payload));
        break;
      case CMD_EXEC_STATE:
        state.exec_state = static_cast<Exec_state_e>(cmd[1]-'0');
        break;
      case CMD_PROGRAM_STATE:
        state.program_state = static_cast<Program_state_e>(cmd[1]-'0');
        break;      
  //    case CMD_AXES: // @TODO move to ini
  //      // @TODO check if axes value may be > than actual number of axes (eg XYYZ)
  //      // see: https://linuxcnc.org/docs/2.8/html/config/ini-config.html#_traj_section [COORDINATES]
  //      state.axes = atoi(payload);
  //      state.displayedAxes = state.axes;
  //      break;
      case CMD_G5X_OFFSET:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          state.g5xOffsets[((int)cmd[1])-48] = atof(payload); //There's probably a better way than -48...  
        } 
        break;
      case CMD_DTG:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          state.axisDtg[((int)cmd[1])-48] = atof(payload); //There's probably a better way than -48...   
        }
        break;
      case CMD_HOMED:
        if ( strchr("012345678", cmd[1]) != NULL ) {
          //state.homed[((int)cmd[1])-48] = atoi(payload); //There's probably a better way than -48... 
          state.homed[cmd[1]-'0'] = atoi(payload); 
        }  
        break;
      case CMD_ALL_HOMED:
        state.all_homed = cmd[1]-'0';
        break;
      case CMD_INI_VALUE:
        //Hand off cmd[1] to setIniValue
        setIniValue(cmd[1], payload);
        break;
      case CMD_FLOOD:
        state.flood = static_cast<Flood_e>(cmd[1]-'0');
        break;
      case CMD_MIST:
        state.mist = static_cast<Mist_e>(cmd[1]-'0');
        break;
    }
  }
}
