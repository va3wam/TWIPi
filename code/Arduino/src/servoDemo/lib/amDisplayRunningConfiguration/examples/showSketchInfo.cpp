#include <Arduino.h>
#include <amDisplayRunningConfiguration.h> //Library class that dislays useful info about this sketch
/***********************************************************************************************************
  Synopsis:
  This sketch shows how to use both include files and class library files to display useful information 
  about the sketch that is running.
  
  Environment:
  - Visual Source Code with PlatformIO plugin
  - Platform: Espressif 32 (https://www.espressif.com/en/products/hardware/esp32/overview)
  - Framework: Arduino 
  - Embedded board: Adafruit HUZZAH32 Feather (https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts)
  - SOC: WROOM32 module (https://dl.espressif.com/doc/esp-idf/latest/hw-reference/modules-and-boards.html)
  - IDE: Visual Studio Code v1.27.2 (https://code.visualstudio.com/)
  - VSC plugin: platformIO v3.6.1a4 (https://platformio.org/platformio-ide)
  - RTOS: FreeRTOS (https://www.freertos.org/Documentation/RTOS_book.html)
  - Language: C++ (Arduino)

  Hardware:
  - None required

  History
*/   
  char my_ver[] = "1.0.0"; // Semantic Versioning (https://semver.org/)
/*
  Version YYYY-MM-DD Description
  ------- ---------- ---------------------------------------------------------------------------------------
  1.0.0   2018-03-29 Code base created.
 ***********************************************************************************************************/
amDisplayRunningConfiguration aboutThisSketch(my_ver); //Define a function that displays info about this sketch

/***********************************************************************************************************
 This function initializes the console where trace information is sent during sketch execution.
 ***********************************************************************************************************/
void initConsole()
{
  Serial.begin(115200); //NOTE: Be sure to put monitor_speed = 115200 in platformio.ini
  delay(5); //Give serial time to initialize before using it
  Serial.println("[initConsole] **************************************************************************"); 
} //initConsole()

/***********************************************************************************************************
 This function dumps a bunch of useful info to the terminal. NOte that an effort has been made to put
 most of the printing of key data into the class amDisplayRunningConfiguration() but some data must be
 generated inside this sketch to get the correct information abot this sketch and not the class file. This 
 is why the file name and line number informaiton must be called from this routine and not the class.
 ***********************************************************************************************************/
void display_Running_Sketch()
{
  Serial.print("[display_Running_Sketch] Start of sketch. Source code line: ");
  Serial.println(__LINE__); //Get currrent line of code being executed
  Serial.print("[display_Running_Sketch] Sketch Name: ");
  Serial.println(__FILE__); //Get file name of sketch being run
  aboutThisSketch.display(); // Show additional information about this sketch in the console
} //display_Running_Sketch()

/***********************************************************************************************************
 Standard Arduino setup function that runs at the start of the sketch. Runs once.
 ***********************************************************************************************************/
void setup() 
{
  initConsole();
  display_Running_Sketch(); //Dump useful sketch information to the console

} //setup()

/***********************************************************************************************************
 Standard Arduino main loop. Runs as a continuous loop.
 ***********************************************************************************************************/
void loop() 
{
  
} //loop()