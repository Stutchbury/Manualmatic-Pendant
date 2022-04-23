/**
 * This is the class that handles incoming and outgoing messages over Serial.
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
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */

#include "ManualmaticMessage.h"


    /** ***************************************************
       Constructor
    */
    ManualmaticMessage::ManualmaticMessage(Stream& stream) : serial(stream) {};

    /**
       Read bytes from serial and try to format a message
       Returns true if a message has been read, filling cmd with a maximum
       of two bytes and payload with up to 30.
    */
    bool ManualmaticMessage::available(char *cmd, char *payload) {
      if (serial.available() > 2 ) {      // If anything comes in Serial (USB),
        char c = Serial.read();
        if ( c == STX ) {
          //tft.println("Have STX");
          //Read the next char into cmd
          cmd[0] = Serial.read();
          cmd[1] = Serial.read();
          //int payloadLen = Serial.readBytesUntil(ETX,rxBuffer,20);
          int i = 0;
          for (i = 0; i < 30; i++ ) {
            c = Serial.read();
            if ( c == ETX ) {
              //tft.println("Have ETX");
              payload[i] = '\0';
              return true;
              break;
            } else {
              payload[i] = c;
            }
          }
          payload[29] = '\0'; //Should never get here
        }
      }
      return false;
    }
    /**
     * Should never be called but need to override Print::write(uint8_t)
     */
    size_t ManualmaticMessage::write(uint8_t) {
      return 0;
    }


    /**
     * Send message for single char cmd (no payload)
     */
    size_t ManualmaticMessage::send(const char cmd ) {
      startMessage(cmd);
      endMessage();
      return 0;
    }
    // And multiple char cmd (no payload)
    size_t ManualmaticMessage::send(const char cmd[] ) {
      startMessage(cmd);
      endMessage();
      return 0;
    }

    /**
       Replicate the Print::print() methods but with cmd as a parameter for 
       both single char cmd and multiple chars
    */

    /**
     * 
     */
    size_t ManualmaticMessage::send(const char cmd, const char payload[] ) {
      startMessage(cmd);
      print(payload);
      endMessage();
      return 0;
    }
    size_t ManualmaticMessage::send(const char cmd[], const char payload[] ) {
      startMessage(cmd);
      print(payload);
      endMessage();
      return 0;
    }


    /**
     * 
     */
    size_t ManualmaticMessage::send(const char cmd, char payload) {
      startMessage(cmd);
      print(payload);
      endMessage();
      return messageSize;
    }
    size_t ManualmaticMessage::send(const char cmd[], char payload) {
      startMessage(cmd);
      print(payload);
      endMessage();
      return messageSize;
    }

    /**
     * 
     */
    size_t ManualmaticMessage::send(const char cmd, unsigned char payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }
    size_t ManualmaticMessage::send(const char cmd[], unsigned char payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }


    /**
     * 
     */
    size_t ManualmaticMessage::send(const char cmd, int payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }
    size_t ManualmaticMessage::send(const char cmd[], int payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }
    
    /**
     * 
     */
    size_t ManualmaticMessage::send(const char cmd, unsigned int payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }
    size_t ManualmaticMessage::send(const char cmd[], unsigned int payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }

    /*
     * 
     */
    size_t ManualmaticMessage::send(const char cmd, long payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }
    size_t ManualmaticMessage::send(const char cmd[], long payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }
    /**
     * 
     */
    size_t ManualmaticMessage::send(const char cmd, unsigned long payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }
    size_t ManualmaticMessage::send(const char cmd[], unsigned long payload, int base /*= DEC*/) {
      startMessage(cmd);
      print(payload, base);
      endMessage();
      return messageSize;
    }
    /**
     * 
     */
    size_t ManualmaticMessage::send(const char cmd, double payload, int precision /*= 4*/) {
      startMessage(cmd);
      print(payload, precision);
      endMessage();
      return messageSize;
    }
    size_t ManualmaticMessage::send(const char cmd[], double payload, int precision /*= 4*/) {
      startMessage(cmd);
      print(payload, precision);
      endMessage();
      return messageSize;
    }

    
    /**
       Add the payload to the message
       Overrides Print::write(const uint8_t *, size_t)
    */
    size_t ManualmaticMessage::write(const uint8_t *buffer, size_t size) {
      while (size-- && messageSize < 34) {
        message[messageSize++] = *buffer++;
      }
      return messageSize; //not really necessary cos we work directly with messageSize
    }


    /**
       Don't think we need to implement this (only one thread).
    */
    int ManualmaticMessage::availableForWrite() {
      return 0;
    }

    void ManualmaticMessage::flush() {
      /* Empty implementation for backward compatibility */
    }

    /**
       Start building the message
    */
    void ManualmaticMessage::startMessage(const char cmd[]) {
      messageSize = 0;
      //Start of message
      message[messageSize++] = STX;
      //Add the cmd
      message[messageSize++] = cmd[0];
      //Blank char if no cmd[1]
      message[messageSize++] = cmd[1] == '\0' ? ' ' : cmd[1];
    }

    void ManualmaticMessage::startMessage(const char cmd) {
      const char c[2] = { cmd, '\0' };
      startMessage(c);
    }


    /**
     * (S)End the message
     */
    void ManualmaticMessage::endMessage() {
      message[messageSize++] = ETX;
      message[messageSize] = '\0';
      serial.print(message);
    }
