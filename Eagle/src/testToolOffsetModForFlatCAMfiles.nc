(Description:)
(Hand crafted job made to test using tool offsets stored in PathPilot)
(Will be used to hand edit FlatCAM jobs to include tool changes)
(Author: Andrew Mitchell)
(History:)
(2018-10-31: Job created)

N01(Setup job. G-Codes do the following...)
(Absolute distance mode)
(Coordinate system 1)
(Maintain defined feed velocity)
(Reset all scale factors to 1)
(XY-plane as active)
(Cutter compensation off)
(Cancel all canned cycles)
(Set active feed rate to units per minute)
(Incremental distance mode for I and K offsets, return I and K to their default behaviour)
(Cancel tool length compensation)
G90 G54 G64 G50 G17 G40 G80 G94 G91.1 G49
G20 (Inch)
G30 (Rapid traverse to saved absolute position)

N10(Retrieve engraving tool used for copper traces)
(Tool change code does the following...)
(Tool 21 – engraving bit)
(Apply tool length offset)
(Tool number associated with length of tool specified by number - same as T above)
(Tool change)
T21 G43 H21 M6 

N11(Check tool is at the right Z height)
G00 Z0.1000 (Move tool head to just above work surface)
M1 (Program stop to allow operator to check Z position)

N20(Retrieve 1.1mm which 0.0433" drill for components)
(Tool change code does the following...)
(Tool 22 – 1.1mm drill bit)
(Apply tool length offset)
(Tool number associated with length of tool specified by number - same as T above)
(Tool change)
T22 G43 H22 M6

N21(Check tool is at the right Z height)
G00 Z0.1000
M1 (Program stop to allow operator to check Z position)

N30(End job)
M5 (Stop spindle rotation)
M9 (All coolant off)
G30 (Rapid traverse to saved absolute position)
M30 (Program end and rewind)





