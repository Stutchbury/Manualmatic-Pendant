/**
 * The main Arduino sketch for the Manualmatic Pendant. See other
 * files for details.
 * 
 * We're using an Arduino IDE 'cheat' to divided the sketch into 
 * 'logical' sections for readability & maintainablitity.
 * 
 * Class definitions are included early, function defintions 'late'.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */
#ifndef MANUALMATIC_VERSION
#define MANUALMATIC_VERSION "0.0.1"
#endif

#include <Arduino.h>
#include <string>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Fonts/FreeMono9pt7b.h>
//#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>

#include <EncoderButton.h>

#include "TouchScreen.h"

#include <DisplayUtils.h>
#include <TouchKeypad.h>
#include "consts.h"
#include "SerialMessage.h"
#include "utils.h"
#include "State.h" //Defines & instatiates a 'state' structure
#include "OffsetKeypad.h"
#include "ButtonRowKeypad.h"


//bool old = false;

//DEBUG
long prevMs = millis();
long currMs = millis();
String debugStr = "";


// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
//For the Adafruit TouchScreen.h
#define YP A6  // X- must be an analog pin, use "An" notation!
#define XM A9  // Y+ must be an analog pin, use "An" notation!
#define YM A8   // X+ can be a digital pin
#define XP A7   // Y- can be a digital pin

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Our Serial message class
SerialMessage serialMessage;

State_s state;

unsigned long now = millis();

//Create all the encoders
EncoderButton feed(1, 2, 3);
EncoderButton mpg(4, 5);
EncoderButton spindle(6, 7, 8);

//and all the buttons
EncoderButton buttonOnOff(19);
EncoderButton buttonX(18);
EncoderButton buttonY(17);
EncoderButton buttonZ(16);
EncoderButton buttonA(15);
EncoderButton buttonMode(0);

//Button row buttons go in an array
EncoderButton buttonRow[5] = { EncoderButton(41), EncoderButton(40), EncoderButton(26), EncoderButton(25), EncoderButton(24) };

//Our touch keypad for entering g5x offsets
OffsetKeypad okp(tft, &FreeSansBold12pt7b, ts, state);

//The area of screen above the button row is a keypad
ButtonRowKeypad brkp(tft, ts, state);

//Forward declare @TODO work out interdependencies...
bool setScreen(Screen_e s );
bool setButtonRow(ButtonRow_e b);

//Divvy up functionality...
#include "serial.h"
#include "display.h"
#include "buttonCallbacks.h"
#include "touchpadCallbacks.h"
#include "manageState.h"
#include "encoderCallbacks.h"


void setup() {
  
  //The usual stuff
  Serial.begin(115200);
  delay(200);
  tft.begin();
  tft.setTextWrap(false);
  delay(200);
  setupDisplay();
  
  setNumDrawnAxes(state.axes);

  //Configure the encoders
  feed.setRateLimit(feedRateLimit);
  feed.setEncoderHandler(onFeedEncoder);
  feed.setEncoderPressedHandler(onFeedPressedEncoder);
  feed.setClickHandler(onFeedClicked);
  feed.setLongPressHandler(onFeedLongPress);

  spindle.setRateLimit(spindleRateLimit);
  spindle.setEncoderHandler(onSpindleEncoder);
  spindle.setClickHandler(onSpindleClicked);
  spindle.setDoubleClickHandler(onSpindleDoubleClicked);
  spindle.setLongPressHandler(onSpindleLongPressed);

  mpg.setRateLimit(mpgRateLimit);
  mpg.setEncoderHandler(onMpgEncoder);


  //And the buttons
  buttonOnOff.setClickHandler(onOnOffClicked);
  buttonOnOff.setLongPressHandler(onOnOffLongPress);
  buttonOnOff.setLongClickDuration(longClickDuration);

  buttonX.setClickHandler(toggleXSelected);  
  buttonX.setLongClickDuration(longClickDuration);

  
  buttonY.setClickHandler(toggleYSelected);  
  buttonY.setLongClickDuration(longClickDuration);
  
  
  buttonZ.setClickHandler(toggleZSelected);
  buttonZ.setLongClickDuration(longClickDuration);
  
  
  buttonA.setClickHandler(toggleASelected);
  //buttonA.setPressedHandler(onButtonAPressed);  
  buttonA.setDoubleClickHandler(toggleDisplayAbsG5x);
  buttonA.setTripleClickHandler(displayDtg);
  buttonA.setLongPressHandler(onButtonALongPressed);
  buttonA.setLongClickDuration(longClickDuration);
  
  buttonMode.setClickHandler(onButtonModeClicked);
  buttonMode.setLongClickDuration(longClickDuration);

  for ( uint8_t b=0; b<5; b++ ) {
    buttonRow[b].setUserId(BUTTON_NONE);
    buttonRow[b].setLongClickDuration(longClickDuration);
    //Set all buttons in button row to same handler
    buttonRow[b].setClickHandler(onButtonRowButtonClicked);
    buttonRow[b].setDoubleClickHandler(onButtonRowDoubleClicked);
  }
   
  setupButtonRowKeypad();
  setupOffsetKeypad();

  setScreen(SCREEN_SPLASH);

}


void loop() {
  now = millis();
  //Update everything...
  updateState();
  mpg.update();
  feed.update();
  spindle.update();
  buttonOnOff.update();
  buttonX.update();
  buttonY.update();
  buttonZ.update();
  buttonA.update();
  buttonMode.update();
  updateButtonRow();
  okp.update();  //Do draw() in updateDisplay()
  brkp.update(); //Ditto

  updateDisplay(state.refreshDisplay);
     
}
