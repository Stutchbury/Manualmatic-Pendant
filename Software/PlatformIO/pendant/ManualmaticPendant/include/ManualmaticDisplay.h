/**
 * @file ManualmaticDisplay.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief  All the methods used to actually draw stuff on the display (except 
 * for the TouchKeypads which are handled by the DisplayTouchKeypad class
 * and the icons (in ManualmaticIcons.h).
 *  
 * Most methods accept a forceRefresh argument which means 'do not check
 * drawn state, just draw it'.

 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */

#ifndef ManualmaticDisplay_h
#define ManualmaticDisplay_h

#include "ManualmaticUtils.h"
#include "Adafruit_GFX.h"
#include "ManualmaticFonts.h"
#include "ManualmaticConsts.h"
#include "ManualmaticState.h"
#include "ManualmaticConfig.h"
#include "ManualmaticIcons.h"
#include "ManualmaticButtonRowKeypad.h"
#include "ManualmaticOffsetKeypad.h"

/**
 * @brief  All the methods used to actually draw stuff on the display
 * 
 */
class ManualmaticDisplay {

  public:
   ManualmaticDisplay(Adafruit_GFX& d, 
                     ManualmaticState& s,
                     ManualmaticConfig& config,
                     ManualmaticButtonRowKeypad& brkp, //Needed for draw()
                     ManualmaticOffsetKeypad& okp      //Needed for draw()
                     );
    void begin();
    void update(bool forceRefresh = false);

  private:
    Adafruit_GFX& gfx;
    ManualmaticState& state;
    ManualmaticConfig& config;
    ManualmaticButtonRowKeypad& brkp;
    ManualmaticOffsetKeypad& okp;
    ManualmaticIcons icons;

    unsigned long now = millis();
    unsigned long lastDisplayRefresh = 0;
    bool forceRefresh = false;

    const uint16_t displayWidth = 320;
    const uint16_t displayHeight = 240;
    const uint8_t axesAreaHeight = 136; //All axis
    const uint8_t encoderLabelAreaHeight = 16;
    const uint8_t encoderValueAreaHeight[3] = {52, 26, 26}; //Single line, 1st line, 2nd line
    const uint8_t encoderColumnWidth = 106; //Centre colum is reduced to 105
    const uint8_t buttonColumnWidth = 63;
    //The top Y of each axis row.
    uint8_t axisDisplayY[4] = {0, 36, 72, 106};

    /**
     * @brief The drawn state of various entities. Used to re-draw only 
     * changed entities
     * 
     */
    struct DrawnState_s {
      Axis_e currentAxis = AXIS_NONE;
      Axis_e joystickAxis = AXIS_NONE;
      uint8_t homed[8] = {0, 0, 0, 0, 0, 0, 0, 0};
      float displayedAxisValues[8] = {0, 0, 0, 0, 0, 0, 0, 0};
      uint8_t displayedAxes = 4;
      uint8_t displayedCoordSystem = 2;
      float spindleOverride = 1;
      float spindleRpm = 400; // @TODO set from config default Spindle rpm set on the pendant
      float spindleSpeed = 0; // Set speed of the spindle on the machine
      float rapidrate = 1;
      float rapidSpeed = 0;
      float rapid_vel = 0; //locally calculated for display based on current_vel & motion_type
      float feed_vel = 0; //locally calculated for display based on current_vel & motion_type
      float feedrate = 1;
      float jogVelocity[2] = { 180, 3000 }; //Sent to serial (as mm/min) but does not update gmoccapy
      JogRange_e jogVelocityRange = JOG_RANGE_HIGH;
      uint8_t currentJogIncrement = 3;
      ButtonRow_e buttonRow = BUTTON_ROW_NONE;
      Task_state_e task_state = STATE_INIT;
      Screen_e screen = SCREEN_INIT;
      Task_mode_e task_mode = MODE_UNKNOWN;
      Flood_e flood = FLOOD_OFF;
      Mist_e mist = MIST_OFF;
      ErrorMessage_e errorMessage = ERRMSG_NONE;
    };


    /**
     * @brief Struct for entities drawn on the display
     */
    DrawnState_s drawn;



    void setupButtonRowKeypad();
    void setupOffsetKeypad();
    void buttonLabelDrawHandler(TouchKey& tk);
    void drawTouchIconCancel(TouchKey& a);
    void drawTouchIconOK(TouchKey& a);


    
    /** ***************************************************************
     * Draw the screen for manual task_mode
     */
    void drawScreenManual(bool forceRefresh = false);
    void drawScreenAuto(bool forceRefresh);
    void drawScreenSplash(bool forceRefresh);
    void drawScreenEstopped(bool forceRefresh);
    void drawScreenEstopReset(bool forceRefresh);


    void drawLines(bool forceRefresh = false);
    void drawButtonLine(uint8_t col);
    void drawAxes(bool forceRefresh = false);
    void setNumDrawnAxes(uint8_t num);
    void drawAxis(uint8_t axis, bool forceRefresh = false);
    int axisColour(uint8_t axis);
    /** ***************************************************************
       Draw the axis label
      Background should be refreshed (new colour) before calling this.
    */
    void drawAxisLabel(uint8_t axis, bool forceRefresh = false);
    /** ***************************************************************
       Draw the coordinate system (Abs, G54, DTG etc)
      Background should bew refreshed (new colour) before calling this.
    */
    void drawAxisCoord(uint8_t axis, bool forceRefresh = false);
    /** ***************************************************************
       Set the value to be displayed for the axis based
      on G5x offset in use or if DTG is required.
      Return true if changed.
    */
    bool setDisplayedAxisValue(uint8_t axis);
    void drawAxisMarkers(bool forceRefresh = false);
    void drawManualEncoderRow(bool forceRefresh = false);
    void drawAutoEncoderRow(bool forceRefresh = false);
    void drawSpindle(bool forceRefresh = false );
    void drawEncoderLabel(uint8_t pos, const char *label, int bg = WHITE, int fg = BLACK );
    /** ***************************************************************
       In manual mode, spindle RPM is set directly
    */
    void drawSpindleRpm(bool forceRefresh = false );

    void drawEncoderValue(uint8_t pos, uint8_t lineNum, const char *val, const char *uom, int bg = BLACK, int fg = WHITE );
    void drawEncoderValue(uint8_t pos, uint8_t lineNum, const char *val, int bg = BLACK, int fg = WHITE );

    void drawSpindleOverride(bool forceRefresh = false );
    /** ***************************************************************
       Actual spindle speed
    */
    void drawSpindleSpeed(bool forceRefresh = false );
    void drawJogIncrement(bool forceRefresh = false );
    void drawJogVelocity(bool forceRefresh = false );

    void drawModeLabel(bool forceRefresh = false);
    void drawRapidOverride(bool forceRefresh = false );
    void drawRapidVelocity(bool forceRefresh = false );
    void drawFeedOverride(bool forceRefresh = false );
    void drawFeedVelocity(bool forceRefresh = false );

    void drawButtonRow(bool forceRefresh = false);
    /**
     * This is the centred label for the button (when there are only yes/no options)
     */
    void drawButtonRowPrompt(char const* label );
    void drawButtonRowError(char const* label );
    void drawStopButton();
    
    struct DisplayAreas_s {
      DisplayArea axes;
      DisplayArea axesMarkers;
      DisplayArea axesLabels;
      DisplayArea axesCoords;
      DisplayArea encoderLabel[3];
      DisplayArea encoderValue[3];
      DisplayArea buttonLabels[5];
      DisplayArea axisMarkers;
      DisplayArea debugRow;
    };

    DisplayAreas_s areas;
    /**
     * For the display of each axis Abs, G5x or DTG position
     */
    DisplayNumber axisPosition[4] = { DisplayNumber(gfx), DisplayNumber(gfx), DisplayNumber(gfx), DisplayNumber(gfx) }; 

};

#endif //ManualmaticDisplay_h
