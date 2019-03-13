( This program repeatedly probes in a regular XY grid and writes the ) 
( probed location's x, y and z data to the file 'probe_results.csv'. ) 
( Pathpilot has numbered parameters that can be used in Gcode scripts.)
( In this program we use the following: )
( x coordinate of from a G38.2 is stored in parameter #5061 )
( y coordinate of from a G38.2 stored in parameter #5062 )
( z coordinate of from a G38.2 stored in parameter #5063 )
( Full reference: https://www.tormach.com/numbered-parameters-reference/ )
( For logging commands see section 6 of this reference link: )
( http://www.vdwalle.com/Norte/G%20Code%20Reference.htm ) 
( Note: the default directory for LOG commands is //home/operator/gcode )
 
( Configuration section) 
G20 (Inches) 
F20 (probe speed)  
#1=0 (X start) 
#2=.04 (X increment) 
#3=51 (X count) 
#4=0 (y start)
#5=.04 (y increment)
#6=51 (Y count) 
#7=.5 (Z safety) 
#8=-.1 (Z probe) 
( End configuration section) 

( Main script section )
(LOGOPEN,./our_programs/Andrew/PCB_stuff/probingScripts/probe_results.csv) 
#9=0 #10=0 
G0Z#7 
O1 while [#9 lt #6] 
#10=0 
G0 Y[#4+#5*#9] 
O2 while [#10 lt #3] 
O3 if [[#9/2] - fix[#9/2] eq 0] 
#11=[#1+#2*#10] 
O3 else 
#11=[#1+#2*[#3-#10-1]] 
O3 endif 
(#11 is X target) 
#5070=1 
O4 while [#5070 NE 0] 
G38.5 z#7 
G38.3 x#11 
O4 endwhile 
G38.2Z#8 
(LOG,#5061,#5062,#5063) 
#10=[#10+1] 
O2 endwhile 
G0Z#7 
#9=[#9+1] 
O1 endwhile 
(LOGCLOSE) 
G0Z#7 
G0X#1Y#4 
( End Main script section )

( Ending section )
M30 (stop program and rewind too the beginning)