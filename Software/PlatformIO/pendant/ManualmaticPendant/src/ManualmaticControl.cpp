#include "ManualmaticControl.h"


ManualmaticControl::ManualmaticControl(ManualmaticMessage& serialMessage, 
                                      ManualmaticConfig& config, 
                                      ManualmaticState& s, 
                                      ManualmaticButtonRowKeypad& brkp,
                                      ManualmaticOffsetKeypad& okp
                                      )
    : message(serialMessage),
      config(config),
      state(s),
      brkp(brkp),
      okp(okp),
      messenger(serialMessage, config, s), 
      feed(ENCODER_A_FEED, ENCODER_B_FEED, BUTTON_FEED),
      mpg(ENCODER_A_MPG, ENCODER_B_MPG),
      spindle(ENCODER_A_SPINDLE, ENCODER_B_SPINDLE, BUTTON_SPINDLE),
      buttonOnOff(BUTTON_ON_OFF),
      buttonX(BUTTON_X),
      buttonY(BUTTON_Y),
      buttonZ(BUTTON_Z),
      buttonA(BUTTON_A),
      buttonMode(BUTTON_MODE),
      buttonModifier(BUTTON_MODIFIER),
      rowButtons { 
        EventButton(BUTTON_ROW_0),
        EventButton(BUTTON_ROW_1),
        EventButton(BUTTON_ROW_2),
        EventButton(BUTTON_ROW_3),
        EventButton(BUTTON_ROW_4)
      },
      joystick(JOYSTICK_X, JOYSTICK_Y),
      buttonJoystick(BUTTON_JOYSTICK)
{ }
/** ********************************************************************** */
void ManualmaticControl::begin() {
  //pinMode(SOFT_ESTOP, INPUT_PULLUP);
  estopSwitch.attach(SOFT_ESTOP, INPUT_PULLUP);
  setupEncoders();
  setupButtons();
  setupButtonRowKeypad();
  setupOffsetKeypad();
  setupJoystick();
  checkEstop(true);
}
/** ********************************************************************** */
void ManualmaticControl::update() {
  checkEstop();
  if ( state.iniState == 1 ) {
    onIniReceived();
  }
  char cmd[2];
  char payload[30];
  if ( message.available(cmd, payload)) {
    state.update(cmd, payload);
  }
  //Check if button row is different to state button row
  if (buttonRow != state.buttonRow) {
    setupButtonRow(state.buttonRow);
    buttonRow = state.buttonRow;
  }
  mpg.update();
  feed.update();
  spindle.update();
  buttonOnOff.update();
  buttonX.update();
  buttonY.update();
  buttonZ.update();
  buttonA.update();
  buttonMode.update();
  okp.update();  //Do draw() in updateDisplay()
  brkp.update(); //Ditto
  updateButtonRow();
  joystick.update();
  buttonJoystick.update();
  buttonModifier.update();
  now = millis();
  
  if ( now > lastHeartbeat + heartbeatMs ) {
    lastHeartbeat = now;
    messenger.sendHeartbeat();
  }

}
/** ********************************************************************** */
void ManualmaticControl::setupEncoders() {
  //Configure the encoders
  feed.setRateLimit(feedRateLimit);
  feed.setEncoderHandler([&](EncoderButton &eb) { onFeedEncoder(eb); } );
  feed.setEncoderPressedHandler([&](EncoderButton &eb) { onFeedPressedEncoder(eb); } );
  feed.setClickHandler([&](EncoderButton &eb) { onFeedClicked(eb); } );
  feed.setLongPressHandler([&](EncoderButton &eb) { onFeedLongPress(eb); } );

  spindle.setRateLimit(spindleRateLimit);
  spindle.setEncoderHandler([&](EncoderButton &eb) { onSpindleEncoder(eb); } );
  spindle.setClickHandler([&](EncoderButton &eb) { onSpindleClicked(eb); } );
  spindle.setDoubleClickHandler([&](EncoderButton &eb) { onSpindleDoubleClicked(eb); } );
  spindle.setLongPressHandler([&](EncoderButton &eb) { onSpindleLongPressed(eb); } );

  mpg.setRateLimit(mpgRateLimit);
  mpg.setEncoderHandler([&](EncoderButton &eb) { onMpgEncoder(eb); } );
}
/** ********************************************************************** */
void ManualmaticControl::setupButtons() {
  buttonOnOff.setClickHandler([&](EventButton &btn) { onOffClicked(btn); });
  buttonOnOff.setLongPressHandler([&](EventButton &btn) { onOnOffLongPress(btn); });
  buttonOnOff.setLongClickDuration(longClickDuration);

  buttonX.setClickHandler([&](EventButton &btn) { toggleXSelected(btn); });  
  buttonX.setLongClickDuration(longClickDuration);

  buttonY.setClickHandler([&](EventButton &btn) { toggleYSelected(btn); });  
  buttonY.setLongClickDuration(longClickDuration);
  
  buttonZ.setClickHandler([&](EventButton &btn) { toggleZSelected(btn); });  
  buttonZ.setLongClickDuration(longClickDuration);
  
  
  buttonA.setClickHandler([&](EventButton &btn) { toggleASelected(btn); });  
  buttonA.setDoubleClickHandler([&](EventButton &btn) { toggleDisplayAbsG5x(btn); });
  buttonA.setTripleClickHandler([&](EventButton &btn) { displayDtg(btn); });
  buttonA.setLongPressHandler([&](EventButton &btn) { onButtonALongPressed(btn); });
  buttonA.setLongClickDuration(longClickDuration);
  
  buttonMode.setClickHandler([&](EventButton &btn) { onButtonModeClicked(btn); });
  buttonMode.setLongClickDuration(longClickDuration);

  for ( uint8_t b=0; b<5; b++ ) {
    rowButtons[b].setUserId(BUTTON_NONE);
    rowButtons[b].setLongClickDuration(longClickDuration);
    //Set all buttons in button row to same handler
    rowButtons[b].setClickHandler([&](EventButton &btn) { onButtonRowButtonClicked(btn); });
    rowButtons[b].setDoubleClickHandler([&](EventButton &btn) { onButtonRowDoubleClicked(btn); });
  }

  buttonModifier.setPressedHandler([&](EventButton &btn) { onButtonModifierPressed(btn); });
  buttonModifier.setReleasedHandler([&](EventButton &btn) { onButtonModifierReleased(btn); });


}

void ManualmaticControl::setupButtonRowKeypad() {
  brkp.enable(false);
  for (uint8_t col=0; col<5; col++) {
    brkp.key(0,col).setTouchedHandler([&](TouchKey &key) { onButtonRowTouched(key); });
    //Draw handlers are set in ManualmaticDisplay
  }
}


void ManualmaticControl::setupOffsetKeypad() {
  okp.enable(false); 
  okp.key(3,0).setTouchedHandler([&](TouchKey &key) { onTouchCancelG5xOffset(key); });
  okp.key(3,4).setTouchedHandler([&](TouchKey &key) { onTouchSetG5xOffset(key); });
  //Draw handlers are set in ManualmaticDisplay
}

void ManualmaticControl::setupJoystick() {
  joystick.setRateLimit(config.joystickRateLimit);
  joystick.x.setStartBoundary(config.joystickStartBoundary);
  joystick.y.setStartBoundary(config.joystickStartBoundary);
  joystick.x.setEndBoundary(config.joystickEndBoundary);
  joystick.y.setEndBoundary(config.joystickEndBoundary);
  joystick.setNumIncrements(config.numJoystickIncrements);
  joystick.x.setChangedHandler([&](EventAnalog &js) { onJoystickXChanged(js); });
  joystick.y.setChangedHandler([&](EventAnalog &js) { onJoystickYChanged(js); });
  joystick.setIdleHandler([&](EventJoystick &js) { onJoystickIdle(js); });
  //Disable the joystick but allow callibration (everyone plays with the joystick!)
  joystick.enable(false, true);
  buttonJoystick.setClickHandler([&](EventButton &btn) { onJoystickClicked(btn); });
  buttonJoystick.setDoubleClickHandler([&](EventButton &btn) { onJoystickDoubleClicked(btn); });
}




/** ********************************************************************** */
void ManualmaticControl::checkEstop(bool force /*=false*/) {
  if ( config.useSoftEstop) {
    estopSwitch.update();
    if ( estopSwitch.changed() || force) {
      state.estop_is_activated = estopSwitch.read();
      messenger.setMachineState(state.estop_is_activated ? STATE_ESTOP : STATE_ESTOP_RESET);
    //Ensure the screen reflects the state of the local button and linuxcnc
      if ( !state.estop_is_activated 
          && state.isTaskState(STATE_ESTOP_RESET)
          && state.isScreen(SCREEN_ESTOP) ) {
        state.setScreen(SCREEN_ESTOP_RESET);
      }
    }
    //Ensure the screen reflects the state of the local button
    if ( state.estop_is_activated && !state.isScreen(SCREEN_ESTOP) ) {
      state.setScreen(SCREEN_ESTOP);
    }
  }
}

void ManualmaticControl::onIniReceived() {
  //Send pendant state back  
  checkEstop(true);
  //messenger.setMachineState(digitalRead(SOFT_ESTOP) == HIGH ? STATE_ESTOP : STATE_ESTOP_RESET); 
  state.iniState = 2;
}

void ManualmaticControl::onFeedEncoder(EncoderButton& rb) {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isManual() ) {
    messenger.incrementJogVelocity(rb.increment() * abs(rb.increment())); //Accelerate
  } else {
    messenger.incrementFeedrate(rb.increment() * abs(rb.increment())); //Accelerate
  }  
}
/**
 * 
 */
void ManualmaticControl::onFeedPressedEncoder(EncoderButton& rb) {
  if ( state.task_mode == MODE_MANUAL ) {
    state.incrementJogIncrement(rb.increment());
  } else {
    messenger.incrementRapidrate(rb.increment() * abs(rb.increment())); //Accelerate
  }
}

void ManualmaticControl::onFeedClicked(EncoderButton& rb) {
  if ( state.isReady() && state.isManual() ) {
    messenger.toggleJogRange();
  }
}

void ManualmaticControl::onFeedLongPress(EncoderButton& rb) {
  if ( state.isTaskMode(MODE_MANUAL) && state.currentOperation == OPERATION_NONE) {
    messenger.resetJogVelocity();
  } else if ( state.isTaskMode(MODE_AUTO) || state.isTaskMode(MODE_MDI) ) {
    messenger.sendFeedrate(); //no arg resets to 100%
    messenger.sendRapidrate(); //no arg resets to 100%
  }
}

void ManualmaticControl::onSpindleEncoder(EncoderButton& rb) {
  if ( !state.isReady() ) {
    return;
  }
  int16_t incr = rb.increment() * abs(rb.increment()); //Accelerate
  if ( !state.isAuto() && state.spindleSpeed == 0 ) { // && state.spindleArmed == true ) {
    state.setSpindleRpm(incr);
  } else {
    messenger.incrementSpindleOverride(incr);
  }
}

void ManualmaticControl::onSpindleClicked(EncoderButton& rb) {
  if ( !state.isReady() ) {
    return;
  }
  if ( !state.isAuto() && state.spindleSpeed != 0 ) { //&& isManual() ) {
    state.setButtonRow(BUTTON_ROW_SPINDLE_STOP);
  } else if ( state.isButtonRow(BUTTON_ROW_SPINDLE_START) ) {
      //Do same as cancel
      state.setButtonRow(BUTTON_ROW_DEFAULT);
  }  
}

void ManualmaticControl::onSpindleDoubleClicked(EncoderButton& rb) {
  if ( !state.isReady() ) {
    return;
  }

  if ( !state.isAuto() ) {
    //Only arm if no other buttons in use and spindle is stopped
    if ( state.isButtonRow(BUTTON_ROW_MANUAL) && state.spindleSpeed == 0 ) {      
      state.setButtonRow(BUTTON_ROW_SPINDLE_START);
    } else if ( state.isButtonRow(BUTTON_ROW_SPINDLE_START) ) {
      //Do same as cancel
      state.setButtonRow(BUTTON_ROW_DEFAULT);
    }
  }
}

void ManualmaticControl::onSpindleLongPressed(EncoderButton& rb) {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isManual() && state.spindleSpeed == 0 ) {
    state.spindleRpm = config.default_spindle_speed;
  }
  messenger.sendSpindleOverride(); //Reset to 100%
}


void ManualmaticControl::onMpgEncoder(EncoderButton& rb) {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isManual() && state.currentAxis != AXIS_NONE ) {
    if ( state.isScreen(SCREEN_MANUAL) ) {
      messenger.jogAxis(state.currentAxis, (config.jogIncrements[state.currentJogIncrement] * rb.increment()) );
    }
  }
}


void ManualmaticControl::onTouchCancelG5xOffset(TouchKey& tkcb) {
  onCancelG5xOffset();
}

void ManualmaticControl::onTouchSetG5xOffset(TouchKey& tkcb) {
  onSetG5xOffset();  
}


void ManualmaticControl::onButtonRowTouched(TouchKey& key) {
  onButtonRowClicked(static_cast<ButtonType_e>(key.userId()));
}

/**
 * Update the state of physical button on the button row.
 */
void ManualmaticControl::updateButtonRow() {
  if ( state.errorMessage != ERRMSG_NONE) {
    if ( millis() - state.errorMessageStartTime >= config.errorMessageTimeout ) {
      state.errorMessage = ERRMSG_NONE;
    }
  }
  if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
    // Switch play or pause based on program_state
    if ( state.isProgramState(PROGRAM_STATE_RUNNING) ) {
      setRowButtonType(4, BUTTON_PAUSE);
    } else {
      setRowButtonType(4, BUTTON_PLAY);
    }
  }
  for ( uint8_t b=0; b<5; b++ ) {
    rowButtons[b].update();
  }
}


/** ********************************************************************** */
void ManualmaticControl::onOffClicked(EventButton btn) {
  if ( state.isTaskState(STATE_ESTOP_RESET) || state.isTaskState(STATE_OFF) ) {
    messenger.setMachineState(STATE_ON);
  } else {    
    messenger.sendAbort();
  }
}
/** ********************************************************************** */
void ManualmaticControl::onOnOffLongPress(EventButton& btn) {
  if ( state.isTaskState(STATE_ON) ) {
    messenger.setMachineState(STATE_OFF);
  }
}
/** ********************************************************************** */
void ManualmaticControl::toggleXSelected(EventButton& btn) {
  if ( !state.isReady() ) {
    return;
  }
  toggleSelectedAxis(AXIS_X);
}
/** ********************************************************************** */
void ManualmaticControl::toggleYSelected(EventButton& btn) {
  if ( !state.isReady() ) {
    return;
  }
  toggleSelectedAxis(AXIS_Y);
}
/** ********************************************************************** */
void ManualmaticControl::toggleZSelected(EventButton& btn) {
  if ( !state.isReady() ) {
    return;
  }
  toggleSelectedAxis(AXIS_Z);
}
/** ********************************************************************** */
void ManualmaticControl::toggleASelected(EventButton& btn) {
  if ( !state.isReady() ) {
    return;
  }
  toggleSelectedAxis(AXIS_A);
}
/** ********************************************************************** */
void ManualmaticControl::toggleDisplayAbsG5x(EventButton& btn) {
  //if ( isScreen(SCREEN_MANUAL) ) {
    if ( state.displayedCoordSystem == 1 ) { //Currently Dtg
      state.displayedCoordSystem = state.prevCoordSystem;
    } else {
      state.displayedCoordSystem = ( state.displayedCoordSystem == 0 ? 2 : 0 );
    }
  //}
}
/** ********************************************************************** */
void ManualmaticControl::displayDtg(EventButton& btn) {
  //if ( isScreen(SCREEN_MANUAL) ) {
    if ( state.displayedCoordSystem != 1 ) {
      state.prevCoordSystem = state.displayedCoordSystem;
      state.displayedCoordSystem = 1;
    } else {
      state.displayedCoordSystem = state.prevCoordSystem;
    }
  //}
}
/** ********************************************************************** */
void ManualmaticControl::toggleSelectedAxis(Axis_e axis) {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isScreen(SCREEN_MANUAL) && !state.isAuto() ) {
    if ( state.currentAxis == axis ) {
      state.currentAxis = AXIS_NONE;
      messenger.jogAxisStop(axis);
    } else if ( state.displayedAxes > axis  ) {
      state.currentAxis = axis;
    }
  }
}
/** ********************************************************************** */
void ManualmaticControl::onButtonALongPressed(EventButton& rb) {
  //if ( isScreen(SCREEN_MANUAL) ) {
    toggleDisplayAAxis(rb);
  //}
}
/** ********************************************************************** */
void ManualmaticControl::toggleDisplayAAxis(EventButton& btn) {
  if ( !state.isScreen(SCREEN_OFFSET_KEYPAD) ) { //Don't allow if setting the offset (or we'll clear the currentAxis)
    if ( state.currentAxis == AXIS_A ) {
      state.currentAxis = AXIS_NONE;
      messenger.jogAxisStop(AXIS_A);
    }
    //Make the currently drawn axis stale so it gets redrawn
    //state.drawn.currentAxis = AXIS_NONE;
    state.currentAxis = AXIS_NONE;
    joystick.enable(false);
    state.joystickAxis[0] = AXIS_NONE;
    state.joystickAxis[1] = AXIS_NONE;
    //@TODO At some point, prevent A axis being shown if machine only has 3 axis...
    state.displayedAxes = (state.displayedAxes == 3 ? 4 : 3);
  }
}
/** ********************************************************************** */
void ManualmaticControl::onButtonModeClicked(EventButton& btn) {
  if ( state.isProgramState(PROGRAM_STATE_STOPPED) || state.isProgramState(PROGRAM_STATE_NONE) ) {
    //uint8_t m = (state.task_mode % 3) + 1; //or (1+x)%3 from 0
    uint8_t m = (state.task_mode % 2) + 1; //Only Manual and Auto (no MDI)
    messenger.sendTaskMode(m);
  }
}
/** ********************************************************************** */
void ManualmaticControl::onButtonRowButtonClicked(EventButton& btn) {
  onButtonRowClicked( static_cast<ButtonType_e>(btn.userId()) );
}
/** ********************************************************************** */
void ManualmaticControl::onButtonRowClicked(ButtonType_e buttonType) {
  switch ( buttonType ) {    
    case BUTTON_NONE:
      break;
    case BUTTON_HALT:
      onButtonHalt();
      break;
    case BUTTON_STOP:
      onButtonStop();
      break;
    case BUTTON_CANCEL:
      onButtonCancel();
      break;
    case BUTTON_TICK:
      onButtonTick();
      break;
    case BUTTON_TOUCHOFF:
      onButtonTouchOff();
      break;
    case BUTTON_HALF:
      //onButtonHalf();
      break;
    case BUTTON_PLAY:
      onButtonPlay();
      break;
    case BUTTON_PAUSE:
      onButtonPause();
      break;
    case BUTTON_ONE_STEP:
      onButtonOneStep();
      break;
    case BUTTON_COOLANT:
      toggleCoolant();
      break;
  }
}
/** ********************************************************************** */
void ManualmaticControl::onButtonRowDoubleClicked(EventButton& btn) {
  switch ( static_cast<ButtonType_e>(btn.userId()) ) {
    case BUTTON_NONE:
      break;
    case BUTTON_HALT:
      break;
    case BUTTON_STOP:
      break;
    case BUTTON_CANCEL:
      break;
    case BUTTON_TICK:
      break;
    case BUTTON_TOUCHOFF:
      break;
    case BUTTON_HALF:
      break;
    case BUTTON_PLAY:
      break;
    case BUTTON_PAUSE:
      break;
    case BUTTON_ONE_STEP:
      break;
    case BUTTON_COOLANT:
      toggleCoolant(true /*double click*/);
      break;
  }
}


void ManualmaticControl::setupButtonRow(ButtonRow_e b=BUTTON_ROW_DEFAULT ) {
  brkp.enable(false);    
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
}


void ManualmaticControl::setRowButtonType(uint8_t col, ButtonType_e bt) {
  rowButtons[col].setUserId(bt);
}

/**
 * All these setButton* functions should probably reside in ButtonRowKeypad class
 */
void ManualmaticControl::unsetButtonRow() {
  for (uint8_t col=0; col<5; col++ ) {
    brkp.key(0,col).enable(false);
    brkp.key(0,col).setUserId(BUTTON_NONE);
    setRowButtonType(col, BUTTON_NONE);
  }
}

void ManualmaticControl::setButtonRowManual(ButtonRow_e b) {
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

void ManualmaticControl::setButtonRowAuto(ButtonRow_e b) {
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

void ManualmaticControl::setButtonRowCancelOrStop(ButtonRow_e b) {
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

void ManualmaticControl::setButtonRowCancelOrTick(ButtonRow_e b) {
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

void ManualmaticControl::setButtonRowCancelOrTickG5x(ButtonRow_e b) {  
  unsetButtonRow();
  //Only Set the physical buttons.
  setRowButtonType(0, BUTTON_CANCEL);
  setRowButtonType(4, BUTTON_TICK);
  brkp.enable(false);
}

/**
 * 
 */
void ManualmaticControl::setButtonRowCancelOrPlay(ButtonRow_e b) {
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


/**
 * 
 */
void ManualmaticControl::onButtonPlay() {
  if ( !state.isReady() ) {
    return;
  }
//  doActionPlay();
  if ( state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
      if ( state.isProgramState(PROGRAM_STATE_STOPPED) ) {
        state.setButtonRow(BUTTON_ROW_PROGRAM_START);
      } else if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        state.setButtonRow(BUTTON_ROW_PROGRAM_START);
      }      
    }    
    else if ( state.isButtonRow(BUTTON_ROW_PROGRAM_START) ) {
      if ( state.isProgramState(PROGRAM_STATE_STOPPED) ) {
        messenger.sendAuto(AUTO_RUN);
        state.setButtonRow(BUTTON_ROW_DEFAULT);
      } else if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        messenger.sendAuto(AUTO_RESUME);
        state.setButtonRow(BUTTON_ROW_DEFAULT);
      }      
    }
  }
}

/**
 * 
 */
void ManualmaticControl::onButtonPause() {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
      if ( state.isProgramState(PROGRAM_STATE_RUNNING) ) {
        messenger.sendAuto(AUTO_PAUSE);
      }
    }    
  }
}

/**
 * 
 */
void ManualmaticControl::onButtonOneStep() {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
      if ( state.isProgramState(PROGRAM_STATE_STOPPED) || state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        messenger.sendAuto(AUTO_STEP);
      }
    }    
  }
}

/**
 * 
 */
void ManualmaticControl::onButtonHalt() {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
      if ( state.isProgramState(PROGRAM_STATE_RUNNING) || state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        messenger.sendAbort();
      }
    }    
  }
}





/**
 * 
 */
void ManualmaticControl::onButtonCancel() {
  if ( !state.isReady() ) {
    return;
  }
  if ( !state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_SPINDLE_START) ) {
      state.setButtonRow(BUTTON_ROW_DEFAULT);
    } else if ( state.isButtonRow(BUTTON_ROW_SPINDLE_STOP) ) {
      state.setButtonRow(BUTTON_ROW_DEFAULT);
    } else if ( state.isButtonRow(BUTTON_ROW_G5X_OFFSET) ) {
      onCancelG5xOffset();
    }
  }
  else if ( state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_PROGRAM_START) ) {
      state.setButtonRow(BUTTON_ROW_DEFAULT);
    }
  }
}


void ManualmaticControl::onButtonTick() {
  if ( !state.isReady() ) {
    return;
  }
  if ( !state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_SPINDLE_START) ) {
      messenger.startSpindle();
      state.setButtonRow(BUTTON_ROW_DEFAULT);
    } else if ( state.isButtonRow(BUTTON_ROW_G5X_OFFSET) ) {
      onSetG5xOffset();  
    } 
  }
}

void ManualmaticControl::onButtonStop() {
  if ( !state.isReady() ) {
    return;
  }
  if ( !state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_SPINDLE_STOP) ) {
      messenger.stopSpindle();
      state.setButtonRow(BUTTON_ROW_DEFAULT);
    }
  }
}

void ManualmaticControl::toggleCoolant(bool doubleClick /*=false*/) {
  if ( !state.isReady() ) {
    return;
  }
  if ( doubleClick ) {
    messenger.sendFlood(state.flood == FLOOD_OFF ? FLOOD_ON : FLOOD_OFF);      
  } else {
    messenger.sendMist(state.mist == MIST_OFF ? MIST_ON : MIST_OFF);
  }
}

/**
 * Setup the touch keypad for g5x offsets
 */
void ManualmaticControl::onButtonTouchOff() {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isButtonRow(BUTTON_ROW_MANUAL) && state.currentAxis != AXIS_NONE ) {
    state.setScreen(SCREEN_OFFSET_KEYPAD);
    okp.enable(); //Enable the offset touch keypad
    brkp.enable(false); //and disable the button row keypad 
  }
}


void ManualmaticControl::onCancelG5xOffset() {
  okp.enable(false);
  state.setScreen(state.previousScreen);
}

void ManualmaticControl::onSetG5xOffset() {
  char payload[20];
  okp.getValueBuffer(payload);
  okp.enable(false);
  state.setScreen(state.previousScreen);
  if ( state.currentAxis != AXIS_NONE  ) {
    messenger.sendG5xOffset(state.currentAxis, payload);
  }
}



void ManualmaticControl::onJoystickXChanged(EventAnalog& ea) {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isManual() ) {
    if ( state.isScreen(SCREEN_MANUAL) && state.joystickAxis[0] != AXIS_NONE ) {
      messenger.jogAxisContinuous(state.joystickAxis[0], (state.jogVelocity[state.jogVelocityRange]/config.numJoystickIncrements) * ea.position());
    }
  }
}

void ManualmaticControl::onJoystickYChanged(EventAnalog& ea) {
  if ( !state.isReady() ) {
    return;
  }
  if ( state.isManual() ) {
    if ( state.isScreen(SCREEN_MANUAL) && state.joystickAxis[1] != AXIS_NONE  ) {
      messenger.jogAxisContinuous(state.joystickAxis[1], (state.jogVelocity[state.jogVelocityRange]/config.numJoystickIncrements) * ea.position());
    }
  }
}

void ManualmaticControl::onJoystickIdle(EventJoystick& ejs) {
    //joystick.enable(false);
}

void ManualmaticControl::onJoystickClicked(EventButton& ejs) {
  if ( !state.isReady() ) {
    return;
  }
  if ( !joystick.enabled() || state.joystickAxis[0] == AXIS_NONE ) {
    joystick.enable(true);
    state.joystickAxis[0] = config.joystickAxisDefault[0];
    state.joystickAxis[1] = config.joystickAxisDefault[1];
  } else {
    joystick.enable(false);
    state.joystickAxis[0] = AXIS_NONE;
    state.joystickAxis[1] = AXIS_NONE;
  }
}

void ManualmaticControl::onJoystickDoubleClicked(EventButton& ejs) {
  if ( !state.isReady() ) {
    return;
  }
  joystick.enable(true);  
  state.joystickAxis[0] = config.joystickAxisAlt[0];
  state.joystickAxis[1] = config.joystickAxisAlt[1];
}

void ManualmaticControl::onButtonModifierPressed(EventButton& rb) {
}

void ManualmaticControl::onButtonModifierReleased(EventButton& rb) {
}
