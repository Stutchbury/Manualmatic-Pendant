/**
 * @file ManualmaticConfig.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief Configuration values that are normally set from a .ini file
 * Some can be overriden at runtime.
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */

#ifndef ManualmaticConfig_h
#define ManualmaticConfig_h

#include "ManualmaticConsts.h"

/**
 * @brief Configuration values that are normally set from a .ini file
 * 
 */
class ManualmaticConfig {


  public:

    uint8_t axes = 4;

    uint8_t defaultCoordSystem = 2; //G5x @TODO Not used yet


    float max_feed_override = 1.2;
    float max_rapid_override = 1.2; //@TODO set in ini?
    float min_spindle_override = 0.5;
    float max_spindle_override = 1.5;
    float default_spindle_speed = 450;
    // In machine units per second - converted in setIniValue() to machine units per minute
    float default_linear_velocity = 1;
    float max_linear_velocity = 5;


    float jogIncrements[4] = {0.001, 0.01, 0.1, 1}; //Local display only
    float defaultJogVelocity[2] = {100, 1000}; //mm/min
    float minJogVelocity = 15;
    float maxJogVelocity = 1500;


    float jogVelocityIncrement[2] = {5,25}; //mm/min

    uint16_t joystickStartBoundary = 50;
    uint16_t joystickEndBoundary = 50;
    uint8_t numJoystickIncrements = 8;
    Axis_e joystickAxisDefault[2] = {AXIS_X, AXIS_Y};
    Axis_e joystickAxisAlt[2] = {AXIS_A, AXIS_Z};
  
};


#endif //ManualmaticConfig_h
