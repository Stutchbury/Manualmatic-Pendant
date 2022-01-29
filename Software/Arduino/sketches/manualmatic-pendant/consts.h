/**
 * Mostly enums but a fair few consts as well.
 * Where applicable, the enums map onto the values used in the LinuxCNC 
 * Python Interface.
 * 
 * All valid commands are defined here - the same mapping exists in the Python class.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */

//Follows the convention of linuxcnc (except INIT)
enum Task_state_e : uint8_t {
  STATE_INIT, STATE_ESTOP, STATE_ESTOP_RESET, STATE_OFF, STATE_ON
};

//Follows the convention of linuxcnc (except UNKNOWN)
enum Task_mode_e : uint8_t {
  MODE_UNKNOWN, MODE_MANUAL, MODE_AUTO, MODE_MDI
};

//Follows the convention of linuxcnc
enum Motion_type_e : uint8_t {
  MOTION_TYPE_NONE, MOTION_TYPE_TRAVERSE, MOTION_TYPE_FEED, MOTION_TYPE_ARC, MOTION_TYPE_TOOLCHANGE, MOTION_TYPE_PROBING, MOTION_TYPE_INDEXROTARY
};

//Follows the convention of linuxcnc - except INTERP_UNKNOWN
enum Interp_e : uint8_t {
  INTERP_UNKNOWN, INTERP_IDLE, INTERP_READING, INTERP_PAUSED, INTERP_WAITING
};

//Follows the convention of linuxcnc
enum Flood_e : uint8_t {
  FLOOD_OFF, FLOOD_ON
};

//Follows the convention of linuxcnc
enum Mist_e : uint8_t {
  MIST_OFF, MIST_ON
};


//Follows the convention of linuxcnc
enum AutoCmd_e : uint8_t {
  AUTO_RUN, AUTO_STEP, AUTO_PAUSE, AUTO_RESUME
};

enum Exec_state_e : uint8_t {
  RCS_DONE, RCS_EXEC, RCS_ERROR
};

enum Program_state_e : uint8_t {
  PROGRAM_STATE_NONE, PROGRAM_STATE_RUNNING, PROGRAM_STATE_PAUSED, PROGRAM_STATE_STOPPED
};


enum Operation_e : uint8_t {
  OPERATION_NONE, OPERATION_POWERFEED
};


/**
 * Valid values for cmd[0]
 * Do not use enum class
 */
enum Cmd_e : uint8_t {
  CMD_ABSOLUTE_POS = 'A', // Absolute position
  CMD_SPINDLE_SPEED = 'S', //Spindle speed set on machine
  CMD_SPINDLE_OVERRIDE = 's', //Spindle override IN/OUT
  CMD_FEED_OVERRIDE = 'f', //Feed override IN/OUT
  CMD_RAPID_SPEED = 'R', //Rapid speed @TODO not used?
  CMD_RAPID_OVERRIDE = 'r', //Rapid override IN/OUT
  CMD_JOG = 'J', //Jog
  CMD_JOG_VELOCITY = 'j', //Jog Velocity IN/OUT (sorta)
  CMD_TASK_MODE = 'M', //mode IN/OUT
  CMD_TASK_STATE = 'E', //task_state (Estop and On/Off) IN/OUT
  CMD_INTERP_STATE = 'I', //interp_state
  CMD_CURRENT_VEL = 'v', //current_vel
  CMD_MOTION_TYPE = 't', //motion_type
  CMD_G5X_OFFSET = 'O', //g5x_offset IN/OUT
  CMD_DTG = 'D', //DTG
  CMD_ALL_HOMED = 'H', //homed
  CMD_HOMED = 'h', //homed
  CMD_INI_VALUE = 'i', //ini value
  CMD_FLOOD = 'C', //Coolant
  CMD_MIST = 'c', //Little Coolant
  CMD_EXEC_STATE = 'e', 
  CMD_PROGRAM_STATE = 'p',
  CMD_AUTO = 'a'
};

/**
 * Valid values for cmd[1] when cmd[0] is CMD_INI_VALUE
 */
enum Cmd_ini_e : uint8_t {
  INI_AXES = 'a', //Number of axes
  INI_MAX_FEED_OVERRIDE ='F',
  INI_MIN_SPINDLE_OVERRIDE = 's',
  INI_MAX_SPINDLE_OVERRIDE = 'S',
  INI_DEFAULT_SPINDLE_SPEED = 'r', //#r for RPM
  INI_LINEAR_UNITS = 'U',
  INI_ANGULAR_UNITS = 'u',
  INI_DEFAULT_LINEAR_VELOCITY = 'v',
  INI_MAX_LINEAR_VELOCITY = 'V'
};



enum Axis_e : int8_t {
  AXIS_NONE=-1, AXIS_X, AXIS_Y, AXIS_Z, AXIS_A, AXIS_B, AXIS_C, AXIS_U, AXIS_V, AXIS_W
};

enum JogRange_e : uint8_t {
  JOG_RANGE_LOW, JOG_RANGE_HIGH
};

const char AXIS_NAME[] = "XYZABCUVW";

const char STX = '\x02';
const char ETX = '\x03';

enum Screen_e : uint8_t {
  SCREEN_INIT, SCREEN_SPLASH, SCREEN_ESTOP, SCREEN_ESTOP_RESET, SCREEN_MANUAL, SCREEN_MDI, SCREEN_AUTO, SCREEN_OFFSET_KEYPAD
};

/**
 * Gives context to ButtonType_e
 */
enum ButtonRow_e : uint8_t {
  BUTTON_ROW_NONE, BUTTON_ROW_DEFAULT, BUTTON_ROW_MANUAL, BUTTON_ROW_AUTO, BUTTON_ROW_MDI, BUTTON_ROW_SPINDLE_START, BUTTON_ROW_SPINDLE_STOP, BUTTON_ROW_PROGRAM_START, BUTTON_ROW_G5X_OFFSET
};

/**
 * The type of button. Should behave based on context of ButtonRow.
 */
enum ButtonType_e : uint8_t {
  BUTTON_NONE, BUTTON_HALT, BUTTON_STOP, BUTTON_CANCEL, BUTTON_TICK, BUTTON_TOUCHOFF, BUTTON_HALF, BUTTON_PLAY, BUTTON_PAUSE, BUTTON_ONE_STEP, BUTTON_COOLANT
};

enum ButtonState_e : uint8_t {
  BUTTON_STATE_NONE, BUTTON_STATE_ACTIVE, BUTTON_STATE_INACTIVE, BUTTON_STATE_ON /*implied active*/, BUTTON_STATE_OFF /*implied active*/
};

const unsigned int longClickDuration = 700;
const uint16_t displayRefreshMs = 100; //200

//Rate limit the MPG to not swamp Serial
const ulong mpgRateLimit = 20; //ms
//Rate limit the spindle & feed encoders for acceleration
const ulong spindleRateLimit = 100; //ms
const ulong feedRateLimit = 100; //ms


float jogVelocityIncrement[2] = {5,25}; //mm/min

const char coordSystem[3][4] = {"Abs", "DTG", "G54"};  
