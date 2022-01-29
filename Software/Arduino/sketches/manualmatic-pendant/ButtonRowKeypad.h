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

class ButtonRowKeypad : public DisplayTouchKeypad {

  public:
    //@TODO When state becomes a class, pass state instance in constructor
    ButtonRowKeypad(Adafruit_GFX& d, TouchScreen& touchscreen, State_s& state)
        : DisplayTouchKeypad(d, touchscreen, DisplayArea(0,205,320,36), 1, 5), _state(state) {
    }

    /**
     * Override update 
     * @TODO Update flood key when on
     */
    void update(uint16_t touchUpdateMs = 10) {
      switch ( static_cast<ButtonRow_e>(userId()) ) {
        case BUTTON_ROW_MANUAL:
          //Make keys 'stale' on change of program state
          key(0,0).setUserState(_state.currentAxis != AXIS_NONE ? BUTTON_STATE_ACTIVE : BUTTON_STATE_INACTIVE); //Zero offset           
          //key(0,1).setUserState(_state.currentAxis != AXIS_NONE ? BUTTON_STATE_ACTIVE : BUTTON_STATE_INACTIVE); //Half
          break;
        case BUTTON_ROW_AUTO:
          //Make keys 'stale' on change of program_state
          key(0,0).setUserState(_state.program_state); //Halt
          key(0,3).setUserState(_state.program_state); //One step 
          key(0,4).setUserState(_state.program_state); //Play or pause
          // Switch play or pause if required
          if ( _state.isProgramState(PROGRAM_STATE_RUNNING) ) {
            key(0,4).setUserId(BUTTON_PAUSE);
          } else {
            key(0,4).setUserId(BUTTON_PLAY);
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
    void updateAndDraw(uint16_t displayRefreshMs=100, uint16_t touchUpdateMs = 10) {
      update(touchUpdateMs);
      draw(displayRefreshMs);
    }
    
    
  private:
    State_s& _state;

};
