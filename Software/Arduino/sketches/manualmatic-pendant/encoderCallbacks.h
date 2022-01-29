/**
 * 
 * Primarily handlers for the encoder callbacks.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */


/** *********************************************************************
 * Define all the encoder callback functions are here.
 */

/**
 * 
 */
void onMpgEncoder(EncoderButton& rb) {
  if ( state.isReady() && state.isManual() && state.currentAxis != AXIS_NONE ) {
    if ( state.isScreen(SCREEN_MANUAL) ) {
      jogAxis(state.currentAxis, (state.jogIncrements[state.currentJogIncrement] * rb.increment()) );
    }
  }
}
/**
 * 
 */
void onFeedPressedEncoder(EncoderButton& rb) {
  if ( state.task_mode == MODE_MANUAL ) {
    incrementJogIncrement(rb.increment());
  } else {
    incrementRapidrate(rb.increment() * abs(rb.increment())); //Accelerate
  }
}
/**
 * 
 */
void onFeedEncoder(EncoderButton& rb) {
  if ( state.isReady() && state.isManual() ) {
    incrementJogVelocity(rb.increment() * abs(rb.increment())); //Accelerate
  } else {
    incrementFeedrate(rb.increment() * abs(rb.increment())); //Accelerate
  }  
}

/**
 * 
 */
void onFeedClicked(EncoderButton& rb) {
  if ( state.isReady() && state.isManual() ) {
    toggleJogRange();
  }
}

/**
 * 
 */
void onFeedLongPress(EncoderButton& rb) {
  if ( state.isTaskMode(MODE_MANUAL) && state.currentOperation == OPERATION_NONE) {
    resetJogVelocity();
  } else if ( state.isTaskMode(MODE_AUTO) || state.isTaskMode(MODE_MDI) ) {
    sendFeedrate(); //no arg resets to 100%
    sendRapidrate(); //no arg resets to 100%
  }
}


void onSpindleEncoder(EncoderButton& rb) {
  int16_t incr = rb.increment() * abs(rb.increment()); //Accelerate
  if ( state.isReady() && !state.isAuto() && state.spindleSpeed == 0 ) { // && state.spindleArmed == true ) {
    setSpindleRpm(incr);
  } else {
    incrementSpindleOverride(incr);
  }
}


void onSpindleClicked(EncoderButton& rb) {
  if ( state.isReady() && !state.isAuto() && state.spindleSpeed != 0 ) { //&& isManual() ) {
    setButtonRow(BUTTON_ROW_SPINDLE_STOP);
    //stopSpindle();
  } else if ( state.isButtonRow(BUTTON_ROW_SPINDLE_START) ) {
      //Do same as cancel
      setButtonRow(BUTTON_ROW_DEFAULT);
  }  
}

void onSpindleLongPressed(EncoderButton& rb) {
  if ( state.isReady() ) {
    if ( state.isManual() && state.spindleSpeed == 0 ) {
      state.spindleRpm = state.default_spindle_speed;
    }
    sendSpindleOverride(); //Reset to 100%
  }
}

void onSpindleDoubleClicked(EncoderButton& rb) {
  if ( state.isReady() && !state.isAuto() ) {
    //Only arm if no other buttons in use and spindle is stopped
    if ( state.isButtonRow(BUTTON_ROW_MANUAL) && state.spindleSpeed == 0 ) {      
      setButtonRow(BUTTON_ROW_SPINDLE_START);
    } else if ( state.isButtonRow(BUTTON_ROW_SPINDLE_START) ) {
      //Do same as cancel
      setButtonRow(BUTTON_ROW_DEFAULT);
    }
  }
}

    
/*
  if (state.currentAxis == -1 ) return;
  
  int c = mpg.encoderCount();
  int8_t dir = mpg.direction(); // == DIR_CW ? 1 : -1;
  drawn.axisAbsPos[state.currentAxis] = state.axisAbsPos[state.currentAxis];
  
  float p = (dir*1*jogIncrements[currentJogIncrement]);
  state.axisAbsPos[state.currentAxis] += p;
  drawAxisValues(false);

  
  mpg.direction() == DIR_CW ? mpgPos += c : mpgPos -= c;
  String d = (mpg.direction() == DIR_CW) ? ">" : "<";
  Serial.print(mpgPos);
  for (int i=0; i<c; i++ ) {
    Serial.print(d);
  }
  Serial.println();
*/
