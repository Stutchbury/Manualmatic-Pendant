/**
 * @file ManualmaticConsts.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief Mostly enums but a fair few consts as well.
 * Where applicable, the enums map onto the values used in the LinuxCNC 
 * Python Interface.
 * All valid commands are defined here - the same mapping exists in the Python class.
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */


#ifndef ManualmaticConsts_h
#define ManualmaticConsts_h

#include <Arduino.h>

#define MANUALMATIC_VERSION "0.1.0"
// Color definitions @TODO Move to display?
//https://github.com/adafruit/Adafruit_ILI9341/blob/master/Adafruit_ILI9341.h
//https://chrishewett.com/blog/true-rgb565-colour-picker/
#define BLACK 0x0000       ///<   0,   0,   0
#define NAVY 0x000F        ///<   0,   0, 123
#define DARKGREEN 0x03E0   ///<   0, 125,   0
#define DARKCYAN 0x03EF    ///<   0, 125, 123
#define MAROON 0x7800      ///< 123,   0,   0
#define PURPLE 0x780F      ///< 123,   0, 123
#define OLIVE 0x7BE0       ///< 123, 125,   0
#define LIGHTGREY 0xC618   ///< 198, 195, 198
#define DARKGREY 0x7BEF    ///< 123, 125, 123
#define BLUE 0x001F        ///<   0,   0, 255
#define GREEN 0x07E0       ///<   0, 255,   0
#define CYAN 0x07FF        ///<   0, 255, 255
#define RED 0xF800         ///< 255,   0,   0
#define MAGENTA 0xF81F     ///< 255,   0, 255
#define YELLOW 0xFFE0      ///< 255, 255,   0
#define WHITE 0xFFFF       ///< 255, 255, 255
#define ORANGE 0xFD20      ///< 255, 165,   0
#define GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define PINK 0xFC18        ///< 255, 130, 198
#define LIGHTGREEN 0x8fee  


/**
 * @brief Follows the convention of linuxcnc (except INIT)
 */
enum Task_state_e : uint8_t {
  STATE_INIT, STATE_ESTOP, STATE_ESTOP_RESET, STATE_OFF, STATE_ON
};

/**
 * @brief Follows the convention of linuxcnc (except UNKNOWN)
 */
enum Task_mode_e : uint8_t {
  MODE_UNKNOWN, MODE_MANUAL, MODE_AUTO, MODE_MDI
};

/**
 * @brief Follows the convention of linuxcnc
 */
enum Motion_type_e : uint8_t {
  MOTION_TYPE_NONE, MOTION_TYPE_TRAVERSE, MOTION_TYPE_FEED, MOTION_TYPE_ARC, MOTION_TYPE_TOOLCHANGE, MOTION_TYPE_PROBING, MOTION_TYPE_INDEXROTARY
};

/**
 * @brief Follows the convention of linuxcnc - except INTERP_UNKNOWN
 */
enum Interp_e : uint8_t {
  INTERP_UNKNOWN, INTERP_IDLE, INTERP_READING, INTERP_PAUSED, INTERP_WAITING
};

/**
 * @brief Follows the convention of linuxcnc
 */
enum Flood_e : uint8_t {
  FLOOD_OFF, FLOOD_ON
};

/**
 * @brief Follows the convention of linuxcnc
 */
enum Mist_e : uint8_t {
  MIST_OFF, MIST_ON
};


/**
 * @brief Follows the convention of linuxcnc
 */
enum AutoCmd_e : uint8_t {
  AUTO_RUN, AUTO_STEP, AUTO_PAUSE, AUTO_RESUME
};

/**
 * @brief Follows the convention of linuxcnc
 */
enum Exec_state_e : uint8_t {
  RCS_DONE, RCS_EXEC, RCS_ERROR
};

/**
 * @brief The state of a program when in 'auto' mode
 * 
 */
enum Program_state_e : uint8_t {
  PROGRAM_STATE_NONE, PROGRAM_STATE_RUNNING, PROGRAM_STATE_PAUSED, PROGRAM_STATE_STOPPED
};


enum Operation_e : uint8_t {
  OPERATION_NONE, OPERATION_POWERFEED
};


/**
 * @brief Valid values for cmd[0]
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
  CMD_JOG_CONTINUOUS = 'N', //Jog continuous (Nudge) payload is velocity
  CMD_JOG_STOP = 'n', //Jog stop (don't nudge)
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
  CMD_FLOOD = 'C', //Coolant IN/OUT
  CMD_MIST = 'c', //Little Coolant IN/OUT
  CMD_EXEC_STATE = 'e', 
  CMD_PROGRAM_STATE = 'p',
  CMD_AUTO = 'a'
};

/**
 * @brief Valid values for cmd[1] when cmd[0] is CMD_INI_VALUE
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
  INI_MAX_LINEAR_VELOCITY = 'V',
  INI_COMPLETE = '.'
};


/**
 * @brief A list of axis + 'none'
 * 
 */
enum Axis_e : int8_t {
  AXIS_NONE=-1, AXIS_X, AXIS_Y, AXIS_Z, AXIS_A, AXIS_B, AXIS_C, AXIS_U, AXIS_V, AXIS_W
};

/**
 * @brief Jog velocity can be toggled between high & low range
 * 
 */
enum JogRange_e : uint8_t {
  JOG_RANGE_LOW, JOG_RANGE_HIGH
};

/**
 * @brief Map the axis position (0-8) to its well-known name
 * 
 */
const char AXIS_NAME[] = "XYZABCUVW";

enum Screen_e : uint8_t {
  SCREEN_INIT, SCREEN_SPLASH, SCREEN_ESTOP, SCREEN_ESTOP_RESET, SCREEN_MANUAL, SCREEN_MDI, SCREEN_AUTO, SCREEN_OFFSET_KEYPAD
};

/**
 * @brief Gives context to ButtonType_e
 */
enum ButtonRow_e : uint8_t {
  BUTTON_ROW_NONE, BUTTON_ROW_DEFAULT, BUTTON_ROW_MANUAL, BUTTON_ROW_AUTO, BUTTON_ROW_MDI, BUTTON_ROW_SPINDLE_START, BUTTON_ROW_SPINDLE_STOP, BUTTON_ROW_PROGRAM_START, BUTTON_ROW_G5X_OFFSET
};

/**
 * @brief type of button. Should behave based on context of ButtonRow.
 */
enum ButtonType_e : uint8_t {
  BUTTON_NONE, BUTTON_HALT, BUTTON_STOP, BUTTON_CANCEL, BUTTON_TICK, BUTTON_TOUCHOFF, BUTTON_HALF, BUTTON_PLAY, BUTTON_PAUSE, BUTTON_ONE_STEP, BUTTON_COOLANT
};

enum ButtonState_e : uint8_t {
  BUTTON_STATE_NONE, BUTTON_STATE_ACTIVE, BUTTON_STATE_INACTIVE, BUTTON_STATE_ON /*implied active*/, BUTTON_STATE_OFF /*implied active*/
};

/**
 * @brief Duration of a long click in milliseconds
 * 
 */
const unsigned int heartbeatMs = 500;

/**
 * @brief Duration of a long click in milliseconds
 * 
 */
const unsigned int longClickDuration = 700;

/**
 * @brief The refresh rate of the display in milliseconds
 * 
 */
const uint16_t displayRefreshMs = 100; //200

/**
 * @brief Rate limit the MPG to not swamp Serial
 */
const unsigned long mpgRateLimit = 20; //ms
/**
 * @brief Rate limit the spindle encoder for acceleration
 */
const unsigned long spindleRateLimit = 100; //ms
/**
 * @brief Rate limit the feed encoder for acceleration
 */
const unsigned long feedRateLimit = 100; //ms

const char coordSystem[3][4] = {"Abs", "DTG", "G54"};

enum ErrorMessage_e : uint8_t {
    ERRMSG_NONE,
    ERRMSG_NOT_HOMED,
};

#endif //ManualmaticConsts_h