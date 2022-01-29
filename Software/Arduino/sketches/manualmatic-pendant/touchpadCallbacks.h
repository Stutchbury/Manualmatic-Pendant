/**
 * The event & draw handlers + setup functions for the touch based 
 * ButtonRowKeypad and OffsetKeypad.
 * 
 * Note: the button row callbacks are simply forwarded to the pysical 
 * button row handler
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */


void onTouchCancelG5xOffset(TouchKey& tkcb) {
  onCancelG5xOffset();
}

void onTouchSetG5xOffset(TouchKey& tkcb) {
  onSetG5xOffset();  
}


void buttonLabelDrawHandler(TouchKey& tk) {
  Coords_s cp = {tk.xCl(), tk.yCl()};
  uint16_t fgColour = ILI9341_WHITE;
  switch ( static_cast<ButtonType_e>(tk.userId()) ) {
    case BUTTON_NONE:
      break;
    case BUTTON_HALT:
      fgColour = ( state.isButtonRow(BUTTON_ROW_AUTO) && ( state.isProgramState(PROGRAM_STATE_STOPPED) || state.isProgramState(PROGRAM_STATE_NONE) ) ) ? ILI9341_DARKGREY : ILI9341_RED;
      drawIconHalt(cp, fgColour);
      break;
    case BUTTON_STOP:
      drawIconStop(cp);
      break;
    case BUTTON_CANCEL:
      drawIconCancel(cp);
      break;
    case BUTTON_TICK:
      drawIconTick(cp);
      break;
    case BUTTON_TOUCHOFF:
      fgColour = state.currentAxis != AXIS_NONE ? ILI9341_WHITE : ILI9341_DARKGREY;
      drawIconTouchOff(cp, fgColour);
      break;
    case BUTTON_HALF:
      fgColour = state.currentAxis != AXIS_NONE ? ILI9341_WHITE : ILI9341_DARKGREY;
      drawIconHalf(cp, fgColour);
      break;
    case BUTTON_PLAY:
      if ( state.isButtonRow(BUTTON_ROW_AUTO) || state.isButtonRow(BUTTON_ROW_PROGRAM_START) ) {
        if ( state.isProgramState(PROGRAM_STATE_STOPPED) ) {
          fgColour = ILI9341_DARKGREEN;
        } else if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
          fgColour = ILI9341_ORANGE;
        } else {
          fgColour = ILI9341_DARKGREY;
        }
      }
      drawIconPlay(cp, fgColour);
      break;
    case BUTTON_PAUSE:
      if ( state.isButtonRow(BUTTON_ROW_AUTO) || state.isButtonRow(BUTTON_ROW_PROGRAM_START) ) {
        /*if ( isProgramState(PROGRAM_STATE_STOPPED) ) {
          fgColour = ILI9341_DARKGREEN;
        } else*/ if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
          fgColour = ILI9341_ORANGE;
        } else {
          fgColour = ILI9341_WHITE;
        }
      }
      drawIconPause(cp, fgColour);
      break;
    case BUTTON_ONE_STEP:
      if ( state.isProgramState(PROGRAM_STATE_STOPPED) ) {
        fgColour = ILI9341_DARKGREEN;
      } else if ( state.isProgramState(PROGRAM_STATE_PAUSED) ) {
        fgColour = ILI9341_ORANGE;
      } else {
        fgColour = ILI9341_DARKGREY;
      }
      drawIconOneStep(cp, fgColour);
      break;
    case BUTTON_COOLANT:
      drawIconTap(cp);
      break;
  }
  if ( static_cast<ButtonType_e>(tk.userId()) != BUTTON_NONE ) {
    drawButtonLine(tk.col());
  }
  
}


void onButtonRowTouched(TouchKey& tk) {
  onButtonRowClicked(static_cast<ButtonType_e>(tk.userId()));
}

void setupButtonRowKeypad() {
  brkp.enable(false);
  for (uint8_t col=0; col<5; col++) {
    brkp.key(0,col).setTouchedHandler(onButtonRowTouched);
    brkp.setDrawHandler(0, col, buttonLabelDrawHandler);
  }
}


void setupOffsetKeypad() {
  okp.enable(false); 
  okp.key(3,0).setTouchedHandler(onTouchCancelG5xOffset);
  okp.key(3,4).setTouchedHandler(onTouchSetG5xOffset);
  okp.setDrawHandler(3, 0, drawTouchIconCancel);
  okp.setDrawHandler(3, 4, drawTouchIconOK);
}
