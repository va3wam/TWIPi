SB7 is a double sided board with all parts of previous SB6 abd PL2 boards except power regulators.
Pushbuttons and external USB Jack are on "solder" side of main board.
In more detail, when you can read the layout version number 7B in the top left corner:
-most components, including the CPU, IMU, motor controllers are on the side of the board facing you, the "front"
-the 4 pushbuttons and USB jack on the left side of the board are on the back, away from you
 (the board is rotated when mounted in bot, so the row of pushbuttons is on the top)
-the red traces are on the back of the board, and connect the components on the front of the board
-the blue traces are on the front of the board, and connect components on the back of the board (pushbuttons & USB jack)
-connections that use VIAs have sections on both sides of the board

There are about 19 simulated VIAs which should be changed to real VIAs if board manufacture is outsourced


7A version layout
-pushbuttons are slightly off centre
-there's a ground test point, but no 3.3, 5, or 12V test points
-board is slightly bigger - 3.90 by 3.95 inches
-mounting holes have been moved - do they have enough clearance?
-the 2 power connectors are faked with 0.2 inch contacts. should get real part references
- battery connection is a 2 conductor cable near back of 4th pushbutton
- regulator connections (in and out) are a 5 conductor cable near back of 1st pushbutton
-there are no power protection diodes. It's assumed that power connectors are keyed
-resistor R4 is used as a VIA and needs to be soldered on both sides
-battery ground is intentionally not connected to regulator output ground
-the grounds for 12 and 5V regulator output are bridged at the regulators and have a common wire
 to the main board called RO-12VG ("regulator output, 12V regulator, ground")

7B version layout
-push buttons centred better
-see if there is an available short USB console cable, and change header to match it.

7C version layout
-all vias have been converted to true vias, not manual ones
-resistor R4 no longer functions as a via, & normal soldering is OK
-power connectors remain unresolved. 
    - may need to move capacitor to allow right angled connectors. 
	- may be too tight for them anyway, unless they face outwards?
-crossed wires for stepper driver STEPDRV2 on board, so all stepper wiring is straight thru


References on Wire Gauge / Current capacity

https://www.powerstream.com/Wire_Size.htm    (24AWG for 3.5 amps, stranded?)
https://www.jst.fr/doc/jst/pdf/current_rating.pdf   (22 AWG for 3.0A)
https://www.multicable.com/resources/reference-data/current-carrying-capacity-of-copper-conductors/
https://www.solaris-shop.com/content/American%20Wire%20Gauge%20Conductor%20Size%20Table.pdf
https://xtronics.com/wiki/Wire-Gauge_Ampacity.html
https://www.gore.com/IndustrialCableConfigurator/popup_hfr_wirespecs.html
http://www.learningaboutelectronics.com/Articles/Wire-gauge.php
https://www.coonerwire.com/amp-chart/
https://en.wikipedia.org/wiki/American_wire_gauge

Approach: Solder wires to board with pre-installed connectors at ends
https://www.bc-robotics.com/shop/2-wire-inline-connector/
https://www.bc-robotics.com/shop/2-pin-jst-sm-plug-receptacle-cable-set/
https://www.creatroninc.com/product/3-pin-js-m-wire-assembly/
https://www.creatroninc.com/product/3-pin-js-f-wire-assembly/
https://www.robotshop.com/en/sfe-130mm-wire-jst-blk-red.html
https://www.robotshop.com/en/sfe-100mm-wire-jst-blk-blue.html

https://canada.newark.com/w/c/connectors/wire-to-board-connectors/prl/results?st=jst%20cables
https://canada.newark.com/w/c/cable-wire-cable-assemblies/cable-assemblies/prl/results?st=jst%20cables

https://www.amazon.ca/URBEST-10Pairs-Female-Connector-Battery/dp/B01M13PMIU
https://solarbotics.com/product/14226



 
