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

