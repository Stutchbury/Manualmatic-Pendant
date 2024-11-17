/**
 * @file ManualmaticControl.h
 * @author Philip Fletcher <philip.fletcher@stutchbury.com>
 * @brief The Manualmatic control class
 * @version 0.1
 * @date 2022-03-01
 * 
 * @copyright Copyright (c) 2022
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 */


#ifndef ManualmaticControl_h
#define ManualmaticControl_h

#include <EncoderButton.h>
#include <EventButton.h>
#include <EventJoystick.h>
#include <Bounce2.h>
#include "ManualmaticWiring.h"
#include "ManualmaticConsts.h"
#include "ManualmaticMessage.h"
#include "ManualmaticMessenger.h"
#include "ManualmaticConfig.h"
#include "ManualmaticState.h"
#include "ManualmaticButtonRowKeypad.h"
#include "ManualmaticOffsetKeypad.h"
#include "ManualmaticUtils.h"

/**
 * @brief The Manualmatic control class
 * 
 */
class ManualmaticControl {

  public:

    /**
     * @brief Construct a new Manualmatic Control object
     * 
     * @param serialMessage A Stream object - usually Serial
     * @param config ManualmaticConfig
     * @param s ManualmaticState
     * @param brkp ManualmaticButtonRowKeypad
     * @param okp ManualmaticOffsetKeypad
     */
    ManualmaticControl(ManualmaticMessage& serialMessage, 
                      ManualmaticConfig& config, 
                      ManualmaticState& s, 
                      ManualmaticButtonRowKeypad& brkp,
                      ManualmaticOffsetKeypad& okp
                      );


    /**
     * @brief Initialise anything that is not done in the constructor
     * 
     */
    void begin();

    /**
     * @brief Called once per loop()
     * 
     */
    void update();

  private:

    ManualmaticMessage& message;
    ManualmaticConfig& config;
    ManualmaticState& state;
    ManualmaticButtonRowKeypad& brkp;
    ManualmaticOffsetKeypad& okp;
    ManualmaticMessenger messenger;

    //Encoders
    EncoderButton feed;
    EncoderButton mpg;
    EncoderButton spindle;

    EventButton buttonOnOff;
    EventButton buttonX;
    EventButton buttonY;
    EventButton buttonZ;
    EventButton buttonA;
    EventButton buttonMode;
    EventButton buttonModifier;
    EventButton rowButtons[5];

    Bounce estopSwitch = Bounce();

    EventJoystick joystick;
    EventButton buttonJoystick;


    ButtonRow_e buttonRow = BUTTON_ROW_NONE;

    void setupEncoders();
    void setupButtons();
    void setupButtonRowKeypad();
    void setupOffsetKeypad();
    void setupJoystick();

    void checkEstop(bool force=false);
    void checkHeartbeat();
    void onIniReceived();

    void onFeedEncoder(EncoderButton& rb);
    void onFeedPressedEncoder(EncoderButton& rb);
    void onFeedClicked(EncoderButton& rb);
    void onFeedLongPress(EncoderButton& rb);

    void onSpindleEncoder(EncoderButton& rb);
    void onSpindleClicked(EncoderButton& rb);
    void onSpindleTripleClicked(EncoderButton& rb);
    void onSpindleLongPressed(EncoderButton& rb);

    void onMpgEncoder(EncoderButton& rb);
    
    void onJoystickXChanged(EventAnalog& ea);
    void onJoystickYChanged(EventAnalog& ea);
    void onJoystickIdle(EventJoystick& ejs);
    void onJoystickClicked(EventButton& ejs);    
    void onJoystickDoubleClicked(EventButton& ejs);    


      
    /**
     * Update the state of physical button on the button row.
     */
    void updateButtonRow();

 
    void onOffClicked(EventButton btn);
    void onOnOffLongPress(EventButton& btn);

    void setupButtonRow(ButtonRow_e);


    /**
     * Switch or toggle the axis selected.
     * We're not just toggling but also issuing a JOG_STOP if deselecting
     */
    void toggleSelectedAxis(Axis_e axis);

    /**
     * We're not just toggling the axis, but issuing a JOG_STOP if deselecting
     */
    void toggleXSelected(EventButton& btn);
    void toggleYSelected(EventButton& btn);
    void toggleZSelected(EventButton& btn);
    void toggleASelected(EventButton& btn);
  
    void toggleDisplayAbsG5x(EventButton& btn);
    void displayDtg(EventButton& btn);
    void onButtonALongPressed(EventButton& rb);
    void toggleDisplayAAxis(EventButton& btn);

    void onButtonModeClicked(EventButton& btn);
    /**
     * All the physical buttons on the button row call this 
     * click handler.
     */
    void onButtonRowButtonClicked(EventButton& btn);
    /**
     * One of the five buttons on the button row has been clicked
     * Call the function that matches the set button type and
     * it will decide what to do based on state and the 
     * ButtonRow_e context.
     * Also called by ButtonRowKeypad touched callback.
     */
    void onButtonRowClicked(ButtonType_e buttonType);
    /**
     * One of the five buttons on the button row has been double clicked
     * Call the function that matches the set button type and
     * it will decide what to do based on state and the 
     * ButtonRow_e context.
     */
    void onButtonRowDoubleClicked(EventButton& btn);

    void onTouchCancelG5xOffset(TouchKey& tkcb);

    void onTouchSetG5xOffset(TouchKey& tkcb);

    void onButtonModifierPressed(EventButton& rb);
    void onButtonModifierReleased(EventButton& rb);


    /**
     * All these setButton* functions should probably reside in ButtonRowKeypad class
     */
    void unsetButtonRow();
    void setRowButtonType(uint8_t col, ButtonType_e bt);
    void setButtonRowManual(ButtonRow_e b);
    void setButtonRowAuto(ButtonRow_e b);
    void setButtonRowMdi(ButtonRow_e b);
    void setButtonRowCancelOrStop(ButtonRow_e b);
    void setButtonRowCancelOrTick(ButtonRow_e b);
    void setButtonRowCancelOrTickG5x(ButtonRow_e b);
    void setButtonRowCancelOrPlay(ButtonRow_e b);

    void onButtonRowTouched(TouchKey& key);
    void onButtonPlay();
    void onButtonPause();
    void onButtonOneStep();
    void onButtonHalt();
    void onButtonCancel();
    void onButtonTick();
    void onButtonStop();
    void toggleCoolant(bool doubleClick=false);
    void onButtonTouchOff();

    void onCancelG5xOffset();
    void onSetG5xOffset();

};


#endif //ManualmaticControl_h
