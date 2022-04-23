/**
 * A bunch of methods to format & send specific messages
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */
#include "Arduino.h"
#include "ManualmaticConsts.h"
#include "ManualmaticConfig.h"
#include "ManualmaticMessenger.h"

ManualmaticMessenger::ManualmaticMessenger(ManualmaticMessage& message, ManualmaticConfig& config, ManualmaticState& s) 
    : serialMessage(message),
      config(config),
      state(s)
  {}

/** *************************************************************
 *  machine state
 */

void ManualmaticMessenger::ManualmaticMessenger::setMachineState(Task_state_e state) {
  char cmd[4];
  cmd[0] = CMD_TASK_STATE;
  cmd[1] = state+'0'; //Shift +48 for char of axis number  
  serialMessage.send(cmd);
}

void ManualmaticMessenger::ManualmaticMessenger::sendAbort() {
  serialMessage.send('!');
}

/** *************************************************************
 *  Jogging
 */

/**
 * Jog the axis by distance
 * Note: velocity is already set
 */
void ManualmaticMessenger::ManualmaticMessenger::jogAxis(uint8_t axis, float distance ) {
    char cmd[4];
    cmd[0] = CMD_JOG;
    cmd[1] = axis+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd, distance);
}

/**
 * Jog the axis continuously by velocity
 * Pass zero to stop
 */
void ManualmaticMessenger::ManualmaticMessenger::jogAxisContinuous(uint8_t axis, float velocity ) {
    char cmd[3];
    cmd[0] = CMD_JOG_CONTINUOUS;
    cmd[1] = axis+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd, velocity);
}

/**
 * Jog the axis continuously by velocity
 * Pass zero to stop
 */
void ManualmaticMessenger::jogAxisStop(uint8_t axis ) {
    char cmd[3];
    cmd[0] = CMD_JOG_STOP;
    cmd[1] = axis+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd);
}



/**
 * Toggle between Tortoise and Rabbit jog speeds
 * 
 */
void ManualmaticMessenger::toggleJogRange() {
  state.jogVelocityRange = (state.jogVelocityRange == JOG_RANGE_LOW) ? JOG_RANGE_HIGH : JOG_RANGE_LOW;  
  serialMessage.send(CMD_JOG_VELOCITY, state.jogVelocity[state.jogVelocityRange]);
}

/**
 * Reset both Tortoise and Rabbit jog velocity to their defaults
 */
void ManualmaticMessenger::resetJogVelocity() {
  state.jogVelocity[JOG_RANGE_LOW] = config.defaultJogVelocity[JOG_RANGE_LOW];  
  state.jogVelocity[JOG_RANGE_HIGH] = config.defaultJogVelocity[JOG_RANGE_HIGH];  
  serialMessage.send(CMD_JOG_VELOCITY, state.jogVelocity[state.jogVelocityRange]);
}

/**
 * Increment the jogVelocity and send to serial
 * Serial will return the set velocity which will update the display
 */
void ManualmaticMessenger::incrementJogVelocity(int16_t incr) {
    float velocity = state.jogVelocity[state.jogVelocityRange] + (incr * (config.jogVelocityIncrement[state.jogVelocityRange]));    
    velocity = max(min(velocity, config.maxJogVelocity), config.minJogVelocity);
    serialMessage.send(CMD_JOG_VELOCITY, velocity);
}

/** *****************************************
 * Zero or half axis
 */
void ManualmaticMessenger::sendG5xOffset(uint8_t axis, char offset[20] ) {
    char cmd[3];
    cmd[0] = CMD_G5X_OFFSET;
    cmd[1] = axis+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd, offset);
}


/**
 * Directly control spindle RPM
 */
void ManualmaticMessenger::sendSpindleRpm() {
  serialMessage.send(CMD_SPINDLE_SPEED, state.spindleRpm, 0);
}

/**
 * Stop the spindle
 */
void ManualmaticMessenger::stopSpindle() {
  //state.spindleArmed = false;
  //state.spindleRpm = 0;
  state.currentSpindleDir = 0;
  if ( state.spindleSpeed != 0 ) {
    serialMessage.send(CMD_SPINDLE_SPEED, 0);  
  }
}

/**
 * Start the spindle
 */
void ManualmaticMessenger::startSpindle() {
    if ( /*istate.spindleArmed == true 
      &&*/ state.spindleSpeed == 0 ) {
      sendSpindleRpm();
      state.currentSpindleDir = state.spindleRpm > 0 ? 1: -1;
    }
}


/**
 * Send the spindle override
 */
void ManualmaticMessenger::sendSpindleOverride(float pct /*=1*/) {
  serialMessage.send(CMD_SPINDLE_OVERRIDE, pct, 3);
}

void ManualmaticMessenger::incrementSpindleOverride(int16_t incr) {
  float rate = state.spindleOverride + (state.spindleOverride * (incr * 0.005));
  rate = max(min(rate, config.max_spindle_override), config.min_spindle_override);
  serialMessage.send(CMD_SPINDLE_OVERRIDE, rate, 3);
}

void ManualmaticMessenger::sendFeedrate(float pct /*=1*/) {
  serialMessage.send(CMD_FEED_OVERRIDE, pct, 3);  
}


void ManualmaticMessenger::sendRapidrate(float pct /*=1*/) {
  serialMessage.send(CMD_RAPID_OVERRIDE, pct, 3);  
}

/**
 * 
 */
void ManualmaticMessenger::incrementFeedrate(int16_t incr) {
  float rate = state.feedrate + (state.feedrate * (incr*0.01));
  //@TODO Check this
  rate = max(min(rate, config.max_feed_override), 0.01);
  serialMessage.send(CMD_FEED_OVERRIDE, rate, 3);
}

/**
 * 
 */
void ManualmaticMessenger::incrementRapidrate(int16_t incr) {
    //@TODO Check this
    float rate = state.rapidrate + (state.rapidrate * (incr*0.01));
    rate = max(min(rate, config.max_rapid_override), 0.01);
    serialMessage.send(CMD_RAPID_OVERRIDE, rate, 3);
}

void ManualmaticMessenger::sendTaskMode(uint8_t mode) {
    char cmd[3];
    cmd[0] = CMD_TASK_MODE;
    cmd[1] = mode+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd);  
}

void ManualmaticMessenger::sendInit() {
  resetJogVelocity();
}


void ManualmaticMessenger::sendFlood(Flood_e onOff) {
    char cmd[3];
    cmd[0] = CMD_FLOOD;
    cmd[1] = onOff+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd);    
}

void ManualmaticMessenger::sendMist(Mist_e onOff) {
    char cmd[3];
    cmd[0] = CMD_MIST;
    cmd[1] = onOff+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd);    
}

void ManualmaticMessenger::sendAuto(AutoCmd_e ac) {
    char cmd[3];
    cmd[0] = CMD_AUTO;
    cmd[1] = ac+'0'; //Shift +48 for char
    serialMessage.send(cmd);      
}

