# Manualmatic Pendant Schematic

[This](manulmatic-schematic.pdf) is how I have wired the [Teensy 4.1](https://www.pjrc.com/store/teensy41.html), the [Adafruit 2.8" Touch screen](https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2) and the various encoders & buttons for the proof of concept. The code base is written to this schematic.

# PCBs and Components

There is now a PCB for this project which is somewhat easier than the molex birds nest shown below - the case is also designed around the board. BoM details are [here](../../docs/BOM.md) and information/pictures for soldering to the PCB is [here](../../docs/SOLDERING.md).

I have a few spare PCBs and small stock of the board mounted components (thanks to minimum order levels). Please contact me via email (in my bio) if you are interested.

# Proof of Concept (*not* recommended!)

The picture below shows the wiring of the proof-of-concept. While this worked and was fairly robust, it's not an approach I can recommend! It is also prior to adding the joystick.

**Out of Date***

Where possible I have grouped pins together, so if you have access to a Molex crimper the wiring is not too onerous. I strongly suggest making short 'ribbon cables' to keep the birds nest tidy(ish).

![pendant wiring](prototype-wiring.jpeg)

The 'bridge' for the configurable button row holds the Teeensy in place at the centre but probably better to push all the Molexes onto the Teensy pins before attaching the display.

The wiring check has now moved to 'Software': [ManualmaticWiringCheck](https://github.com/Stutchbury/Manualmatic-Pendant/tree/main/Software/PlatformIO/pendant/ManualmaticWiringCheck)  

This is a PlatformIO 'sketch' that will enable you to check everything is wired as expected, including the display and touchscreen.

The schematic is [here](manulmatic-schematic.pdf).