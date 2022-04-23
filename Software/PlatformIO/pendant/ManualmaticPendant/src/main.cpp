/**
 * @file main.cpp
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief Main file for Manualmatic Pendant
 * @version 0.0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */
#include <Arduino.h>

//We are dependent on GFX but not to a particular screen
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "TouchScreen.h"

#include "Manualmatic.h"


// Use hardware SPI
Adafruit_ILI9341 gfx = Adafruit_ILI9341(TFT_CS, TFT_DC);

TouchScreen ts = TouchScreen(TOUCHSCREEN_XP, TOUCHSCREEN_YP, TOUCHSCREEN_XM, TOUCHSCREEN_YM, TOUCHSCREEN_OHMS);

Manualmatic manualmatic(Serial, gfx, ts);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200);

   gfx.begin();
   manualmatic.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  manualmatic.update();
}