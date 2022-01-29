/**
 * A struct that maintains the state of the pendant.
 * 
 * Two copies are maintained: the first is instantiated in the primary
 * sketch file as 'state', the second as 'drawn' in display.h
 * This allows us to only draw values on the display that have changed
 * - writing to the screen over SPI is relatively slow (compared to a
 * Teensy) so we only want to do it when absolutely necessary.
 * 
 * This will probably end up being a class...
 * 
 * There are a lot of conflicting naming conventions here - I've
 * tried to follow the Python 'underscore' naming convention where I'm mirroring
 * properties in the LinuxCNC Interface class but my natural inclination
 * is towards camelCase (for local state & methods), so there's a bit of both...
 * Sorry about that.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */

struct State_s {
  Task_state_e task_state = STATE_INIT;
  Task_mode_e task_mode = MODE_UNKNOWN;
  Interp_e interpState = INTERP_IDLE;
  Exec_state_e exec_state = RCS_DONE;

  Program_state_e program_state = PROGRAM_STATE_NONE;

  Operation_e currentOperation = OPERATION_NONE;
  //ini values
  float max_feed_override = 1;
  float min_spindle_override = 0.5;
  float max_spindle_override = 1;
  float default_spindle_speed = 450;
  // In machine units per second - converted in setIniValue() to machine units per minute
  float default_linear_velocity = 1;
  float max_linear_velocity = 5;


  Axis_e currentAxis = AXIS_NONE;
  uint8_t axes = 4;
  float g5xOffsets[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  float axisAbsPos[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  float axisDtg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t homed[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t all_homed = 0;
  float displayedAxisValues[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t displayedAxes = 4;
  uint8_t currentCoordSystem = 2; //Not used??
  uint8_t displayedCoordSystem = 2;
  uint8_t prevCoordSystem = 0;
  uint8_t defaultCoordSystem = 2; //G5x
  float spindleOverride = 1;
  float spindleRpm = default_spindle_speed; // Spindle rpm set on the pendant
  float spindleSpeed = 0; // Set speed of the spindle on the machine
  //bool spindleArmed = false;
  int8_t currentSpindleDir = 0;
  float feedrate = 1;
  float rapidrate = 1;
  float rapidSpeed = 0;
  float current_vel = 0; //From machine
  float rapid_vel = 0; //locally calculated for display based on current_vel & motion_type
  float feed_vel = 0; //locally calculated for display based on current_vel & motion_type
  Motion_type_e motion_type = MOTION_TYPE_NONE;
  //Jogging
  float jogIncrements[4] = {0.001, 0.01, 0.1, 1}; //Local display only
  uint8_t currentJogIncrement = 3;
  float defaultJogVelocity[2] = {180, 3000}; //mm/min
  float jogVelocity[2] = { defaultJogVelocity[0], defaultJogVelocity[1] }; //Sent to serial (as mm/min) but does not update gmoccapy
  JogRange_e jogVelocityRange = JOG_RANGE_HIGH;
  float minJogVelocity = 15;
  float maxJogVelocity = 6000;
  //
  bool refreshDisplay = false;
  Screen_e screen = SCREEN_INIT;
  Screen_e previousScreen = SCREEN_INIT;
  char keypadDisplayBuf[20] = "0";
  bool keyUntouched[4][5]; //Used to delay turning off touched key highlight/feedback

  uint8_t flood = FLOOD_OFF;
  uint8_t mist = MIST_OFF;



  //The row of buttons
  ButtonRow_e buttonRow = BUTTON_ROW_NONE;
  ButtonRow_e previousButtonRow = BUTTON_ROW_NONE;

  bool isTaskState(Task_state_e s) {
    return s == task_state;
  }

  bool isHomed() {
    return all_homed == 1;
  }

  bool isReady() {
    return ( task_state == STATE_ON && isHomed() );
  }

  bool isIdle() {
    return isReady() && interpState == INTERP_IDLE;
  }

  bool isManual() {
    return task_mode == MODE_MANUAL;
  }

  bool isMdi() {
    return task_mode == MODE_MDI;
  }

  bool isAuto() {
    return task_mode == MODE_AUTO;
  }



  bool isButtonRow(ButtonRow_e b) {
    return b == buttonRow;
  }


  bool isScreen(Screen_e s) {
    return s == screen;
  }


  bool isTaskMode(Task_mode_e mode) {
    return mode == task_mode;
  }


  bool isProgramState(Program_state_e ps) {
    return ps == program_state;
  }

};
