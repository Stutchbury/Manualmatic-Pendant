#include "ManualmaticDisplay.h"



ManualmaticDisplay::ManualmaticDisplay(
    Adafruit_GFX& d, 
    ManualmaticState& s, 
    ManualmaticConfig& config,
    ManualmaticButtonRowKeypad& brkp,
    ManualmaticOffsetKeypad& okp
    ) : gfx(d), 
      state(s), 
      config(config), 
      brkp(brkp),
      okp(okp),
      icons(gfx)
    { areas.axes = DisplayArea(0, 0, displayWidth, axesAreaHeight);
      areas.axesMarkers = DisplayArea(0, 0, 19, axesAreaHeight);
      areas.axesLabels = DisplayArea(20, 0, 50, axesAreaHeight);
      areas.axesCoords = DisplayArea(50, 0, 32, axesAreaHeight);
      areas.encoderLabel[0] = DisplayArea(0, axesAreaHeight, encoderColumnWidth, encoderLabelAreaHeight);
      areas.encoderLabel[1] = DisplayArea(encoderColumnWidth + 1, axesAreaHeight, encoderColumnWidth, encoderLabelAreaHeight);
      areas.encoderLabel[2] = DisplayArea((encoderColumnWidth * 2) + 2, axesAreaHeight, encoderColumnWidth, encoderLabelAreaHeight);
      areas.encoderValue[0] = DisplayArea(0, axesAreaHeight + encoderLabelAreaHeight, encoderColumnWidth, encoderValueAreaHeight[0] );
      areas.encoderValue[1] = DisplayArea(encoderColumnWidth + 1, axesAreaHeight + encoderLabelAreaHeight, encoderColumnWidth, encoderValueAreaHeight[0] );
      areas.encoderValue[2] = DisplayArea((encoderColumnWidth * 2) + 2, axesAreaHeight + encoderLabelAreaHeight, encoderColumnWidth, encoderValueAreaHeight[0] );

      areas.buttonLabels[0] = DisplayArea( 0, areas.encoderValue[0].b() + 1, buttonColumnWidth, displayHeight - areas.encoderValue[0].b() - 1);
      areas.buttonLabels[1] = DisplayArea( buttonColumnWidth + 1, areas.encoderValue[0].b() + 1, buttonColumnWidth, displayHeight - areas.encoderValue[0].b() - 1);
      areas.buttonLabels[2] = DisplayArea( (buttonColumnWidth * 2) + 2, areas.encoderValue[0].b() + 1, buttonColumnWidth, displayHeight - areas.encoderValue[0].b() - 1);
      areas.buttonLabels[3] = DisplayArea( (buttonColumnWidth * 3) + 3, areas.encoderValue[0].b() + 1, buttonColumnWidth, displayHeight - areas.encoderValue[0].b() - 1);
      areas.buttonLabels[4] = DisplayArea( (buttonColumnWidth * 4) + 4, areas.encoderValue[0].b() + 1, buttonColumnWidth + 1, displayHeight - areas.encoderValue[0].b() - 1);

      areas.axisMarkers = DisplayArea(0, 0, 16, axesAreaHeight);

      areas.debugRow = DisplayArea(0, areas.buttonLabels[0].y(), 340, areas.buttonLabels[0].h());


  }

void ManualmaticDisplay::begin() {
  gfx.setTextWrap(false);
  gfx.setRotation(1);
  gfx.fillScreen(BLACK);
  setNumDrawnAxes(config.axes);
  setupButtonRowKeypad();
  setupOffsetKeypad();
}

void ManualmaticDisplay::update(bool forceRefresh /*= false*/) {
/** ***************************************************************
 * The main display function called once per loop - decides what
 * to display based on actual state vs drawn state.
 */
  now = millis();
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
        okp.draw();
        break;
      case SCREEN_SPLASH:
        drawScreenSplash(forceRefresh);
        break;
      case SCREEN_ESTOP:
        drawScreenEstopped(forceRefresh);
        break;
      case SCREEN_ESTOP_RESET:
        drawScreenEstopReset(forceRefresh);
        break;
      default: //SCREEN_INIT
        drawScreenSplash(forceRefresh);
    }
    state.refreshDisplay = false;
  }
  
}


void ManualmaticDisplay::setupButtonRowKeypad() {
  for (uint8_t col=0; col<5; col++) {
    brkp.setDrawHandler(0, col, [&](TouchKey &key) { buttonLabelDrawHandler(key); } );
  }
}


void ManualmaticDisplay::setupOffsetKeypad() {
  okp.setDrawHandler(3, 0, [&](TouchKey &key) { drawTouchIconCancel(key); } );
  okp.setDrawHandler(3, 4, [&](TouchKey &key) { drawTouchIconOK(key); } );
}

void ManualmaticDisplay::buttonLabelDrawHandler(TouchKey& tk) {
  Coords_s cp = {tk.xCl(), tk.yCl()};
  uint16_t fgColour = WHITE;
  switch ( static_cast<ButtonType_e>(tk.userId()) ) {
    case BUTTON_NONE:
      break;
    case BUTTON_HALT:
      fgColour = ( state.isButtonRow(BUTTON_ROW_AUTO) && ( state.isProgramState(PROGRAM_STATE_STOPPED) || state.isProgramState(PROGRAM_STATE_NONE) ) ) ? DARKGREY : RED;
      icons.drawIconHalt(cp, fgColour);
      break;
    case BUTTON_STOP:
      icons.drawIconStop(cp);
      break;
    case BUTTON_CANCEL:
      icons.drawIconCancel(cp);
      break;
    case BUTTON_TICK:
      icons.drawIconTick(cp);
      break;
    case BUTTON_TOUCHOFF:
      fgColour = state.currentAxis != AXIS_NONE ? WHITE : DARKGREY;
      icons.drawIconTouchOff(cp, fgColour);
      break;
    case BUTTON_HALF:
      fgColour = state.currentAxis != AXIS_NONE ? WHITE : DARKGREY;
      icons.drawIconHalf(cp, fgColour);
      break;
    case BUTTON_PLAY:
      if ( state.isButtonRow(BUTTON_ROW_AUTO) || state.isButtonRow(BUTTON_ROW_PROGRAM_START) ) {
        if ( state.isProgramState(PROGRAM_STATE_STOPPED) ) {
          fgColour = DARKGREEN;
        } else if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
          fgColour = ORANGE;
        } else {
          fgColour = DARKGREY;
        }
      }
      icons.drawIconPlay(cp, fgColour);
      break;
    case BUTTON_PAUSE:
      if ( state.isButtonRow(BUTTON_ROW_AUTO) || state.isButtonRow(BUTTON_ROW_PROGRAM_START) ) {
        /*if ( isProgramState(PROGRAM_STATE_STOPPED) ) {
          fgColour = DARKGREEN;
        } else*/ if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
          fgColour = ORANGE;
        } else {
          fgColour = WHITE;
        }
      }
      icons.drawIconPause(cp, fgColour);
      break;
    case BUTTON_ONE_STEP:
      if ( state.isProgramState(PROGRAM_STATE_STOPPED) ) {
        fgColour = DARKGREEN;
      } else if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        fgColour = ORANGE;
      } else {
        fgColour = DARKGREY;
      }
      icons.drawIconOneStep(cp, fgColour);
      break;
    case BUTTON_COOLANT:
      uint8_t flood = floor(tk.userState()/10);
      uint8_t mist = (tk.userState()%10);
      if ( mist == (uint8_t)MIST_ON && flood == (uint8_t)FLOOD_ON) {
        fgColour = DARKGREEN;
        icons.drawIconMistFlood(cp, fgColour);
      } else if ( mist == (uint8_t)MIST_ON) {
        fgColour = GREEN;
        icons.drawIconMist(cp, fgColour);
      } else if ( flood == (uint8_t)FLOOD_ON) {
        fgColour = GREEN;
        icons.drawIconFlood(cp, fgColour);
      } else {
        icons.drawIconCoolant(cp, fgColour);
      }
      break;
  }
  if ( static_cast<ButtonType_e>(tk.userId()) != BUTTON_NONE ) {
    drawButtonLine(tk.col());
  }
  
}


void ManualmaticDisplay::drawScreenManual(bool forceRefresh /*= false*/) {
  drawLines(forceRefresh);
  drawAxes(forceRefresh);
  drawAxisMarkers(forceRefresh);
  drawManualEncoderRow(forceRefresh);
  drawButtonRow(forceRefresh);
//  drawDebug();
}

void ManualmaticDisplay::drawScreenAuto(bool forceRefresh /*= false*/) {
  drawLines(forceRefresh);
  drawAxes(forceRefresh);
  drawModeLabel(forceRefresh);
  drawAutoEncoderRow(forceRefresh);
  drawButtonRow(forceRefresh);
//  drawDebug();
}

void ManualmaticDisplay::drawScreenSplash(bool forceRefresh) {
  if ( forceRefresh ) {
    gfx.fillScreen(BLACK);
    gfx.setCursor(35, 120);
    gfx.setFont(&FreeSansBold12pt7b);
    gfx.print("Manualmatic Pendant");
    gfx.setCursor(35, 145);
    gfx.setFont(&FreeSansBold9pt7b);
    gfx.print("Compiled: ");
    char ver[16];
    version(__DATE__, ver);
    gfx.print(ver);
    gfx.print(" ");
    gfx.print(__TIME__);
    gfx.setCursor(100, 170);
    gfx.print("Version: v");
    gfx.print(MANUALMATIC_VERSION);
  }
}

void ManualmaticDisplay::drawScreenEstopped(bool forceRefresh) {
  icons.drawEstopped(forceRefresh);
}

void ManualmaticDisplay::drawScreenEstopReset(bool forceRefresh) {
  if ( forceRefresh ) {
    gfx.fillScreen(WHITE);
    drawStopButton();
  }
}




void ManualmaticDisplay::drawLines(bool forceRefresh /*= false*/) {
  if ( forceRefresh ) {
    for (int i = 1; i < 3; i++ ) { //ignore the first button
      gfx.drawLine(
        areas.encoderLabel[i].x() - 1,
        areas.encoderLabel[i].y(),
        areas.encoderLabel[i].x() - 1,
        areas.encoderValue[i].b(),
        WHITE);
    }
    //gfx.drawFastHLine(areas.buttonLabels[0].x(), areas.buttonLabels[0].y() - 1, displayWidth, WHITE);
  }
}

/** ***************************************************************

*/
void ManualmaticDisplay::drawAxes(bool forceRefresh /*= false*/) {
  if ( forceRefresh || drawn.displayedAxes != state.displayedAxes ) {
    forceRefresh = true;
    gfx.fillRect(areas.axes.x(), areas.axes.y(), areas.axes.w(), areas.axes.h(), BLACK);
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


void ManualmaticDisplay::setNumDrawnAxes(uint8_t num) {
  uint8_t axes = min(max(3, num), 4);
  uint8_t topMargin = axes == 4 ? 0 : 5;
  uint8_t incr = (axes == 4 ? 34 : 44);
  for (uint8_t i = 0; i < axes; i++) {
    axisDisplayY[i] = ( (i * incr) + topMargin );
    axisPosition[i].setFont(&FreeMonoBold24pt7b);
    axisPosition[i].setFormat(7, 3); //@TODO 4 for inches
    axisPosition[i].setPosition(gfx.width()-axisPosition[i].w(), axisDisplayY[i]);    
  }
}


void ManualmaticDisplay::drawAxis(uint8_t axis, bool forceRefresh /*= false*/) {
  if ( drawn.homed[axis] != state.homed[axis] ) {
    forceRefresh = true;
    drawn.homed[axis] = state.homed[axis];
  }
  gfx.setTextColor(axisColour(axis));
  drawAxisLabel(axis, forceRefresh);
  drawAxisCoord(axis, forceRefresh);
  bool updated = setDisplayedAxisValue(axis);
  if ( forceRefresh || updated ) {
    if ( axisPosition[axis].draw(state.displayedAxisValues[axis], axisColour(axis), forceRefresh ) ) {
      drawn.displayedAxisValues[axis] = state.displayedAxisValues[axis];
    }
  }
}

int ManualmaticDisplay::axisColour(uint8_t axis) {
  if ( !state.homed[axis] ) {
    return RED;
  }
  if ( state.displayedCoordSystem == 0 ) { //Abs
    return YELLOW;
  } else if ( state.displayedCoordSystem == 1 ) { //DTG
    return BLUE;
  } else if ( state.displayedCoordSystem == 2 ) { //G5x
    return GREEN;
  }
  return WHITE;
}

void ManualmaticDisplay::drawAxisLabel(uint8_t axis, bool forceRefresh /*= false*/) {
  if ( forceRefresh ) {
    uint8_t da = state.displayedAxes;
    gfx.fillRect(areas.axesLabels.x(), areas.axes.yDiv(da, axis), areas.axesLabels.w(), areas.axes.hDiv(da), BLACK );
    gfx.setFont(&FreeMonoBold24pt7b);
    gfx.setCursor(areas.axesLabels.x(), areas.axes.yCl(da, axis) + 12);
    gfx.print(AXIS_NAME[axis]);
  }
}


void ManualmaticDisplay::drawAxisCoord(uint8_t axis, bool forceRefresh /*= false*/) { //[3]) {
  if ( forceRefresh ) {
    uint8_t da = state.displayedAxes;
    gfx.fillRect(areas.axesCoords.x(), areas.axes.yDiv(da, axis), areas.axesCoords.w(), areas.axes.hDiv(da), BLACK );
    gfx.setFont(&FreeMono9pt7b);
    gfx.setCursor(areas.axesCoords.x(), areas.axes.yCl(da, axis) + 12);
    gfx.print(coordSystem[state.displayedCoordSystem]);
  }
}

bool ManualmaticDisplay::setDisplayedAxisValue(uint8_t axis) {
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

void ManualmaticDisplay::drawAxisMarkers(bool forceRefresh /*= false*/) {
  // We only check one of the joystick axes
  if ( forceRefresh || drawn.currentAxis != state.currentAxis  || drawn.joystickAxis != state.joystickAxis[0] ) {
    gfx.fillRect(areas.axisMarkers.x(), areas.axisMarkers.y(), areas.axisMarkers.w(), areas.axisMarkers.h(), BLACK);

    for ( uint8_t i=0; i< state.displayedAxes; i++) {
      Coords_s cp = {0, axisDisplayY[i]};
      if ( state.currentAxis == i ) {
        gfx.fillTriangle(
          0, axisDisplayY[i],
          15, axisDisplayY[i] + 15,
          0, axisDisplayY[i] + 30,
          WHITE
        );
      }
      if ( state.joystickAxis[0] == i ) {
        icons.drawJoystickMarker(cp);
      }
      if ( state.joystickAxis[1] == i ) {
        icons.drawJoystickMarker(cp);
      }


    }
    drawn.currentAxis = state.currentAxis;
    drawn.joystickAxis = state.joystickAxis[0];
  }
}


void ManualmaticDisplay::drawManualEncoderRow(bool forceRefresh /*= false*/) {
  drawSpindle(forceRefresh);
  drawJogIncrement(forceRefresh);
  drawJogVelocity(forceRefresh);
}

void ManualmaticDisplay::drawAutoEncoderRow(bool forceRefresh /*= false*/) {
  drawSpindle(forceRefresh);
  drawRapidOverride(forceRefresh);
  drawRapidVelocity(forceRefresh);
  drawFeedOverride(forceRefresh);
  drawFeedVelocity(forceRefresh);
}


void ManualmaticDisplay::drawSpindle(bool forceRefresh /*= false*/ ) {
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


void ManualmaticDisplay::drawEncoderLabel(uint8_t pos, const char *label, int bg /*= WHITE*/, int fg /*= BLACK*/ ) {
  int16_t  x, y;
  uint16_t w, h;
  char buf[10];
  strcpy(buf, label);
  gfx.fillRect(areas.encoderLabel[pos].x(), areas.encoderLabel[pos].y(), areas.encoderLabel[pos].w(), areas.encoderLabel[pos].h(), bg);
  gfx.setFont(&FreeSansBold9pt7b);
  gfx.setTextSize(1);
  gfx.setTextColor(fg);
  gfx.getTextBounds(buf, areas.encoderLabel[pos].x(), areas.encoderLabel[pos].b(), &x, &y, &w, &h);
  gfx.setCursor(areas.encoderLabel[pos].xCl()-(w/2), areas.encoderLabel[pos].b()-(y-areas.encoderLabel[pos].y()));
  gfx.print(buf);
  gfx.setTextColor(WHITE);
}

void ManualmaticDisplay::drawSpindleRpm(bool forceRefresh /*= false*/ ) {
  if ( forceRefresh || drawn.spindleRpm != state.spindleRpm * state.spindleOverride ) {
    uint8_t a = 0;
    char buffer[10];
    dtostrf((state.spindleRpm * state.spindleOverride), -6, 0, buffer);
    drawEncoderValue(a, 0, buffer);
    drawn.spindleRpm = state.spindleRpm * state.spindleOverride;
  }
}


/** ***************************************************************

*/
void ManualmaticDisplay::drawEncoderValue(uint8_t pos, uint8_t lineNum, const char *val, const char *uom, int bg /*= BLACK*/, int fg /*= WHITE*/ ) {
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
  gfx.fillRect(areas.encoderValue[pos].x(), areas.encoderValue[pos].yDiv(div, row), areas.encoderValue[pos].w(), areas.encoderValue[pos].hDiv(div), bg);
  gfx.setFont(&FreeSansBold18pt7b);
  gfx.getTextBounds(buf, areas.encoderValue[pos].x(), areas.encoderValue[pos].y(), &x, &y, &w, &h);
  if ( lineNum > 0 || w > areas.encoderValue[pos].w() ) {
    gfx.setFont(&FreeSansBold12pt7b);
    gfx.getTextBounds(buf, areas.encoderValue[pos].x(), areas.encoderValue[pos].y(), &x, &y, &w, &h);
  }
  gfx.setTextColor(fg);
  gfx.setCursor(areas.encoderValue[pos].xCl()-(w/2), areas.encoderValue[pos].yCl(div, row) + (h/2));
  gfx.print(buf);
  gfx.setTextColor(WHITE);
}

/** ***************************************************************

*/
void ManualmaticDisplay::drawEncoderValue(uint8_t pos, uint8_t lineNum, const char *val, int bg /*= BLACK*/, int fg /*= WHITE*/ ) {
  drawEncoderValue(pos, lineNum, val, "", bg, fg );
}

void ManualmaticDisplay::drawSpindleOverride(bool forceRefresh /*= false*/ ) {
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
   Actual spindle speed
*/
void ManualmaticDisplay::drawSpindleSpeed(bool forceRefresh /*= false*/ ) {
  if ( forceRefresh || drawn.spindleSpeed != (state.spindleSpeed * state.spindleOverride) ) {
    uint8_t a = 0;
    char buffer[10];
    dtostrf((state.spindleSpeed * state.spindleOverride), -6, 0, buffer);
    drawEncoderValue(a, 2, buffer);
    drawn.spindleSpeed = (state.spindleSpeed * state.spindleOverride);
  }
}

void ManualmaticDisplay::drawJogIncrement(bool forceRefresh /*= false*/ ) {
  if ( forceRefresh || drawn.currentJogIncrement != state.currentJogIncrement ) {
    uint8_t a = 1;
    if ( forceRefresh ) {
      //gfx.fillRect(areas.encoders[a].x(), areas.encoders[a].y(), areas.encoders[a].w(), areas.encoders[a].h(), BLACK);
      drawEncoderLabel(a, "Jog Incr");
    } else {
      //gfx.fillRect(areas.encoders[a].x(), areas.encoders[a].y+20, areas.encoders[a].w(), areas.encoders[a].h-20, BLACK);
    }
    char buffer[7];
    uint8_t pre = max(3 - state.currentJogIncrement, 1); //Precision
    dtostrf(config.jogIncrements[state.currentJogIncrement], -4, pre, buffer);
    drawEncoderValue(a, 0, buffer);
    drawn.currentJogIncrement = state.currentJogIncrement;
  }
}

void ManualmaticDisplay::drawJogVelocity(bool forceRefresh /*= false*/ ) {
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
    drawEncoderValue(a, 0, buffer, BLACK, (state.jogVelocityRange == JOG_RANGE_HIGH ? LIGHTGREEN : WHITE));
    drawn.jogVelocity[state.jogVelocityRange] = state.jogVelocity[state.jogVelocityRange];
    drawn.jogVelocityRange = state.jogVelocityRange;
  }
}


/** ***************************************************************
 * Draw a label for auto or mdi
 */
void ManualmaticDisplay::drawModeLabel(bool forceRefresh /*= false*/) {
  //@TODO redraw if number of axes changes
  if ( forceRefresh ) {
    //http://www.barth-dev.de/online/rgb565-color-picker/
    //https://chrishewett.com/blog/true-rgb565-colour-picker/
    gfx.fillRect(areas.axisMarkers.x(), areas.axisMarkers.y(), areas.axisMarkers.w(), areas.axisMarkers.h(), 0x1082 ); //DARKGREY);
    gfx.setFont(&FreeSansBold9pt7b);
    //gfx.setTextColor(0x39C7);
    gfx.setTextColor(LIGHTGREEN);
    if ( state.isScreen(SCREEN_AUTO) ) {
      gfx.setCursor(0, 35);
      gfx.print("A\nU\nT\nO");
    } else if ( state.isScreen(SCREEN_MDI) ) {
      gfx.setCursor(0, 40);
      gfx.print("M\nD\nI");
    }
  }
}

void ManualmaticDisplay::drawRapidOverride(bool forceRefresh /*= false*/ ) {
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

void ManualmaticDisplay::drawRapidVelocity(bool forceRefresh /*= false*/ ) {
  if ( forceRefresh || drawn.rapid_vel != state.rapid_vel ) {
    uint8_t a = 1;
    char buffer[10];
    dtostrf(state.rapid_vel, -6, 0, buffer);
    drawEncoderValue(a, 2, buffer);
    drawn.rapid_vel = state.rapid_vel;
  }
}


void ManualmaticDisplay::drawFeedOverride(bool forceRefresh /*= false*/ ) {
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

void ManualmaticDisplay::drawFeedVelocity(bool forceRefresh /*= false*/ ) {
  if ( forceRefresh || drawn.feed_vel != state.feed_vel ) {
    uint8_t a = 2;
    char buffer[10];
    dtostrf(state.feed_vel, -6, 0, buffer);
    drawEncoderValue(a, 2, buffer);
    drawn.feed_vel = state.feed_vel;
  }
}


void ManualmaticDisplay::drawButtonRow(bool forceRefresh /*= false*/) {
  if ( forceRefresh || drawn.errorMessage != state.errorMessage ) {
    const char *errmsg = nullptr;
    switch( state.errorMessage ) {
      case ERRMSG_NONE:
        break;
      case ERRMSG_NOT_HOMED:
        errmsg = "Not homed";
        break;
    }
    if ( errmsg ) {
      brkp.clear();
      drawButtonRowError(errmsg);
    }
    drawn.errorMessage = state.errorMessage;
  }
  if ( state.errorMessage != ERRMSG_NONE ) {
    return;
  }
  if ( forceRefresh || drawn.buttonRow != state.buttonRow ) {
    brkp.clear();
    //Draw the configured buttons
    brkp.draw(0);
    //If required, draw any additional text
    switch ( state.buttonRow ) {
      case BUTTON_ROW_NONE:
        break;
      case BUTTON_ROW_DEFAULT:
        break;
      case BUTTON_ROW_MANUAL:
      case BUTTON_ROW_AUTO:
      case BUTTON_ROW_MDI:
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
  } else {
    //Just draw the configured buttons
    brkp.draw(0);
  }
}

void ManualmaticDisplay::drawButtonRowPrompt(char const* label ) {
  int16_t  x, y;
  uint16_t w, h;
  gfx.fillRect(areas.buttonLabels[1].x() - 1, areas.buttonLabels[1].y()+1, (areas.buttonLabels[1].w() * 3) + 5, areas.buttonLabels[3].h(), BLACK);
  gfx.setFont(&FreeSansBold12pt7b);
  gfx.getTextBounds(label, areas.buttonLabels[2].x(), areas.encoderLabel[0].b(), &x, &y, &w, &h);
  gfx.setCursor(areas.buttonLabels[2].xCl() - (w / 2), areas.buttonLabels[2].yCl() + (h / 2));
  gfx.setTextColor(WHITE);
  gfx.print(label);
}

void ManualmaticDisplay::drawButtonRowError(char const* label ) {
  int16_t  x, y;
  uint16_t w, h;
  gfx.fillRect(areas.buttonLabels[1].x() - 1, areas.buttonLabels[1].y()+1, (areas.buttonLabels[1].w() * 3) + 5, areas.buttonLabels[3].h(), RED);
  gfx.setFont(&FreeSansBold12pt7b);
  gfx.getTextBounds(label, areas.buttonLabels[2].x(), areas.encoderLabel[0].b(), &x, &y, &w, &h);
  gfx.setCursor(areas.buttonLabels[2].xCl() - (w / 2), areas.buttonLabels[2].yCl() + (h / 2));
  gfx.setTextColor(WHITE);
  gfx.print(label);
}

void ManualmaticDisplay::drawStopButton() {
  int d = 150; //diameter
  int r = d / 2;
  int lw = d / 17; //line width
  int r2 = r - (lw * 2);
  int r3 = r2 - lw;
  Coords_s cp = { 160, 120 }; //centre point
  gfx.fillCircle(cp.x, cp.y, r, RED);
  gfx.fillCircle(cp.x, cp.y, r2, WHITE);
  gfx.fillCircle(cp.x, cp.y, r3, RED);
  gfx.fillRect(cp.x - (lw * 1.5), cp.y - r2, lw * 3, r, RED);
  gfx.fillRect(cp.x - (lw * 0.5), cp.y - r2 - lw, lw, lw * 4, WHITE);
}


void ManualmaticDisplay::drawTouchIconCancel(TouchKey& a) {
  Coords_s cp = { a.xCl(), a.yCl() };
  icons.drawIconX(cp);
}

void ManualmaticDisplay::drawTouchIconOK(TouchKey& a) {
  Coords_s cp = { a.xCl(), a.yCl() };
  icons.drawIconTick(cp);
}


void ManualmaticDisplay::drawButtonLine(uint8_t col) {
  if ( col > 0 ) { //draw left line
      gfx.drawFastVLine(
        areas.buttonLabels[col].x()-1,
        areas.buttonLabels[col].y(),
        areas.buttonLabels[col].h(),
        WHITE);
  }
  if ( col < 4 ) { //draw right line
      gfx.drawFastVLine(
        areas.buttonLabels[col].r()+1,
        areas.buttonLabels[col].y(),
        areas.buttonLabels[col].h(),
        WHITE);
  }
  gfx.drawFastHLine(areas.buttonLabels[0].x(), areas.buttonLabels[0].y(), displayWidth, WHITE);
}
