/***********************************************************************************************************
 amDisplayRunningConfiguration.cpp
 Created by Andrew Mitchell
 This function dumps a bunch of useful info to the terminal. This code is based on an example we found at 
 this URL: https://stackoverflow.com/questions/14143517/find-the-name-of-an-arduino-sketch-programmatically  
 History
 Version YYYY-MM-DD Description
 ------- ---------- ---------------------------------------------------------------------------------------
 1.0     2018-03-29 Code base created.                                 
 ***********************************************************************************************************/
#include "Arduino.h" //Give this library access to the standard Arduino types and constants
#include "amDisplayRunningConfiguration.h" //Function definitions

/////////////////////////////////////////////////////////////////////////////////////////////////////////////	
// Define constructor 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////	
amDisplayRunningConfiguration::amDisplayRunningConfiguration(char *sketch_version)
{
    strcpy(_sketch_version,sketch_version);

} //amDisplayRunningConfiguration::amDisplayRunningConfiguration()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////	
// Define display method 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////	
void amDisplayRunningConfiguration::display()
{                                 
  Serial.print("[amDisplayRunningConfiguration] Sketch Version: ");
  Serial.println(_sketch_version); //Get version of code being run (manually set in header section of main)
  Serial.print("[amDisplayRunningConfiguration] Sketch compilation date: ");
  Serial.print(__DATE__); //Get compilation date of code being run
  Serial.print(" at ");
  Serial.println(__TIME__); //Get compilation time of code being run
  Serial.print("[amDisplayRunningConfiguration] ESP32 SDK used: ");
  Serial.println(ESP.getSdkVersion()); //Get version of ESP-IDF used to build code
  Serial.print("[amDisplayRunningConfiguration] Chip revision: ");
  Serial.println(ESP.getChipRevision()); //Get version of ESP-IDF used to build code 
  Serial.print("[amDisplayRunningConfiguration] CPU Freq: ");
  Serial.println(ESP.getCpuFreqMHz());
  Serial.print("[amDisplayRunningConfiguration] Flash Size: ");
  Serial.println(ESP.getFlashChipSize());
  Serial.print("[amDisplayRunningConfiguration] Flash Speed: ");
  Serial.println(ESP.getFlashChipSpeed());
  Serial.print("[amDisplayRunningConfiguration] Setup is running on core ");
  Serial.print(xPortGetCoreID()); //Get core that Arduino setup() and loop() are running on
  Serial.print(" with a priority of ");
  Serial.println(uxTaskPriorityGet(NULL)); //Get task priority that Arduino setup() and loop() are running at
} //amDisplayRunningConfiguration::display()