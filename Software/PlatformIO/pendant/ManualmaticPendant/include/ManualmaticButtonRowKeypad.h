/**
 * @file ManualmaticButtonRowKeypad.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief Extends DisplayTouchKeypad to create a 1x5 TouchKeypad providing both
 * labels and an alternative input method for the physical button row
 * buttons.
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */

#ifndef ManualmaticButtonRowKeypad_h
#define ManualmaticButtonRowKeypad_h

#include "Adafruit_GFX.h"
#include "TouchScreen.h"

#include "ManualmaticState.h"
#include "TouchKeypad.h"

/**
 * @brief Displays the labels for the physical buttons and
 * also responds to touch on those labels.
 * 
 * 
 */
class ManualmaticButtonRowKeypad : public DisplayTouchKeypad {

  public:
    ManualmaticButtonRowKeypad(Adafruit_GFX& d, TouchScreen& touchscreen, ManualmaticState& state);

    /**
     * Override update 
     */
    void update(uint16_t touchUpdateMs = 10);
    
    /**
     * Not currently calling this but required if either update() or draw() are overriden
     * to make sure we call overriden versions of methods.
     */
    void updateAndDraw(uint16_t displayRefreshMs=100, uint16_t touchUpdateMs = 10);
    
   private:
     ManualmaticState& state;
     void setMistFloodState();

};


#endif //ManualmaticButtonRowKeypad_h
