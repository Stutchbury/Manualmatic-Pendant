/**
 * Extends DisplayTouchKeypad to create a 1x5 TouchKeypad providing both
 * labels and an alternative input method for the physical button row
 * buttons.
 *
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 *
 */

#include "ManualmaticButtonRowKeypad.h"

ManualmaticButtonRowKeypad::ManualmaticButtonRowKeypad(Adafruit_GFX& d, TouchScreen& touchscreen, ManualmaticState& state)
  : DisplayTouchKeypad(d, touchscreen, DisplayArea(0, 205, 320, 36), 1, 5), state(state) {
}

/**
 * Override update
 */
void ManualmaticButtonRowKeypad::update(uint16_t touchUpdateMs /*= 10*/) {
  switch (static_cast<ButtonRow_e>(userId())) {
  case BUTTON_ROW_MANUAL:
    //Make keys 'stale' on change of currentAxis
    key(0, 0).setUserState(state.currentAxis != AXIS_NONE ? BUTTON_STATE_ACTIVE : BUTTON_STATE_INACTIVE); //Zero offset           
    //key(0,1).setUserState(state.currentAxis != AXIS_NONE ? BUTTON_STATE_ACTIVE : BUTTON_STATE_INACTIVE); //Half
    setMistFloodState();
    break;
  case BUTTON_ROW_AUTO:
    //Make keys 'stale' on change of program_state
    key(0, 0).setUserState(state.program_state); //Halt
    setMistFloodState();
    key(0, 3).setUserState(state.program_state); //One step 
    key(0, 4).setUserState(state.program_state); //Play or pause
    // Switch play or pause if required
    if (state.isProgramState(PROGRAM_STATE_RUNNING)) {
      key(0, 4).setUserId(BUTTON_PAUSE);
    }
    else {
      key(0, 4).setUserId(BUTTON_PLAY);
    }
    break;
  default:
    break;
  }

  //Then call the overriden method
  DisplayTouchKeypad::update(touchUpdateMs);
}


/**
 * Not currently calling this but required if either update() or draw() are overriden
 * to make sure we call overriden versions of methods.
 */
void ManualmaticButtonRowKeypad::updateAndDraw(uint16_t displayRefreshMs /*= 100*/, uint16_t touchUpdateMs /*= 10*/) {
  update(touchUpdateMs);
  draw(displayRefreshMs);
}

void ManualmaticButtonRowKeypad::setMistFloodState() {
  key(0, 2).setUserState((state.flood *10) + state.mist);
}