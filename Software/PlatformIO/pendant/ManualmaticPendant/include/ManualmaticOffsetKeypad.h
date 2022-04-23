/**
 * @file ManualmaticOffsetKeypad.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief Extends DisplayTouchKeypad to create a 4x5 TouchKeypad to enter g5x
 * offsets.
 * 
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */
#ifndef ManualmaticOffsetKeypad_h
#define ManualmaticOffsetKeypad_h

#include "Adafruit_GFX.h"
#include "TouchScreen.h"
#include "ManualmaticFonts.h"

#include "ManualmaticState.h"
#include <TouchKeypad.h>

/**
 * @brief Creates a 4x5 TouchKeypad to enter g5x offsets.
 * 
 */
class ManualmaticOffsetKeypad : public DisplayTouchKeypad {

  public:
    /**
     * @brief Construct a new Manualmatic Offset Keypad:: Manualmatic Offset Keypad object
     * 
     * @param d The GFX display
     * @param f The required fornt for alphanumeric key labels
     * @param touchscreen The touchscreen
     * @param state ManulmaticState
     */
    ManualmaticOffsetKeypad(Adafruit_GFX& d, const GFXfont* f, TouchScreen& touchscreen, ManualmaticState& state);

    /**
     * @brief Override draw() to draw labels and displayed value
     * 
     * @param displayRefreshMs 
     */
    void draw(uint16_t displayRefreshMs=100);

    /**
     * @brief We've overriden draw() so must override this
     * 
     * @param displayRefreshMs 
     * @param touchUpdateMs 
     */
    void updateAndDraw(uint16_t displayRefreshMs=100, uint16_t touchUpdateMs = 10);
    
    /**
     * @brief Get the Value Buffer object
     * 
     * @param outBuffer 
     */
    void getValueBuffer(char* outBuffer);

    /**
     * @brief Override to clear screen and force refresh
     * 
     * @param b 
     */
    void enable(bool b=true);

  protected:

    /**
     * Overriding the onKeyTouched. 
     * When a custom callback is not defined/called, update
     * the valueBuffer.
     */
    virtual bool onKeyTouched(TouchKey& key);
  
  private:

    ManualmaticState& state;

    char valueBuffer[20] = "0";
    char drawnValueBuffer[20] = "";
    Axis_e drawnAxis = AXIS_NONE;
    DisplayArea valueArea = DisplayArea(0, 0, 320, 50);
    // An array holing the key labels
    char keyLabels[4][5][5] = {
      { "F1","7", "8", "9", "F3"  },
      { "F2","4", "5", "6", "F4" },
      { "Clr","1", "2", "3", "<==" },
      { "Can","+/-", "0", ".", "OK" }
    };

    void setKeyLabels();

    void drawValue();
    void drawLabel();

};


#endif //ManualmaticOffsetKeypad_h
