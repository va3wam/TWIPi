/***********************************************************************************************************
 amDisplayRunningConfiguration.h
 Created by Andrew Mitchell
 This function dumps a bunch of useful info to the terminal. This code is based on an example we found at 
 this URL: https://stackoverflow.com/questions/14143517/find-the-name-of-an-arduino-sketch-programmatically  
 History
 Version YYYY-MM-DD Description
 ------- ---------- ---------------------------------------------------------------------------------------
 1.0     2018-03-29 Code base created.                                 
 ***********************************************************************************************************/
#ifndef amDisplayRunningConfiguration_h //Include Guard to prevent duplicate definition of this library
#define amDisplayRunningConfiguration_h //Define as header file
#include "Arduino.h" //Give this library access to the standard Arduino types and constants

class amDisplayRunningConfiguration
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////	
    // Define constructor and public properties and methods 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////	
    public:
        amDisplayRunningConfiguration(char *sketch_version); 
        void display();   

	/////////////////////////////////////////////////////////////////////////////////////////////////////////	
    // When dealing with private variables, it is common convention to place an underscore before the 
    // variable name to let a user know the variable is private.		
	/////////////////////////////////////////////////////////////////////////////////////////////////////////	
    private:
        char _sketch_version[];

}; //class amDisplayRunningConfiguration

#endif //End wrapping of the #ifndef Include Guard