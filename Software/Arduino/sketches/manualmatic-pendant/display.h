/**
 * All the functions used to actually draw stuff on the display (except 
 * for the TouchKeypads which are handled by the DisplayTouchKeypad class
 * and the icons (in icons.h).
 * 
 * This is divided into axis section, encoders section and button row but is
 * too big and will probably be split into smaller sections.
 * 
 * The only function called in the sketch loop() is updateDisplay()
 * 
 * Most functions accept a forceRefresh argument which means 'do not chack
 * drawn state, just draw it'.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */

#include "icons.h"

unsigned long lastDisplayRefresh = 0;

const uint16_t displayWidth = 320;
const uint16_t displayHeight = 240;

const uint8_t axesAreaHeight = 136; //All axis

const uint8_t encoderLabelAreaHeight = 16;
const uint8_t encoderValueAreaHeight[3] = {52, 26, 26}; //Single line, 1st line, 2nd line
const uint8_t encoderColumnWidth = 106; //Centre colum is reduced to 105
const uint8_t buttonColumnWidth = 63;

struct Areas_s {
  DisplayArea axes = DisplayArea(0, 0, displayWidth, axesAreaHeight);
  DisplayArea axesMarkers = DisplayArea(0, 0, 19, axesAreaHeight);
  DisplayArea axesLabels = DisplayArea(20, 0, 50, axesAreaHeight);
  DisplayArea axesCoords = DisplayArea(50, 0, 32, axesAreaHeight);
  DisplayArea encoderLabel[3] = {
    DisplayArea(0, axesAreaHeight, encoderColumnWidth, encoderLabelAreaHeight),
    DisplayArea(encoderColumnWidth + 1, axesAreaHeight, encoderColumnWidth, encoderLabelAreaHeight),
    DisplayArea((encoderColumnWidth * 2) + 2, axesAreaHeight, encoderColumnWidth, encoderLabelAreaHeight)
  };
  DisplayArea encoderValue[3] = {
    DisplayArea(0, axesAreaHeight + encoderLabelAreaHeight, encoderColumnWidth, encoderValueAreaHeight[0] ),
    DisplayArea(encoderColumnWidth + 1, axesAreaHeight + encoderLabelAreaHeight, encoderColumnWidth, encoderValueAreaHeight[0] ),
    DisplayArea((encoderColumnWidth * 2) + 2, axesAreaHeight + encoderLabelAreaHeight, encoderColumnWidth, encoderValueAreaHeight[0] )
  };

  DisplayArea buttonLabels[5] = {
    DisplayArea( 0, encoderValue[0].b() + 1, buttonColumnWidth, displayHeight - encoderValue[0].b() - 1),
    DisplayArea( buttonColumnWidth + 1, encoderValue[0].b() + 1, buttonColumnWidth, displayHeight - encoderValue[0].b() - 1),
    DisplayArea( (buttonColumnWidth * 2) + 2, encoderValue[0].b() + 1, buttonColumnWidth, displayHeight - encoderValue[0].b() - 1),
    DisplayArea( (buttonColumnWidth * 3) + 3, encoderValue[0].b() + 1, buttonColumnWidth, displayHeight - encoderValue[0].b() - 1),
    DisplayArea( (buttonColumnWidth * 4) + 4, encoderValue[0].b() + 1, buttonColumnWidth + 1, displayHeight - encoderValue[0].b() - 1)
  };
  DisplayArea axisMarkers = DisplayArea(0, 0, 16, axesAreaHeight);

  DisplayArea debugRow = DisplayArea(0, buttonLabels[0].y(), 340, buttonLabels[0].h());

  /**
     Keypad

     F1  7   8   9   Can
     F2  4   5   6   Clr
     F3  1   2   3   Bk
     F4  -   0   .   OK

  */
  /**
     Keypad 2 (consistent cancel/OK button position)

     F1  7   8   9   F3
     F2  4   5   6   F4
     Clr 1   2   3   Bksp
     Can -   0   .   OK

  */


  DisplayArea touchKeys[4][5];
  DisplayArea keypadDisplayLabel = { 0, 0, 160, 240 / 5};
  DisplayArea keypadDisplay = { 160, 0, 160, 240 / 5};

};

/** ***************************************
   The defined areas with defaults set above
*/
Areas_s areas;
/** ***************************************
    Mirror the state to determine if drawn yet
*/
State_s drawn;
/**
 * For the displayed axes
 */
DisplayNumber axisPosition[4] = { DisplayNumber(tft), DisplayNumber(tft), DisplayNumber(tft), DisplayNumber(tft) }; 

//The top Y of each axis row.
uint8_t axisDisplayY[4] = {0, 36, 72, 106};



/** ***************************************************************

*/
void drawTouchIconCancel(TouchKey& a) {
  Coords_s cp = { a.xCl(), a.yCl() };
  drawIconX(cp);
}

void drawTouchIconOK(TouchKey& a) {
  Coords_s cp = { a.xCl(), a.yCl() };
  drawIconTick(cp);
}


/** ***************************************************************

*/
void drawDebug() {
  if ( debugStr.length() ) {
    tft.fillRect(areas.debugRow.x(), areas.debugRow.y(), areas.debugRow.w(), areas.debugRow.h(), ILI9341_BLACK);
    tft.setCursor(areas.debugRow.x(), areas.debugRow.y() + 10);
    tft.setFont();
    tft.setTextSize(2);
    tft.print(debugStr);
    debugStr = "";
  }
}




/** ***************************************************************

*/

void drawButtonLine(uint8_t col) {
  if ( col > 0 ) { //draw left line
      tft.drawFastVLine(
        areas.buttonLabels[col].x()-1,
        areas.buttonLabels[col].y(),
        areas.buttonLabels[col].h(),
        ILI9341_WHITE);
  }
  if ( col < 4 ) { //draw right line
      tft.drawFastVLine(
        areas.buttonLabels[col].r()+1,
        areas.buttonLabels[col].y(),
        areas.buttonLabels[col].h(),
        ILI9341_WHITE);
  }
  tft.drawFastHLine(areas.buttonLabels[0].x(), areas.buttonLabels[0].y(), displayWidth, ILI9341_WHITE);
}


void drawButtonLines(bool forceRefresh = false) {
//  if ( forceRefresh ) {
//    for (int i = 1; i < 5; i++ ) { //ignore the first button
//      tft.drawLine(
//        areas.buttonLabels[i].x() - 1,
//        areas.buttonLabels[i].y(),
//        areas.buttonLabels[i].x() - 1,
//        displayHeight,
//        ILI9341_WHITE);
//    }
//  }
}
void drawLines(bool forceRefresh = false) {
  if ( forceRefresh ) {
    for (int i = 1; i < 3; i++ ) { //ignore the first button
      tft.drawLine(
        areas.encoderLabel[i].x() - 1,
        areas.encoderLabel[i].y(),
        areas.encoderLabel[i].x() - 1,
        areas.encoderValue[i].b(),
        ILI9341_WHITE);
    }
    //tft.drawFastHLine(areas.buttonLabels[0].x(), areas.buttonLabels[0].y() - 1, displayWidth, ILI9341_WHITE);
  }
  //drawButtonLines(forceRefresh);
}

/** ***************************************************************

*/
int axisColour(uint8_t axis) {
  if ( !state.homed[axis] ) {
    return ILI9341_RED;
  }
  if ( state.displayedCoordSystem == 0 ) { //Abs
    return ILI9341_YELLOW;
  } else if ( state.displayedCoordSystem == 1 ) { //DTG
    return ILI9341_BLUE;
  } else if ( state.displayedCoordSystem == 2 ) { //G5x
    return ILI9341_GREEN;
  }
  return ILI9341_WHITE;
}

/** ***************************************************************
   Draw the axis label
   Background should be refreshed (new colour) before calling this.
*/
void drawAxisLabel(uint8_t axis, bool forceRefresh = false) {
  if ( forceRefresh ) {
    uint8_t da = state.displayedAxes;
    tft.fillRect(areas.axesLabels.x(), areas.axes.yDiv(da, axis), areas.axesLabels.w(), areas.axes.hDiv(da), ILI9341_BLACK );
    tft.setFont(&FreeMonoBold24pt7b);
    tft.setCursor(areas.axesLabels.x(), areas.axes.yCl(da, axis) + 12);
    tft.print(AXIS_NAME[axis]);
  }
}


/** ***************************************************************
   Draw the coordinate system (Abs, G54, DTG etc)
   Background should bew refreshed (new colour) before calling this.
*/
void drawAxisCoord(uint8_t axis, bool forceRefresh = false) { //[3]) {
  if ( forceRefresh ) {
    uint8_t da = state.displayedAxes;
    tft.fillRect(areas.axesCoords.x(), areas.axes.yDiv(da, axis), areas.axesCoords.w(), areas.axes.hDiv(da), ILI9341_BLACK );
    tft.setFont(&FreeMono9pt7b);
    tft.setCursor(areas.axesCoords.x(), areas.axes.yCl(da, axis) + 12);
    tft.print(coordSystem[state.displayedCoordSystem]);
  }
}

/** ***************************************************************
   Set the value to be displayed for the axis based
   on G5x offset in use or if DTG is required.
   Return true if changed.
*/
bool setDisplayedAxisValue(uint8_t axis) {
  float old = state.displayedAxisValues[axis];
  if ( state.displayedCoordSystem == 0 ) {
    state.displayedAxisValues[axis] = state.axisAbsPos[axis];
  } else if ( state.displayedCoordSystem == 2 ) {
    state.displayedAxisValues[axis] = ( ( (state.axisAbsPos[axis] * -1)  +  state.g5xOffsets[axis] ) * -1); //@TODO Check - this seems too convoluted.
  } else if ( state.displayedCoordSystem == 1 ) {
    state.displayedAxisValues[axis] = state.axisDtg[axis];
  }
  return state.displayedAxisValues[axis] != old;
}



/** ***************************************************************

*/
void drawAxisMarkers(bool forceRefresh = false) {
  if ( forceRefresh || drawn.currentAxis != state.currentAxis ) {
    tft.fillRect(areas.axisMarkers.x(), areas.axisMarkers.y(), areas.axisMarkers.w(), areas.axisMarkers.h(), ILI9341_BLACK);
    if ( state.currentAxis != AXIS_NONE ) {
      tft.fillTriangle(
        0, axisDisplayY[state.currentAxis],
        15, axisDisplayY[state.currentAxis] + 15,
        0, axisDisplayY[state.currentAxis] + 30,
        ILI9341_WHITE
      );
    }
    drawn.currentAxis = state.currentAxis;
  }
}

/** ***************************************************************

*/
void setNumDrawnAxes(uint8_t num) {
  uint8_t axes = min(max(3, num), 4);
  uint8_t topMargin = axes == 4 ? 0 : 5;
  uint8_t incr = (axes == 4 ? 34 : 44);
  for (uint8_t i = 0; i < axes; i++) {
    axisDisplayY[i] = ( (i * incr) + topMargin );
    axisPosition[i].setFont(&FreeMonoBold24pt7b);
    axisPosition[i].setFormat(7, 3); //@TODO 4 for inches
    axisPosition[i].setPosition(tft.width()-axisPosition[i].w(), axisDisplayY[i]);    
//    serialMessage.send("DD", "dw:");
//    serialMessage.send("DD", axisPosition[i].digitWidth);
//    serialMessage.send("DD", "x1:");
//    serialMessage.send("DD", axisPosition[i].x1);
//    serialMessage.send("DD", "w:");
//    serialMessage.send("DD", axisPosition[i].w1);
  }
}

/** ***************************************************************

*/
void drawAxis(uint8_t axis, bool forceRefresh = false) {
  if ( drawn.homed[axis] != state.homed[axis] ) {
    forceRefresh = true;
    drawn.homed[axis] = state.homed[axis];
  }
  tft.setTextColor(axisColour(axis));
  drawAxisLabel(axis, forceRefresh);
  drawAxisCoord(axis, forceRefresh);
  bool updated = setDisplayedAxisValue(axis);
  if ( forceRefresh || updated ) {
    if ( axisPosition[axis].draw(state.displayedAxisValues[axis], axisColour(axis), forceRefresh ) ) {
      drawn.displayedAxisValues[axis] = state.displayedAxisValues[axis];
    }
  }
  

}

/** ***************************************************************

*/
void drawAxes(bool forceRefresh = false) {
  if ( forceRefresh || drawn.displayedAxes != state.displayedAxes ) {
    forceRefresh = true;
    tft.fillRect(areas.axes.x(), areas.axes.y(), areas.axes.w(), areas.axes.h(), ILI9341_BLACK);
    setNumDrawnAxes(state.displayedAxes);
    drawn.displayedAxes = state.displayedAxes;
  }
  if ( drawn.displayedCoordSystem != state.displayedCoordSystem ) {
    forceRefresh = true;
    drawn.displayedCoordSystem = state.displayedCoordSystem;
  }
  for ( int axis = 0; axis < state.displayedAxes; axis++ ) {
    drawAxis(axis, forceRefresh);
  }
}









/** ***************************************************************

*/
void drawEncoderValue(uint8_t pos, uint8_t lineNum, const char *val, const char *uom, int bg = ILI9341_BLACK, int fg = ILI9341_WHITE ) {
  int16_t  x, y;
  uint16_t w, h;
  char buf[10];
  strcpy(buf, val);
  if ( strlen(uom) > 0 ) {
    strcat(buf, uom);
  }
  trimTrailing(buf);
  char b[10];
  dtostrf(strlen(buf), 3, 0, b);
  //If line num is 0 then only one line, if 1 then first (of 2) lines, if 2 then second (of 2) lines
  uint8_t div = lineNum > 0 ? 2 : 1;
  uint8_t row = lineNum > 1 ? 1 : 0;
  tft.fillRect(areas.encoderValue[pos].x(), areas.encoderValue[pos].yDiv(div, row), areas.encoderValue[pos].w(), areas.encoderValue[pos].hDiv(div), bg);
  tft.setFont(&FreeSansBold18pt7b);
  tft.getTextBounds(buf, areas.encoderValue[pos].x(), areas.encoderValue[pos].y(), &x, &y, &w, &h);
  if ( lineNum > 0 || w > areas.encoderValue[pos].w() ) {
    tft.setFont(&FreeSansBold12pt7b);
    tft.getTextBounds(buf, areas.encoderValue[pos].x(), areas.encoderValue[pos].y(), &x, &y, &w, &h);
  }
  tft.setTextColor(fg);
  tft.setCursor(areas.encoderValue[pos].xCl()-(w/2), areas.encoderValue[pos].yCl(div, row) + (h/2));
  tft.print(buf);
  tft.setTextColor(ILI9341_WHITE);
}

/** ***************************************************************

*/
void drawEncoderValue(uint8_t pos, uint8_t lineNum, const char *val, int bg = ILI9341_BLACK, int fg = ILI9341_WHITE ) {
  drawEncoderValue(pos, lineNum, val, "", bg, fg );
}



/** ***************************************************************

*/
void drawEncoderLabel(uint8_t pos, const char *label, int bg = ILI9341_WHITE, int fg = ILI9341_BLACK ) {
  int16_t  x, y;
  uint16_t w, h;
  char buf[10];
  strcpy(buf, label);
  tft.fillRect(areas.encoderLabel[pos].x(), areas.encoderLabel[pos].y(), areas.encoderLabel[pos].w(), areas.encoderLabel[pos].h(), bg);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setTextSize(1);
  tft.setTextColor(fg);
  tft.getTextBounds(buf, areas.encoderLabel[pos].x(), areas.encoderLabel[pos].b(), &x, &y, &w, &h);
  tft.setCursor(areas.encoderLabel[pos].xCl()-(w/2), areas.encoderLabel[pos].b()-(y-areas.encoderLabel[pos].y()));
  tft.print(buf);
  tft.setTextColor(ILI9341_WHITE);
}



/** ***************************************************************

*/
void drawSpindleOverride(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.spindleOverride != state.spindleOverride ) {
    uint8_t a = 0;
    char buffer[10];
    //@TODO Check if need to display speed or override percent (manual/mid or auto)
    dtostrf(state.spindleOverride * 100, 3, 0, buffer);
    strcat(buffer, "%");
    drawEncoderValue(a, 1, buffer);
    drawn.spindleOverride = state.spindleOverride;
  }
}

/** ***************************************************************
   In manual mode, spindle RPM is set directly
*/
void drawSpindleRpm(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.spindleRpm != state.spindleRpm * state.spindleOverride ) {
    uint8_t a = 0;
    char buffer[10];
    dtostrf((state.spindleRpm * state.spindleOverride), -6, 0, buffer);
    drawEncoderValue(a, 0, buffer);
    drawn.spindleRpm = state.spindleRpm * state.spindleOverride;
  }
}

/** ***************************************************************
   Actual spindle speed
*/
void drawSpindleSpeed(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.spindleSpeed != (state.spindleSpeed * state.spindleOverride) ) {
    uint8_t a = 0;
    char buffer[10];
    dtostrf((state.spindleSpeed * state.spindleOverride), -6, 0, buffer);
    drawEncoderValue(a, 2, buffer);
    drawn.spindleSpeed = (state.spindleSpeed * state.spindleOverride);
  }
}

/** ***************************************************************

*/
void drawSpindle(bool forceRefresh = false ) {
  uint8_t a = 0;
  if ( forceRefresh ) {
    drawEncoderLabel(a, "Spindle");
  }
  if ( !state.isAuto() && state.spindleSpeed == 0 ) {
    drawSpindleRpm(forceRefresh);
    drawn.spindleOverride = NAN; //will force refresh when spindle started
    drawn.spindleSpeed = NAN; //will force refresh when spindle started
  } else {
    drawn.spindleRpm = NAN; //will force refresh when spindle stopped
    drawSpindleOverride(forceRefresh);
    drawSpindleSpeed(forceRefresh);
  }
}


/** ***************************************************************

*/
void drawRapidOverride(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.rapidrate != state.rapidrate ) {
    uint8_t a = 1;
    if ( forceRefresh ) {
      drawEncoderLabel(a, "Rapid");
    }
    char buffer[6];
    dtostrf(state.rapidrate * 100, 3, 0, buffer);
    strcat(buffer, "%");
    drawEncoderValue(a, 1, buffer);
    drawn.rapidrate = state.rapidrate;
  }
}

/** ***************************************************************
   Actual rapid speed
   @TODO no longer used?
*/
void drawRapidSpeed(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.rapidSpeed != (state.rapidSpeed * state.rapidrate) ) {
    uint8_t a = 1;
    char buffer[10];
    dtostrf((state.rapidSpeed * state.rapidrate), -6, 0, buffer);
    drawEncoderValue(a, 2, buffer);
    drawn.rapidSpeed = (state.rapidSpeed * state.rapidrate);
  }
}

/** ***************************************************************
   Actual spindle speed
*/
void drawRapidVel(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.rapid_vel != state.rapid_vel ) {
    uint8_t a = 1;
    char buffer[10];
    dtostrf(state.rapid_vel, -6, 0, buffer);
    drawEncoderValue(a, 2, buffer);
    drawn.rapid_vel = state.rapid_vel;
  }
}

/** ***************************************************************

*/
void drawFeedOverride(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.feedrate != state.feedrate ) {
    uint8_t a = 2;
    if ( forceRefresh ) {
      drawEncoderLabel(a, "Feed");
    }
    char buffer[6];
    dtostrf(state.feedrate * 100, 3, 0, buffer);
    strcat(buffer, "%");
    drawEncoderValue(a, 1, buffer);
    drawn.feedrate = state.feedrate;
  }
}

/** ***************************************************************
   Actual feed speed
   @TODO no longer used?
*/
//void drawFeedSpeed(bool forceRefresh = false ) {
//  if ( forceRefresh || drawn.feedSpeed != (state.feedSpeed * state.feedrate) ) {
//    uint8_t a = 2;
//    char buffer[10];
//    dtostrf((state.feedSpeed * state.feedrate), -6, 0, buffer);
//    drawEncoderValue(a, 2, buffer);
//    drawn.feedSpeed = (state.feedSpeed * state.feedrate);
//  }
//}

/** ***************************************************************
   Actual feed speed
*/
void drawFeedVel(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.feed_vel != state.feed_vel ) {
    uint8_t a = 2;
    char buffer[10];
    dtostrf(state.feed_vel, -6, 0, buffer);
    drawEncoderValue(a, 2, buffer);
    drawn.feed_vel = state.feed_vel;
  }
}


/** ***************************************************************

*/
void drawJogVelocity(bool forceRefresh = false ) {
  if ( forceRefresh
       || drawn.jogVelocity[state.jogVelocityRange] != state.jogVelocity[state.jogVelocityRange]
       || drawn.jogVelocityRange != state.jogVelocityRange
     ) {
    uint8_t a = 2;
    if ( forceRefresh ) {
      drawEncoderLabel(a, "Jog mm/m");
    }
    char buffer[7];
    dtostrf(state.jogVelocity[state.jogVelocityRange], -5, 0, buffer);
    drawEncoderValue(a, 0, buffer);
    drawn.jogVelocity[state.jogVelocityRange] = state.jogVelocity[state.jogVelocityRange];
    drawn.jogVelocityRange = state.jogVelocityRange;
  }
}

/** ***************************************************************

*/
void drawJogIncrement(bool forceRefresh = false ) {
  if ( forceRefresh || drawn.currentJogIncrement != state.currentJogIncrement ) {
    uint8_t a = 1;
    if ( forceRefresh ) {
      //tft.fillRect(areas.encoders[a].x(), areas.encoders[a].y(), areas.encoders[a].w(), areas.encoders[a].h(), ILI9341_BLACK);
      drawEncoderLabel(a, "Jog Incr");
    } else {
      //tft.fillRect(areas.encoders[a].x(), areas.encoders[a].y+20, areas.encoders[a].w(), areas.encoders[a].h-20, ILI9341_BLACK);
    }
    char buffer[7];
    uint8_t pre = max(3 - state.currentJogIncrement, 1); //Precision
    dtostrf(state.jogIncrements[state.currentJogIncrement], -4, pre, buffer);
    drawEncoderValue(a, 0, buffer);
    drawn.currentJogIncrement = state.currentJogIncrement;
  }
}



/** ***************************************************************

*/
void drawStopButton() {
  int d = 150; //diameter
  int r = d / 2;
  int lw = d / 17; //line width
  int r2 = r - (lw * 2);
  int r3 = r2 - lw;
  Coords_s cp = { 160, 120 }; //centre point
  tft.fillCircle(cp.x, cp.y, r, ILI9341_RED);
  tft.fillCircle(cp.x, cp.y, r2, ILI9341_WHITE);
  tft.fillCircle(cp.x, cp.y, r3, ILI9341_RED);
  tft.fillRect(cp.x - (lw * 1.5), cp.y - r2, lw * 3, r, ILI9341_RED);
  tft.fillRect(cp.x - (lw * 0.5), cp.y - r2 - lw, lw, lw * 4, ILI9341_WHITE);
}

/** ***************************************************************

*/
void drawEstopReset(bool forceRefresh = false) {
  if ( forceRefresh ) {
    tft.fillScreen(ILI9341_WHITE);
    drawStopButton();
  }
}


/** ***************************************************************

*/
void drawManualEncoderRow(bool forceRefresh = false) {
  drawSpindle(forceRefresh);
  drawJogVelocity(forceRefresh);
  drawJogIncrement(forceRefresh);
}

/** ***************************************************************

*/
void drawAutoEncoderRow(bool forceRefresh = false) {
  drawSpindle(forceRefresh);
  drawRapidOverride(forceRefresh);
  drawRapidVel(forceRefresh);
  drawFeedOverride(forceRefresh);
  drawFeedVel(forceRefresh);
}

/* ***********************************************************************************************************************************
    Buttons
*/



/**
 * This is the centred label for the button (when there are only yes/no options)
 */
void drawButtonRowPrompt(char const* label ) {
  int16_t  x, y;
  uint16_t w, h;
  tft.fillRect(areas.buttonLabels[1].x() - 1, areas.buttonLabels[1].y()+1, (areas.buttonLabels[1].w() * 3) + 5, areas.buttonLabels[3].h(), ILI9341_BLACK);
  tft.setFont(&FreeSansBold12pt7b);
  tft.getTextBounds(label, areas.buttonLabels[2].x(), areas.encoderLabel[0].b(), &x, &y, &w, &h);
  tft.setCursor(areas.buttonLabels[2].xCl() - (w / 2), areas.buttonLabels[2].yCl() + (h / 2));
  tft.setTextColor(ILI9341_WHITE);
  tft.print(label);
}

/** ***************************************************************

*/
void drawButtonRow(bool forceRefresh = false) {
  //Draw the configured buttons
  brkp.draw(0);
  //Draw any additional text
  if ( forceRefresh || drawn.buttonRow != state.buttonRow ) {
    switch ( state.buttonRow ) {
      case BUTTON_ROW_NONE:
        break;
      case BUTTON_ROW_DEFAULT:
        break;
      case BUTTON_ROW_MANUAL:
      case BUTTON_ROW_AUTO:
      case BUTTON_ROW_MDI:
        drawButtonLines(true); //@TODO Sort out button lines....
        break;
      case BUTTON_ROW_SPINDLE_START:
        drawButtonRowPrompt("Start Spindle?");
        break;
      case BUTTON_ROW_SPINDLE_STOP:
        drawButtonRowPrompt("Stop Spindle?");
        break;
      case BUTTON_ROW_PROGRAM_START:
        drawButtonRowPrompt("Run Program?");
        break;
      case BUTTON_ROW_G5X_OFFSET:
        break;
        //default:
        //drawButtonLines();
    }
    drawn.buttonRow = state.buttonRow;
  }
}


/** ***************************************************************
 * Draw a label for auto or mdi
 */
void drawModeLabel(bool forceRefresh = false) {
  //@TODO redraw if number of axes changes
  if ( forceRefresh ) {
    //http://www.barth-dev.de/online/rgb565-color-picker/
    tft.fillRect(areas.axisMarkers.x(), areas.axisMarkers.y(), areas.axisMarkers.w(), areas.axisMarkers.h(), 0x1082 ); //ILI9341_DARKGREY);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setTextColor(0x39C7);
    if ( state.isScreen(SCREEN_AUTO) ) {
      tft.setCursor(0, 35);
      tft.print("A\nU\nT\nO");
    } else if ( state.isScreen(SCREEN_MDI) ) {
      tft.setCursor(0, 40);
      tft.print("M\nD\nI");
    }
  }
}

/** ***************************************************************
 * Draw the screen for auto task_mode
 */
void drawScreenAuto(bool forceRefresh = false) {
  drawLines(forceRefresh);
  drawAxes(forceRefresh);
  drawModeLabel(forceRefresh);
  drawAutoEncoderRow(forceRefresh);
  drawButtonRow(forceRefresh);
//  drawDebug();
}

/** ***************************************************************
 * Draw the screen for manual task_mode
 */
void drawScreenManual(bool forceRefresh = false) {
  drawLines(forceRefresh);
  drawAxes(forceRefresh);
  drawAxisMarkers(forceRefresh);
  drawManualEncoderRow(forceRefresh);
  drawButtonRow(forceRefresh);
//  drawDebug();
}

/** ***************************************************************
 * The startup screen showing name and compiled date & version
 */
void drawSplash(bool forceRefresh = false) {
  if ( forceRefresh ) {
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(35, 120);
    tft.setFont(&FreeSansBold12pt7b);
    tft.print("Manualmatic Pendant");
    tft.setCursor(35, 145);
    tft.setFont(&FreeSansBold9pt7b);
    tft.print("Compiled: ");
    char ver[16];
    version(__DATE__, ver);
    tft.print(ver);
    tft.print(" ");
    tft.print(__TIME__);
    tft.setCursor(100, 170);
    tft.print("Version: v");
    tft.print(MANUALMATIC_VERSION);
  }
}
/** ***************************************************************
 * The main display function called once per loop - decides what
 * to display based on actual state vs drawn state.
 */
void updateDisplay(bool forceRefresh = false) {
  if ( now > lastDisplayRefresh + displayRefreshMs ) {
    lastDisplayRefresh = now;
    if ( drawn.task_state != state.task_state ) {
      forceRefresh = true;
      drawn.task_state = state.task_state;
    }
    if ( drawn.screen != state.screen ) {
      forceRefresh = true;
      drawn.screen = state.screen;
    }
    if ( drawn.task_mode != state.task_mode) {
      forceRefresh = true;
      drawn.task_mode = state.task_mode;
    }

    switch ( state.screen) {
      case SCREEN_MANUAL:
        drawScreenManual(forceRefresh);
        break;
      case SCREEN_AUTO:
        drawScreenAuto(forceRefresh);
        break;
      //      case SCREEN_MDI:
      //      break;
      case SCREEN_OFFSET_KEYPAD:
        //drawKeypad(forceRefresh);
        okp.draw();
        break;
      case SCREEN_SPLASH:
        drawSplash(forceRefresh);
        break;
      case SCREEN_ESTOP:
        drawEstopped(forceRefresh);
        break;
      case SCREEN_ESTOP_RESET:
        drawEstopReset(forceRefresh);
        break;
      default: //SCREEN_INIT
        drawSplash(forceRefresh);
    }
    state.refreshDisplay = false;
  }
}


/** ***************************************************************
 *   Initialise the display
 */
void setupDisplay() {
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
}
