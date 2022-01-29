/**
 * Primarily the handlers for button events, along with a few associated functions.
 * Note these callbacks are also called by touching the row button labels.
 * 
 * This is where we check state etc before sending messages over serial.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */

/**
 * 
 */
void toggleXSelected(EncoderButton& btn) {
  if ( state.isScreen(SCREEN_MANUAL) && state.isReady() && !state.isAuto() ) {
    state.currentAxis = state.currentAxis == AXIS_X ? AXIS_NONE : AXIS_X;
    //@TODO Move this - should only set state.
    drawAxisMarkers();
  }
}

/**
 * 
 */
void toggleYSelected(EncoderButton& btn) {
  if ( state.isScreen(SCREEN_MANUAL) && state.isReady() && !state.isAuto() ) {
    state.currentAxis = state.currentAxis == AXIS_Y ? AXIS_NONE : AXIS_Y;
    drawAxisMarkers();
  }
}

/**
 * 
 */
void toggleZSelected(EncoderButton& btn) {
  if ( state.isScreen(SCREEN_MANUAL) && state.isReady() && !state.isAuto() ) {
    state.currentAxis = state.currentAxis == AXIS_Z ? AXIS_NONE : AXIS_Z;
    drawAxisMarkers();
  }
}

/**
 * 
 */
void toggleASelected(EncoderButton& btn) {
  if ( state.isScreen(SCREEN_MANUAL) && state.isReady() && !state.isAuto() ) {
    if ( state.axes == 4 ) {
      state.currentAxis = state.currentAxis == AXIS_A ? AXIS_NONE : AXIS_A;
      drawAxisMarkers();
    }
  }
}

/**
 * 
 */
void toggleDisplayAAxis(EncoderButton& btn) {
  if ( !state.isScreen(SCREEN_OFFSET_KEYPAD) ) { //Don't allow if setting the offset (or we'll clear the currentAxis)
    if ( state.currentAxis == AXIS_A ) {
      state.currentAxis = AXIS_NONE;
    }
    //Make the currently drawn axis stale so it gets redrawn
    drawn.currentAxis = AXIS_NONE;
    //@TODO At some point, prevent A axis being shown if machine only has 3 axis...
    state.displayedAxes = (state.displayedAxes == 3 ? 4 : 3);
  }
}

/**
 * 
 */
void toggleDisplayAbsG5x(EncoderButton& btn) {
  //if ( isScreen(SCREEN_MANUAL) ) {
    if ( state.displayedCoordSystem == 1 ) { //Currently Dtg
      state.displayedCoordSystem = state.prevCoordSystem;
    } else {
      state.displayedCoordSystem = ( state.displayedCoordSystem == 0 ? 2 : 0 );
    }
  //}
}

/**
 * 
 */
void displayDtg(EncoderButton& btn) {
  //if ( isScreen(SCREEN_MANUAL) ) {
    if ( state.displayedCoordSystem != 1 ) {
      state.prevCoordSystem = state.displayedCoordSystem;
      state.displayedCoordSystem = 1;
    } else {
      state.displayedCoordSystem = state.prevCoordSystem;
    }
  //}
}

void onOnOffClicked(EncoderButton& btn) {
  if ( state.isTaskState(STATE_ESTOP_RESET) || state.isTaskState(STATE_OFF) ) {
    setMachineState(STATE_ON);
  } else {
    sendAbort();
  }
}

void onOnOffLongPress(EncoderButton& btn) {
  if ( state.isTaskState(STATE_ON) ) {
    //turnMachineOff(btn);
    setMachineState(STATE_OFF);
  }
}

void onButtonModeClicked(EncoderButton& btn) {
  if ( state.isProgramState(PROGRAM_STATE_STOPPED) || state.isProgramState(PROGRAM_STATE_NONE) ) {
    uint8_t m = (state.task_mode % 3) + 1; //or (1+x)%3 from 0
    sendTaskMode(m);
  }
}



//void onButtonAPressed(EncoderButton& rb) {
//  prevMs = millis();
//}

void onButtonALongPressed(EncoderButton& rb) {
  //if ( isScreen(SCREEN_MANUAL) ) {
    toggleDisplayAAxis(rb);
  //}
}



/** ***************************************************************************
 * These are all the configurable button options.
 * Although they're called 'onButtonXXX' they are also fired by the ButtonRowKeypad
 * via onButtonRowClicked()
 * 
 */

void onCancelG5xOffset() {
  okp.enable(false);
  setScreen(state.previousScreen);
}

void onSetG5xOffset() {
  char payload[20];
  okp.getValueBuffer(payload);
  okp.enable(false);
  setScreen(state.previousScreen);
  if ( state.currentAxis != AXIS_NONE  ) {
    sendG5xOffset(state.currentAxis, payload);
  }
}




/**
 * 
 */
void onButtonPlay() {
//  doActionPlay();
  if ( state.isReady() && state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
      if ( state.isProgramState(PROGRAM_STATE_STOPPED) ) {
        setButtonRow(BUTTON_ROW_PROGRAM_START);
      } else if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        setButtonRow(BUTTON_ROW_PROGRAM_START);
      }      
    }    
    else if ( state.isButtonRow(BUTTON_ROW_PROGRAM_START) ) {
      if ( state.isProgramState(PROGRAM_STATE_STOPPED) ) {
        sendAuto(AUTO_RUN);
        setButtonRow(BUTTON_ROW_DEFAULT);
      } else if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        sendAuto(AUTO_RESUME);
        setButtonRow(BUTTON_ROW_DEFAULT);
      }      
    }
  }
}

/**
 * 
 */
void onButtonPause() {
  if ( state.isReady() && state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
      if ( state.isProgramState(PROGRAM_STATE_RUNNING) ) {
        sendAuto(AUTO_PAUSE);
      }
    }    
  }
}

/**
 * 
 */
void onButtonOneStep() {
  if ( state.isReady() && state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
      if ( state.isProgramState(PROGRAM_STATE_STOPPED) || state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        sendAuto(AUTO_STEP);
      }
    }    
  }
}

/**
 * 
 */
void onButtonHalt() {
  if ( state.isReady() && state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
      if ( state.isProgramState(PROGRAM_STATE_RUNNING) || state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        sendAbort();
      }
    }    
  }
}





/**
 * 
 */
void onButtonCancel() {
  if ( state.isReady() && !state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_SPINDLE_START) ) {
      setButtonRow(BUTTON_ROW_DEFAULT);
    } else if ( state.isButtonRow(BUTTON_ROW_SPINDLE_STOP) ) {
      setButtonRow(BUTTON_ROW_DEFAULT);
    } else if ( state.isButtonRow(BUTTON_ROW_G5X_OFFSET) ) {
      onCancelG5xOffset();
    }
  }
  else if ( state.isReady() && state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_PROGRAM_START) ) {
      setButtonRow(BUTTON_ROW_DEFAULT);
    }
  }
}


void onButtonTick() {
  if ( state.isReady() && !state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_SPINDLE_START) ) {
      startSpindle();
      setButtonRow(BUTTON_ROW_DEFAULT);
    } else if ( state.isButtonRow(BUTTON_ROW_G5X_OFFSET) ) {
      onSetG5xOffset();  
    } 
  }
}

void onButtonStop() {
  if ( state.isReady() && !state.isAuto() ) {
    if ( state.isButtonRow(BUTTON_ROW_SPINDLE_STOP) ) {
      stopSpindle();
      setButtonRow(BUTTON_ROW_DEFAULT);
    }
  }
}

void onButtonCoolant(bool doubleClick=false) {
  if ( state.isReady() ) {
    if ( doubleClick ) {
      sendMist(state.mist == MIST_OFF ? MIST_ON : MIST_OFF);
    } else {
      sendFlood(state.flood == FLOOD_OFF ? FLOOD_ON : FLOOD_OFF);      
    }
  }
}

/**
 * Setup the touch keypad for g5x offsets
 */
void onButtonTouchOff() {
  if ( state.isButtonRow(BUTTON_ROW_MANUAL) && state.currentAxis != AXIS_NONE ) {
    setScreen(SCREEN_OFFSET_KEYPAD);
  }
}

/**
 * One of the five buttons on the button row has been clicked
 * Call the function that matches the set button type and
 * it will decide what to do based on state and the 
 * ButtonRow_e context.
 * Also called by ButtonRowKeypad touched callback.
 */
void onButtonRowClicked(ButtonType_e buttonType) {
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
      onButtonCoolant();
      break;
  }
}

/**
 * All the physical buttons on the button row call this 
 * click handler.
 */
void onButtonRowClicked(EncoderButton& btn) {
  onButtonRowClicked( static_cast<ButtonType_e>(btn.userId()) );
}

/**
 * One of the five buttons on the button row has been clicked
 * Call the function that matches the set button type and
 * it will decide what to do based on state and the 
 * ButtonRow_e context.
 */
void onButtonRowDoubleClicked(EncoderButton& btn) {
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
      onButtonCoolant(true /*double click*/);
      break;
  }
}


void setRowButtonType(uint8_t col, ButtonType_e bt) {
  buttonRow[col].setUserId(bt);
}

/**
 * Update the state of physical button on the button row.
 */
void updateButtonRow() {
  if ( state.isButtonRow(BUTTON_ROW_AUTO) ) {
    // Switch play or pause based on program_state
    if ( state.isProgramState(PROGRAM_STATE_RUNNING) ) {
      setRowButtonType(4, BUTTON_PAUSE);
    } else {
      setRowButtonType(4, BUTTON_PLAY);
    }
  }
  for ( uint8_t b=0; b<5; b++ ) {
    buttonRow[b].update();
  }
}
