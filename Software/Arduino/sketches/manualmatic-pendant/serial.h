/**
 * Probably confusingly named but these functions format & send messages
 * via SerialMessage. They *do not* perform validation.
 * 
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */


/** *************************************************************
 *  machine state
 */

void setMachineState(Task_state_e state) {
  char cmd[4];
  cmd[0] = CMD_TASK_STATE;
  cmd[1] = state+'0'; //Shift +48 for char of axis number  
  serialMessage.send(cmd);
}

void sendAbort() {
  serialMessage.send('!');
}

/** *************************************************************
 *  Jogging
 */

/**
 * Jog the axis by distance
 * Note: velocity is already set
 */
void jogAxis(uint8_t axis, float distance ) {
    char cmd[4];
    cmd[0] = CMD_JOG;
    cmd[1] = axis+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd, distance);
}

/**
 * Toggle between Tortoise and Rabbit jog speeds
 * 
 */
void toggleJogRange() {
  state.jogVelocityRange = (state.jogVelocityRange == JOG_RANGE_LOW) ? JOG_RANGE_HIGH : JOG_RANGE_LOW;  
  serialMessage.send(CMD_JOG_VELOCITY, state.jogVelocity[state.jogVelocityRange]);
}

/**
 * Reset both Tortoise and Rabbit jog velocity to their defaults
 */
void resetJogVelocity() {
  state.jogVelocity[JOG_RANGE_LOW] = state.defaultJogVelocity[JOG_RANGE_LOW];  
  state.jogVelocity[JOG_RANGE_HIGH] = state.defaultJogVelocity[JOG_RANGE_HIGH];  
  serialMessage.send(CMD_JOG_VELOCITY, state.jogVelocity[state.jogVelocityRange]);
}

/**
 * Increment the jogVelocity and send to serial
 * Serial will return the set velocity which will update the display
 */
void incrementJogVelocity(int16_t incr) {
    float velocity = state.jogVelocity[state.jogVelocityRange] + (incr * (jogVelocityIncrement[state.jogVelocityRange]));    
    velocity = max(min(velocity, state.maxJogVelocity), state.minJogVelocity);
    serialMessage.send(CMD_JOG_VELOCITY, velocity);
}

/** *****************************************
 * Zero or half axis
 */
void sendG5xOffset(uint8_t axis, char offset[20] ) {
    char cmd[3];
    cmd[0] = CMD_G5X_OFFSET;
    cmd[1] = axis+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd, offset);
}


/**
 * Directly control spindle RPM
 */
void sendSpindleRpm() {
  serialMessage.send(CMD_SPINDLE_SPEED, state.spindleRpm, 0);
}

/**
 * Stop the spindle
 */
void stopSpindle() {
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
void startSpindle() {
    if ( /*istate.spindleArmed == true 
      &&*/ state.spindleSpeed == 0 ) {
      sendSpindleRpm();
      state.currentSpindleDir = state.spindleRpm > 0 ? 1: -1;
    }
}


/**
 * Send the spindle override
 */
void sendSpindleOverride(float pct=1) {
  serialMessage.send(CMD_SPINDLE_OVERRIDE, pct, 3);
}

void incrementSpindleOverride(int16_t incr) {
  float rate = state.spindleOverride + (state.spindleOverride * (incr * 0.005));
  rate = max(min(rate, state.max_spindle_override), state.min_spindle_override);
  serialMessage.send(CMD_SPINDLE_OVERRIDE, rate, 3);
}

void sendFeedrate(float pct=1) {
  serialMessage.send(CMD_FEED_OVERRIDE, pct, 3);  
}


void sendRapidrate(float pct=1) {
  serialMessage.send(CMD_RAPID_OVERRIDE, pct, 3);  
}

/**
 * 
 */
void incrementFeedrate(int16_t incr) {
  float rate = state.feedrate + (state.feedrate * (incr*0.01));
  //@TODO Check this
  rate = max(min(rate, state.max_feed_override), 0.01);
  serialMessage.send(CMD_FEED_OVERRIDE, rate, 3);
}

/**
 * 
 */
void incrementRapidrate(int16_t incr) {
    //@TODO Check this
    float rate = state.rapidrate + (state.rapidrate * (incr*0.01));
    //@TODO Check this - use ini values
    rate = max(min(rate, 1.2), 0.01);
    serialMessage.send(CMD_RAPID_OVERRIDE, rate, 3);
}

void sendTaskMode(uint8_t mode) {
    char cmd[3];
    cmd[0] = CMD_TASK_MODE;
    cmd[1] = mode+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd);  
}

void sendInit() {
  resetJogVelocity();
}


void sendFlood(Flood_e onOff) {
    char cmd[3];
    cmd[0] = CMD_FLOOD;
    cmd[1] = onOff+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd);    
}

void sendMist(Mist_e onOff) {
    char cmd[3];
    cmd[0] = CMD_MIST;
    cmd[1] = onOff+'0'; //Shift +48 for char of axis number
    serialMessage.send(cmd);    
}

void sendAuto(AutoCmd_e ac) {
    char cmd[3];
    cmd[0] = CMD_AUTO;
    cmd[1] = ac+'0'; //Shift +48 for char
    serialMessage.send(cmd);      
}
