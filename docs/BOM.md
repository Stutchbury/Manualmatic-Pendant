# Bill of Materials

These are the components I have chosen to use for the Manualmatic when using the PCB.



|Num| Item | Ref	| Qty	| Value | Datasheet/Notes |  Manufacturer |
|--| ---- | ---- | --- | ----- | --------- | ------------- |
|1|![Teensy 4.1](../images/teensy4.1.jpg)  | U1 |	1	| Teensy4.1| https://www.pjrc.com/store/teensy41.html | Teensy |
|2|![Adafruit Display](../images/adafruit_2.8-128.jpg) | J1 |	1	| Adafruit-1770-2.8-TFT-Touch	 |	https://www.adafruit.com/product/1770	| Adafruit |
|3|![Encoder button](../images/PEC11R-4115F-S0018.png)  | S12 S13 | 2	| PEC11R-4115F-S0018	|	https://www.mouser.co.uk/datasheet/2/54/PEC11R-777457.pdf	| Bourns |
|4|![push button](../images/B3F-1025.png) | S1 S2 S3 S4 S5 |	5	| B3F-1025 |	https://www.mouser.co.uk/datasheet/2/307/en-b3f-13826.pdf	| Omron |
|5|![right angle push button](../images/B3F-3105.png) | S6 S7 S8 S9 S10 S11 S14 | 7	| B3F-3105| https://www.mouser.co.uk/datasheet/2/307/en-b3f-13826.pdf |		Omron |
|6|![Joystick](../images/COM-09032.jpg)  | U2 |	1	|COM-09032	| https://www.sparkfun.com/products/9032	| SparkFun Electronics |
|7|![MPG right angle header](../images/right-angle-header.png) | J2 | 1 | 4 pin right angle male header| For MPG | Generic|
|8|![MPG female plug](../images/female-plug-mpg.jpg) | n/a  | 1 | 4 pin female plug| For MPG | Generic|
|9|![Estop right angle header](../images/right-angle-header-estop.jpg) | J3 | 1 | 2 pin right angle male header | For Estop button | Generic|
|10|![Estop female plug](../images/female-plug-estop.jpg) | n/a  | 1 | 2 pin female plug| For Estop button | Generic|
|11|![Teensy female headers](../images/female-headers-teensy.jpg) | U1 | 4 x 12pin | Female headers | To plug Teensy into PCB. <br/>8.5mm high, excluding pins | Generic|
|12|![Display female headers](../images/female-headers-display.jpg) | J1 | 2 x 7pin| Female headers | To plug in display into PCB. <br/> 8.5mm high, excluding pins | Generic|
|13|![MPG](../images/mpg.jpg) | n/a | 1 | 60mm Manual Pulse Generator | ebay ones seem to work just fine, unlike the encoder buttons. Some may have 6 connections but you can ignore A- & B-.| Generic|
|14|![Estop](../images/16mmEstop.jpg) | n/a | 1 | 16mm Estop button | Example: https://www.ebay.co.uk/itm/184899597984 | Generic|
|15|![USB Cable](../images/right-angle-micro-usb.jpg) | n/a | 1| 2-3m tight/compact right angle micro-USB cable | Example: https://www.ebay.co.uk/itm/194756552182?var=494734226747 | Generic|
|16|![PCB](../images/manualmatic-pcbs.jpeg) | n/a | 1 | Custom PCB| Contact me (email address on my profile)  | Stutchbury|


Unless you're brave enough to solder both the Teensy and the Adafruit display to the PCB (I'm not and the case *is not* designed for direct soldering), you'll need 48 female headers for the Teensy (item 9) and 14 female headers for the display (item 10).

I bought these as 4 x 12 pins and 2 x 7 pins as this was significantly cheaper than contiguous blocks. You may need to lightly file adjacent edges so they don't bow the board when soldered in.

Items 7-10 above can be omitted if you solder the MPG & Estop wires directly to the PCB.


J4 & J5 (3 & 4 pin spare breakouts) are optional right angle male headers.


**Notes:**

1. I recommend sourcing the two encoder buttons from a known supplier unless you are sure of their provenance - I have had two 'ebay specials' fail which have effectively wasted the PCBs. The specified encoders also have a short shaft to work with the case.
2. The 5 push buttons and 7 right angle buttons are of a specific detent force (orange button) and closely match the detent force of the two encoders and the joystick, maximising the consistency of 'feel'. Ebay ones are of very variable detent strength - often very low.
3. The joystick PCB footprint will only fit the Sparkfun Deluxe COM-09032 joystick. The Xbox replacements have a different footprint (and the ebay ones have 'interesting' resistance characteristics).
4. If you are using the 3D printed case (recomended, it's a work of art ;-)), you'll need 4 x 11mm brass stand-offs and 8 x 8-10mm M3 dome/button head screws.
5. We are using the Adafruit display in SPI mode, so you will need to bridge the pads on the back of the board as per: https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/spi-wiring-and-test#spi-mode-jumpers-906160-5