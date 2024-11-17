/**
 * @file ManualmaticState.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief Maintains the state of the pendant.
 * 
 * Contains structures that maintain state for both control and display.
 * This allows us to only draw values on the display that have changed
 * - writing to the screen over SPI is relatively slow (compared to a
 * Teensy) so we only want to do it when absolutely necessary.
 * 
 * 
 * There are a lot of conflicting naming conventions here - I've
 * tried to follow the Python 'underscore' naming convention where I'm mirroring
 * properties in the LinuxCNC Interface class but my natural inclination
 * is towards camelCase (for local members & methods), so there's a bit of both...
 * Sorry about that.
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef ManualmaticState_h
#define ManualmaticState_h

#include "ManualmaticWiring.h"
#include "ManualmaticConsts.h"
#include "ManualmaticConfig.h"
#include "ManualmaticUtils.h"



/**
 * @brief The state of the Manualmatic Pendant
 * 
 */
class ManualmaticState {
  public:
    /**
     * @brief Construct a new Manualmatic State object
     * 
     * @param config Requires manualmaticConfig
     */
    ManualmaticState(ManualmaticConfig& config);

    /**
     * @brief Update the state with a cmd and payload
     * 
     * @param cmd 
     * @param payload 
     */
    void update(char cmd[2], char payload[30]);

    /**
     * @brief Set state values from ini file values 
     * @TODO move to config?
     * 
     * @param cmd1 
     * @param payload 
     */
    void setIniValue(char cmd1, char* payload);

    // /**
    //  * @brief Used by ManualmaticControl
    //  * 
    //  */
    // struct ControlState_s {
    //   ButtonRow_e buttonRow = BUTTON_ROW_NONE;
    // };
    
    // Hold a consistent 'time' for each loop
    uint32_t now = millis();
    uint32_t lastHeartbeatSent = 0; //0 denotes no heartbeat
    uint32_t lastHeartbeatReceived = 0; //0 denotes no heartbeat
    bool pulse = false; //Toggled by send hearbeat

    uint8_t estop_is_activated = digitalRead(SOFT_ESTOP);
    Task_state_e task_state = STATE_INIT;
    Task_mode_e task_mode = MODE_UNKNOWN;
    Interp_e interpState = INTERP_IDLE;
    Exec_state_e exec_state = RCS_DONE;
    Program_state_e program_state = PROGRAM_STATE_NONE;
    Operation_e currentOperation = OPERATION_NONE;
    Axis_e currentAxis = AXIS_NONE;
    Flood_e flood = FLOOD_OFF;
    Mist_e mist = MIST_OFF;
    uint8_t g5xIndex = 1;
    float g5xOffsets[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float g92Offsets[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float toolOffsets[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float axisAbsPos[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float axisDtg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t homed[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t all_homed = 0;
    float displayedAxisValues[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t displayedAxes = 4;
    //uint8_t currentCoordSystem = 2; //Not used??
    Display_coords_e displayedCoordSystem = DISPLAY_COORDS_G5X;
    Display_coords_e prevCoordSystem = DISPLAY_COORDS_NONE;
    float spindleOverride = 1;
    float spindleSpeed = 0; // Commanded spindle speed
    float spindleRpm = 0; // Actual spindle rpm
    int8_t spindleDirection = 0; //Indicates whether spindle is on or off 1=fwd, -1=rev, 0=stopped
    float feedrate = 1;
    float rapidrate = 1;
    float rapidSpeed = 0;
    float current_vel = 0; //From machine
    float rapid_vel = 0; //locally calculated for display based on current_vel & motion_type
    float feed_vel = 0; //locally calculated for display based on current_vel & motion_type
    Motion_type_e motion_type = MOTION_TYPE_NONE;
    //Jogging
    uint8_t currentJogIncrement = 3;
    //float jogVelocity[2] = { defaultJogVelocity[0], defaultJogVelocity[1] }; //Sent to serial (as mm/min) but does not update gmoccapy
    float jogVelocity[2] = { 180, 3000 }; //Sent to serial (as mm/min) but does not update gmoccapy
    JogRange_e jogVelocityRange = JOG_RANGE_HIGH;
    //
    Ini_state_e iniState = INI_STATE_DISCONNECTED;
    //
    bool refreshDisplay = false;
    Screen_e screen = SCREEN_INIT;
    Screen_e previousScreen = SCREEN_INIT;

    Axis_e joystickAxis[2] = {AXIS_NONE, AXIS_NONE};

    //The row of buttons
    ButtonRow_e buttonRow = BUTTON_ROW_NONE;
    ButtonRow_e previousButtonRow = BUTTON_ROW_NONE;

    ErrorMessage_e errorMessage = ERRMSG_NONE;
    int errorMessageStartTime;

    char debug[20];

    // /**
    //  * @brief A public struct for use by ManualmaticControl.
    //  * 
    //  */
    // ControlState_s control;


    bool setTaskMode(Task_mode_e mode, bool force=false);

    bool setScreen(Screen_e s );
    
    void setTaskState(Task_state_e s);

    bool setButtonRow(ButtonRow_e b=BUTTON_ROW_DEFAULT );

    bool isTaskState(Task_state_e s) {
      return s == task_state;
    }

    bool isHomed() {
      return all_homed == 1;
    }

    /**
     * @brief Check if the machine is on and homed.
     * 
     * By default, set a message if not homed. Can be used silently by
     * passing 'false'.
     * 
     */
    bool isReady(bool setMessage = true);

    bool isIdle() {
      return isReady(false) && interpState == INTERP_IDLE;
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

    /**
     * Calculate the displayed values for feed and rapid based on motion_type & current_vel
     */
    void setCurrentVelocities();

    void incrementJogIncrement(int16_t incr);


    /**
     * Increment the spindle RPM (Manual mode)
     * Ensure we cannot accidentally reverse direction or 
     * exceed max spindle RPM
     */
    void incrementSpindleSpeed(int16_t incr);

    /**
     * Reset the spindle defaults (RPM & percent)
     */
    void resetSpindleDefaults();

    void setErrorMessage(ErrorMessage_e error);

    void onConnected();
    void onDisconnected();

  private:
    ManualmaticConfig& config;




};

#endif //ManualmaticState_h
