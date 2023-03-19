/**
 * @file ManualmaticMessenger.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief A helper class containing a bunch of methods to format & send specific messages
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */
#include "ManualmaticMessage.h"
#include "ManualmaticConfig.h"
#include "ManualmaticState.h"

#ifndef ManualmaticSend_h
#define ManualmaticSend_h

/**
 * @brief A helper class containing a bunch of methods to format & send specific messages
 * 
 */
class ManualmaticMessenger {

  public:

    ManualmaticMessenger(ManualmaticMessage& message, ManualmaticConfig& config, ManualmaticState& s);


    void sendHeartbeat();

    /** *************************************************************
     *  machine state
     */

    void setMachineState(Task_state_e state);

    void sendAbort();

    /** *************************************************************
     *  Jogging
     */

    /**
     * Jog the axis by distance
     * Note: velocity is already set
     */
    void jogAxis(uint8_t axis, float distance );

    /**
     * Jog the axis continuously by velocity
     * Pass zero to stop
     */
    void jogAxisContinuous(uint8_t axis, float velocity );

    /**
     * Jog the axis continuously by velocity
     * Pass zero to stop
     */
    void jogAxisStop(uint8_t axis );



    /**
     * Toggle between Tortoise and Rabbit jog speeds
     * 
     */
    void toggleJogRange();

    /**
     * Reset both Tortoise and Rabbit jog velocity to their defaults
     */
    void resetJogVelocity();

    /**
     * Increment the jogVelocity and send to serial
     * Serial will return the set velocity which will update the display
     */
    void incrementJogVelocity(int16_t incr);

    /** *****************************************
     * Zero or half axis
     */
    void sendG5xOffset(uint8_t axis, char offset[20] );

    /**
     * Directly control spindle RPM
     */
    void sendSpindleRpm();

    /**
     * Stop the spindle
     */
    void stopSpindle();

    /**
     * Start the spindle
     */
    void startSpindle();


    /**
     * Send the spindle override
     */
    void sendSpindleOverride(float pct=1);

    void incrementSpindleOverride(int16_t incr);

    void sendFeedrate(float pct=1);

    void sendRapidrate(float pct=1);

    /**
     * 
     */
    void incrementFeedrate(int16_t incr);

    /**
     * 
     */
    void incrementRapidrate(int16_t incr);

    void sendTaskMode(uint8_t mode);

    void sendInit();


    void sendFlood(Flood_e onOff);

    void sendMist(Mist_e onOff);

    void sendAuto(AutoCmd_e ac);



  private:
    ManualmaticMessage& serialMessage;
    ManualmaticConfig& config;
    ManualmaticState& state;

};




#endif //ManualmaticSend_h
