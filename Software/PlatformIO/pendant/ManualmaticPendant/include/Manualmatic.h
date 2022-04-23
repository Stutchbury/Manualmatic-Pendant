/**
 * @brief Primary Manualmatic Pendant class
 * 
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 *
 */

#ifndef Manualmatic_h
#define Manualmatic_h


#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "TouchScreen.h"

#include "EventButton.h"

#include "ManualmaticWiring.h"
#include "ManualmaticFonts.h"
#include "ManualmaticUtils.h"
#include "ManualmaticMessage.h"
#include "ManualmaticConfig.h"
#include "ManualmaticState.h"
#include "ManualmaticDisplay.h"
#include "ManualmaticControl.h"
#include "ManualmaticButtonRowKeypad.h"
#include "ManualmaticOffsetKeypad.h"

/**
 * @brief The primary Manualmatic Pendant class.
 * 
 */
class Manualmatic {

public:

  /**
   * @brief Construct a new Manualmatic object
   * 
   * @param stream 
   * @param gfx 
   * @param ts 
   */
  Manualmatic(Stream& stream, Adafruit_GFX& gfx, TouchScreen& ts);

  /**
   * @brief Setup the class and call the necessary begin() methods 
   * of members.
   * 
   */
  void begin();

  /**
   * @brief Called once per loop(), updated each of the 
   * private members.
   * 
   */
  void update();


private:
  ManualmaticMessage serialMessage;
  ManualmaticConfig config;
  ManualmaticState state;
  ManualmaticDisplay display;
  ManualmaticButtonRowKeypad brkp;
  ManualmaticOffsetKeypad okp;
  ManualmaticControl control;

};


#endif //Manualmatic_h
