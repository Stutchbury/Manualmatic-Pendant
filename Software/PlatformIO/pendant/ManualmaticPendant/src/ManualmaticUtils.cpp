#include "ManualmaticUtils.h"


#include <Arduino.h>
/**
 * A few utility functions...
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */


/**
   Pass __DATE__
   Used to display the last compiled date at startup
*/
void version(char const *date, char *buff) {
  int month, day, year;
  static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  sscanf(date, "%s %d %d", buff, &day, &year);
  month = (strstr(month_names, buff) - month_names) / 3 + 1;
  sprintf(buff, "%d.%02d.%02d", year, month, day);
}


/**
   Remove trailing whitespace characters from a string
   Thank you:
   https://www.codezclub.com/c-trim-trailing-white-space-characters-in-a-string/
*/
void trimTrailing(char * string) {
  int lastIndex=0, i;
  /* Finds the last non white space character */
  i = 0;
  while (string[i] != '\0') {
    if (string[i] != ' ' && string[i] != '\t' && string[i] != '\n') {
      lastIndex = i;
    }
    i++;
  }
  /* Mark the next character to last non white space character as NULL */
  string[lastIndex + 1] = '\0';
}

