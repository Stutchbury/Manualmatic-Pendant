/**
 * Extends DisplayTouchKeypad to create a 4x5 TouchKeypad to enter g5x
 * offsets.
 *
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 *
 */


#include "ManualmaticOffsetKeypad.h"

ManualmaticOffsetKeypad::ManualmaticOffsetKeypad(Adafruit_GFX& d, const GFXfont* f, TouchScreen& touchscreen, ManualmaticState& state)
  : DisplayTouchKeypad(d, f, touchscreen, DisplayArea(0, 50, 320, 240 - 50), 4, 5), state(state) {
  setKeyLabels();
}

void ManualmaticOffsetKeypad::draw(uint16_t displayRefreshMs /*= 100*/) {
  //Call parent class
  DisplayTouchKeypad::draw(displayRefreshMs);
  //Draw the valueBuffer
  if (enabled()) {
    drawValue();
    drawLabel();
  }
}


void ManualmaticOffsetKeypad::updateAndDraw(uint16_t displayRefreshMs /*= 100*/, uint16_t touchUpdateMs /*= 10*/) {
  update(touchUpdateMs);
  draw(displayRefreshMs);
}


void ManualmaticOffsetKeypad::getValueBuffer(char* outBuffer) {
  strcpy(outBuffer, valueBuffer);
}

void ManualmaticOffsetKeypad::enable(bool b /*= true*/) {
  DisplayTouchKeypad::enable(b);
  //We have to clear the whole screen, not just the keypad area.
  gfx.fillScreen(bgColour);
  strcpy(drawnValueBuffer, ""); //Force a redraw
  forceRefresh = true;
  drawnAxis = AXIS_NONE;
}


bool ManualmaticOffsetKeypad::onKeyTouched(TouchKey& key) {
  if (!key.fireKeyTouchedCallback()) {
    uint8_t len = strlen(valueBuffer);
    if (key.row() == 1 && key.col() == 4) {
      //Half Offset
      float half = state.displayedAxisValues[state.currentAxis]/2;
      sprintf(valueBuffer, "%.3f", half);
    }
    else if (strchr("0123456789.", labels[key.row()][key.col()][0]) != NULL) {
      // 0-9 or '.' touched
      char digit = labels[key.row()][key.col()][0];
      if (digit == '.' && strchr(valueBuffer, '.') != NULL) {
        //Do not repeat decimal point
        return true;
      }
      else if (strcmp(valueBuffer, "0") == 0 || strcmp(valueBuffer, "-0") == 0) {
        //buffer is just '0'
        if (digit == '.') {
          valueBuffer[len] = digit;
          valueBuffer[len + 1] = '\0';
        }
        else {
          valueBuffer[len - 1] = digit;
        }
      }
      else if (digit != '-') {
        valueBuffer[len] = digit;
        valueBuffer[len + 1] = '\0';
      }
      //--
    }
    else if (key.row() == 3 && key.col() == 1) {
      //+/-
      //if already -ve
      if (valueBuffer[0] == '-') {
        for (int i = 1; i <= len; i++) {
          valueBuffer[i - 1] = valueBuffer[i];
        }
        //valueBuffer[len - 1] = '\0';
      }
      else { //currently +ve
        for (int i = len - 1; i >= 0; i--) {
          valueBuffer[i + 1] = valueBuffer[i];
        }
        valueBuffer[0] = '-';
      }
    }
    else if (key.row() == 2 && key.col() == 0) {
      //Clear (reset to zero)
      valueBuffer[0] = '0';
      valueBuffer[1] = '\0';
    }
    else if (key.row() == 2 && key.col() == 4) {
      //Backspace
      if (len == 1) {
        valueBuffer[0] = '0';
        valueBuffer[1] = '\0';
      }
      else {
        valueBuffer[len - 1] = '\0';
      }
    }
  }
  return true;
}

void ManualmaticOffsetKeypad::setKeyLabels() {
  for (uint8_t row = 0; row < rows; row++) {
    for (uint8_t col = 0; col < cols; col++) {
      setLabel(row, col, keyLabels[row][col]);
    }
  }
}

void ManualmaticOffsetKeypad::drawValue() {
  if (strcmp(drawnValueBuffer, valueBuffer) != 0) {
    //Serial.println(valueBuffer);
    int16_t  x, y;
    uint16_t w, h;
    gfx.setFont(&FreeMonoBold18pt7b);
    gfx.setTextColor(fgColour);
    gfx.getTextBounds(valueBuffer, valueArea.x(), valueArea.b(), &x, &y, &w, &h);
    gfx.setCursor(valueArea.r() - w - 5, valueArea.yCl() + (h / 2));
    gfx.fillRect(valueArea.x() + 85, valueArea.y(), valueArea.w() - 85, valueArea.h(), bgColour);
    gfx.print(valueBuffer);
    strcpy(drawnValueBuffer, valueBuffer);
  }
}

void ManualmaticOffsetKeypad::drawLabel() {
  if (drawnAxis != state.currentAxis) {
    gfx.setTextColor(fgColour);
    gfx.setCursor(valueArea.x() + 4, valueArea.y() + 32);
    gfx.setFont(&FreeSansBold12pt7b);
    gfx.print(AXIS_NAME[state.currentAxis]);
    gfx.setFont(&FreeSansBold9pt7b);
    gfx.print(" offset:");
    drawnAxis = state.currentAxis;
  }
}

