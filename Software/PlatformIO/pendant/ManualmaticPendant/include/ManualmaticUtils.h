/**
 * @file ManualmaticUtils.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief A few utility functions...
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */
#ifndef ManualmaticUtils_h
#define ManualmaticUtils_h

#include <Arduino.h>
#include "ManualmaticConsts.h"

/**
   Pass __DATE__
   Used to display the last compiled date at startup
*/
void version(char const *date, char *buff);
/**
 * Used for sin cos etc.
 */
double inline degree2radian(int16_t a) {
  return (a * 0.017453292519);
}

/**
   Remove trailing whitespace characters from a string
   Thank you:
   https://www.codezclub.com/c-trim-trailing-white-space-characters-in-a-string/
*/
void trimTrailing(char * string);

#endif //ManualmaticUtils_h
