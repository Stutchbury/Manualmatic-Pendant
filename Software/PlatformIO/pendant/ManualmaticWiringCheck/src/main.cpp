/** 
 * 
 * Manualmatic Pendant Wiring Check
 * 
 * Test the display, touchscreen, buttons, encoders and joystick 
 * on the pendant are all present and correctly wired.
 * 
 * Copy the contents of ManualmaticWiring.h from the main project.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */


#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
#include <EventButton.h>
#include <EncoderButton.h>
#include <EventJoystick.h>
#include "ManualmaticWiring.h"

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 gfx = Adafruit_ILI9341(TFT_CS, TFT_DC);

TouchScreen ts = TouchScreen(TOUCHSCREEN_XP, TOUCHSCREEN_YP, TOUCHSCREEN_XM, TOUCHSCREEN_YM, TOUCHSCREEN_OHMS);
unsigned long lastTsUpdate = millis();

//Encoders
EncoderButton feed(ENCODER_A_FEED, ENCODER_B_FEED, BUTTON_FEED);
EncoderButton mpg(ENCODER_A_MPG, ENCODER_B_MPG);
EncoderButton spindle(ENCODER_A_SPINDLE, ENCODER_B_SPINDLE, BUTTON_SPINDLE);
//Buttons
EventButton buttonOnOff(BUTTON_ON_OFF);
EventButton buttonX(BUTTON_X);
EventButton buttonY(BUTTON_Y);
EventButton buttonZ(BUTTON_Z);
EventButton buttonA(BUTTON_A);
EventButton buttonMode(BUTTON_MODE);
EventButton buttonModifier(BUTTON_MODIFIER);
EventButton buttonRow[5] = { 
  EventButton(BUTTON_ROW_0), 
  EventButton(BUTTON_ROW_1), 
  EventButton(BUTTON_ROW_2), 
  EventButton(BUTTON_ROW_3),
  EventButton(BUTTON_ROW_4)
};
//Joystick
EventJoystick joystick(JOYSTICK_X, JOYSTICK_Y);
EventButton buttonJoystick(BUTTON_JOYSTICK);
void clr() {gfx.fillRect(0,200, 320, 40, 0);}
void onFeedEncoder(EncoderButton& btn) { clr(); gfx.printf("Feed encoder: %i \n", btn.position()); }
void onSpindleEncoder(EncoderButton& btn) { clr(); gfx.printf("Spindle encoder: %i \n", btn.position()); }
void onMpgEncoder(EncoderButton& btn) { clr(); gfx.printf("MPG encoder: %i \n", btn.position()); }
void onFeed(EncoderButton& btn) { clr(); gfx.println("Feed button"); }
void onSpindle(EncoderButton& btn) { clr(); gfx.println("Spindle button"); }
void onOnOff(EventButton& btn) { clr(); gfx.println("On/Off button"); }
void onX(EventButton& btn) { clr(); gfx.println("X axis button"); }
void onY(EventButton& btn) { clr(); gfx.println("Y axis button"); }
void onZ(EventButton& btn) { clr(); gfx.println("Z axis button"); }
void onA(EventButton& btn) { clr(); gfx.println("A axis button"); }
void onMode(EventButton& btn) { clr(); gfx.println("Mode button"); }
void onRow(EventButton& btn) { clr(); gfx.printf("Row button %i \n", btn.userId()); }
void onModifier(EventButton& btn) { clr(); gfx.println("Modifier button"); }
void onJoystick(EventButton& btn) { clr(); gfx.println("Joystick button"); }
void onJoystickMoved(EventJoystick& ej) {
  clr(); gfx.printf("Joystick X: %i, Y: %i \n", ej.x.position(), ej.y.position());
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200);
  gfx.begin();
  gfx.setRotation(1);
  gfx.fillScreen(0);
  mpg.setEncoderHandler(onMpgEncoder);
  feed.setEncoderHandler(onFeedEncoder);
  spindle.setEncoderHandler(onSpindleEncoder);
  feed.setClickHandler(onFeed);
  spindle.setClickHandler(onSpindle);
  buttonOnOff.setClickHandler(onOnOff);
  buttonX.setClickHandler(onX);
  buttonY.setClickHandler(onY);
  buttonZ.setClickHandler(onZ);
  buttonA.setClickHandler(onA);
  buttonMode.setClickHandler(onMode);
  buttonModifier.setClickHandler(onModifier);
  for (uint8_t col=0; col<5; col++) {
    buttonRow[col].setClickHandler(onRow);
    buttonRow[col].setUserId(col);    
  }
  joystick.setChangedHandler(onJoystickMoved);
  buttonJoystick.setClickHandler(onJoystick);
  gfx.setTextSize(2);
  gfx.setCursor(5,50);
  gfx.println("Manualmatic Wiring Check:");
  gfx.setCursor(5,70);
  gfx.println("1. Press each button.");
  gfx.setCursor(5,90);
  gfx.println("2. Turn encoders");
  gfx.setCursor(5,110);
  gfx.println("3. Press encoders");
  gfx.setCursor(5,130);
  gfx.println("4. Touch screen");
}

void loop() {
  // put your main code here, to run repeatedly:
  gfx.setCursor(5,200);
  mpg.update();
  feed.update();
  spindle.update();
  buttonOnOff.update();
  buttonX.update();
  buttonY.update();
  buttonZ.update();
  buttonA.update();
  buttonMode.update();
  buttonModifier.update();
  for (uint8_t col=0; col<5; col++) {
    buttonRow[col].update();    
  }
  buttonJoystick.update();
  joystick.update();


  if ( millis() > lastTsUpdate + 100 ) {
    lastTsUpdate = millis();

    // a point object holds x y and z coordinates
    TSPoint p = ts.getPoint();
    
    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    if (p.z > ts.pressureThreshhold) {
      clr();
      gfx.printf("Touch X: %i, Y: %i \n", p.x, p.y);
    }
  }
}