/**
 * Test the encoders & buttons for the Manualmatic pendant are wired correctly.
 * 
 * Load the sketch and open the serial monitor to check.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */


#include <Arduino.h>

/**
 * Used for encoder, encoder+button and buttons
 */
#include <EncoderButton.h>

/**
 * Main encoder wheel. This will eventually have MPG_SW attached.
 */
EncoderButton mpg(4, 5);

EncoderButton feed(1, 2, 3);
EncoderButton spindle(6, 7, 8);

EncoderButton buttonOnOff(19);
EncoderButton buttonX(18);
EncoderButton buttonY(17);
EncoderButton buttonZ(16);
EncoderButton buttonA(15);
EncoderButton buttonMode(0);

//The configurable button row
EncoderButton buttonRow[5] = { EncoderButton(41), EncoderButton(40), EncoderButton(26), EncoderButton(25), EncoderButton(24) };


/**
 * In this sketch a Teensy 4.1 will never give an increment greater than 1 
 * (or less than -1, obvs) - if you want to see that, either put a delay() 
 * in the loop or mpg.setRateLimit(<pick a number of millis>).
 */
void onMpgEncoder(EncoderButton& btn) { 
  int8_t incr = btn.increment();
  Serial.print("MPG: ");
  for ( uint8_t i=0; i<abs(incr); i++ ) {
    Serial.print(incr > 0 ? ">" : "<");
  }
  Serial.println(incr); 
}
void onFeedEncoder(EncoderButton& btn) { Serial.print("I am feed encoder "); Serial.println(btn.increment() > 0 ? ">" : "<"); }
void onSpindleEncoder(EncoderButton& btn) { Serial.print("I am spindle encoder "); Serial.println(btn.increment() > 0 ? ">" : "<"); }
void onFeed(EncoderButton& btn) { Serial.println("I am button Feed clicked"); }
void onSpindle(EncoderButton& btn) { Serial.println("I am button Spindle"); }
void onOnOff(EncoderButton& btn) { Serial.println("I am button On/Off"); }
void onX(EncoderButton& btn) { Serial.println("I am button X"); }
void onY(EncoderButton& btn) { Serial.println("I am button Y"); }
void onZ(EncoderButton& btn) { Serial.println("I am button Z"); }
void onA(EncoderButton& btn) { Serial.println("I am button A"); }
void onMode(EncoderButton& btn) { Serial.println("I am button Mode"); }
void onRow(EncoderButton& btn) { Serial.print("I am row button: "); Serial.println(btn.userId()); }



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200);
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
  for (uint8_t col=0; col<5; col++) {
    buttonRow[col].setClickHandler(onRow);
    buttonRow[col].setUserId(col);    
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  mpg.update();
  feed.update();
  spindle.update();
  buttonOnOff.update();
  buttonX.update();
  buttonY.update();
  buttonZ.update();
  buttonA.update();
  buttonMode.update();
  for (uint8_t col=0; col<5; col++) {
    buttonRow[col].update();    
  }
}
