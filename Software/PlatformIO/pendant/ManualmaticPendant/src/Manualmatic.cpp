#include "Manualmatic.h"


Manualmatic::Manualmatic(Stream& stream, Adafruit_GFX& gfx, TouchScreen& ts)
    : serialMessage(stream), 
      config(), 
      state(config), 
      display(gfx, state, config, brkp, okp), 
      brkp(gfx, ts, state),
      okp(gfx, &FreeSansBold12pt7b, ts, state),
      control(serialMessage, config, state, brkp, okp)
    {
  }


void Manualmatic::begin() {
  
  control.begin();
  display.begin();
  state.setScreen(SCREEN_SPLASH);
}

void Manualmatic::update() {
  control.update();
  display.update();
}