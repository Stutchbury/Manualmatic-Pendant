/**
 * @file ManualmaticWiring.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief Defines all the pin connections for the Teensy 4.1 to
 * display, touchscreen, encoders, buttons and joystick.
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */

#ifndef ManualmaticWiring_h
#define ManualmaticWiring_h

#include <Arduino.h>

// For the Adafruit shield, these are the default.
const uint8_t TFT_DC = 9;
const uint8_t TFT_CS = 10;
//For the Adafruit TouchScreen.h
const uint8_t TOUCHSCREEN_XP = A6;  // Y+ must be an analog pin, use "An" notation!
const uint8_t TOUCHSCREEN_YP = A9;  // X- must be an analog pin, use "An" notation!
const uint8_t TOUCHSCREEN_XM = A8;  // Y- can be a digital pin
const uint8_t TOUCHSCREEN_YM = A7;  // X+ can be a digital pin
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, it's 300 ohms across the X plate
const uint16_t TOUCHSCREEN_OHMS = 300;

                                      //Schematic Label
const uint8_t ENCODER_A_FEED = 1;     //FEED_A
const uint8_t ENCODER_B_FEED = 2;     //FEED_B
const uint8_t BUTTON_FEED = 3;        //FEED_SW
const uint8_t ENCODER_A_MPG = 5;      //MPG_A
const uint8_t ENCODER_B_MPG = 4;      //MPG_B
const uint8_t ENCODER_A_SPINDLE = 7;  //SPINDLE_A
const uint8_t ENCODER_B_SPINDLE = 6;  //SPINDLE_B
const uint8_t BUTTON_SPINDLE = 8;     //SPINDLE_SW

const uint8_t BUTTON_ON_OFF = 19;     //ON_OFF_SW
const uint8_t BUTTON_X = 18;          //X_AXIS
const uint8_t BUTTON_Y = 17;          //Y_AXIS
const uint8_t BUTTON_Z = 16;          //Z_AXIS
const uint8_t BUTTON_A = 15;          //A_AXIS
const uint8_t BUTTON_MODE = 0;        //MODE_SW
const uint8_t BUTTON_MODIFIER = 14;   //MODIFIER_SW
const uint8_t BUTTON_ROW_0 = 36;      //ROW_SW_0
const uint8_t BUTTON_ROW_1 = 37;      //ROW_SW_1
const uint8_t BUTTON_ROW_2 = 26;      //ROW_SW_2
const uint8_t BUTTON_ROW_3 = 25;      //ROW_SW_3
const uint8_t BUTTON_ROW_4 = 24;      //ROW_SW_4

const uint8_t JOYSTICK_X = 39;        //JOY_X
const uint8_t JOYSTICK_Y = 38;        //JOY_Y
const uint8_t BUTTON_JOYSTICK = 29;   //JOY_SW




#endif //ManualmaticWiring_h
