/**
 * @file ManualmaticMessage.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief This is the class that handles incoming and outgoing messages over Serial.
 *
 * We extend Print to take advantage of the multitude of print() overrides
 * for different data types.
 * 
 * It is an *extremely* simple (read: fast, efficient) protocol:
 * 
 * First byte: STX
 * Second byte: Primary command
 * Third byte: Secondary command - often an axis number.
 * Fourth+(max 30) bytes: optional payload
 * Last byte: ETX
 * 
 * All values (including payload) are sent as char
 * 
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */
#ifndef ManualmaticMessage_h
#define ManualmaticMessage_h

#include <Arduino.h>

/**
 * @brief This is the class that handles incoming and outgoing messages over Serial.
 * 
 */
class ManualmaticMessage : public Print {

  public:

   /**
    * @brief Construct a new Manualmatic Message object
    * 
    * @param serial Arduino Stream type (usually Serial)
    */
    ManualmaticMessage(Stream& serial);


    /**
     * @brief Read bytes from serial and try to format a message
     * 
     * @param cmd Filled with command (two bytes) if available
     * @param payload Optionally filled with payload if available (max 30 bytes)
     * @return true if a message has been read
     * @return false if no message available
     */
    bool available(char *cmd, char *payload);

    /**
     * Send message for single char cmd (no payload)
     */
    size_t send(const char cmd);

    // And multiple char cmd (no payload)
    size_t send(const char cmd[]);

    /**
     * Replicate the Print::print() methods but with cmd as a parameter for 
     * both single char cmd and multiple chars
     */
    size_t send(const char cmd, const char payload[] );

    size_t send(const char cmd[], const char payload[] );

    size_t send(const char cmd, char payload);

    size_t send(const char cmd[], char payload);

    size_t send(const char cmd, unsigned char payload, int base = DEC);

    size_t send(const char cmd[], unsigned char payload, int base = DEC);

    size_t send(const char cmd, int payload, int base = DEC);

    size_t send(const char cmd[], int payload, int base = DEC);
    
    size_t send(const char cmd, unsigned int payload, int base = DEC);

    size_t send(const char cmd[], unsigned int payload, int base = DEC);

    size_t send(const char cmd, long payload, int base = DEC);

    size_t send(const char cmd[], long payload, int base = DEC);

    size_t send(const char cmd, unsigned long payload, int base = DEC);

    size_t send(const char cmd[], unsigned long payload, int base = DEC);

    size_t send(const char cmd, double payload, int precision = 4);

    size_t send(const char cmd[], double payload, int precision = 4);

  private:

    Stream& serial;
   /**
    * @brief Defines the start of a ManualmaticMessage
    * 
    */
   const char STX = '\x02';
   /**
    * @brief Defines the end of a ManualmaticMessage
    * 
    */
    const char ETX = '\x03';
    size_t messageSize = 0;
                      // STX Cmd Payload ETX \0
    char message[35]; //  1   2   0-30    1   1

    /**
     * Should never be called but need to override Print::write(uint8_t)
     */
    size_t write(uint8_t);

    
    /**
       Add the payload to the message
       Overrides Print::write(const uint8_t *, size_t)
    */
    size_t write(const uint8_t *buffer, size_t size);


    /**
       Don't think we need to implement this (only one thread).
    */
    int availableForWrite();

    /* Empty implementation for backward compatibility */
    void flush();

    /**
       Start building the message
    */
    void startMessage(const char cmd[]);

    void startMessage(const char cmd);


    /**
     * (S)End the message
     */
    void endMessage();

};


#endif //ManualmaticMessage_h
