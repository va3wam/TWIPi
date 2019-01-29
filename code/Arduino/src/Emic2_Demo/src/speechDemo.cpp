#include <Arduino.h>
#include <amDisplayRunningConfiguration.h> //Library class that dislays useful info about this sketch
/***********************************************************************************************************
  Synopsis:
  This sketch tests the Parallax EMIC2 text to speech module

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
  - Parallax EMIC2 text to speech module
    Serial bus communication
    5VDC power required
    Voice options selected with Nx command where x is one of these values
    0: Perfect Paul (Paulo)
    1: Huge Harry (Francisco)
    2: Beautiful Betty
    3: Uppity Ursula
    4: Doctor Dennis (Enrique)
    5: Kit the Kid
    6: Frail Frank
    7: Rough Rita
    8: Whispering Wendy (Beatriz)

  History
*/   
  char my_ver[] = "1.0.1"; // Semantic Versioning (https://semver.org/)
/*
  Version YYYY-MM-DD Description
  ------- ---------- ---------------------------------------------------------------------------------------
  1.0.1   2018-12-01 Added voice and volume settings
  1.0.0   2018-11-29 Code base created.
 ***********************************************************************************************************/
amDisplayRunningConfiguration aboutThisSketch(my_ver); //Define a function that displays info about this sketch
#define ledPin 13 //On-board LED on this pin
#if defined ( ESP32 ) //If this is an ESP32 platform
  HardwareSerial  EMIC2Serial( 2 ); //Define reference to second serail port like this
#else
   #define  EMIC2Serial Serial1; //Otherwise define reference to second serial port like this 
#endif

/***********************************************************************************************************
 This function initializes the console where trace information is sent during sketch execution.
 ***********************************************************************************************************/
void initConsole()
{
  Serial.begin(115200); //Console output. NOTE: Be sure to put monitor_speed = 115200 in platformio.ini
  delay(5); //Give serial time to initialize before using it
  Serial.println("[initConsole] **************************************************************************"); 
  Serial.print("[initConsole] Start of sketch. Source code line: ");
  Serial.println(__LINE__); //Get currrent line of code being executed
} //initConsole()

/***********************************************************************************************************
 This function dumps a bunch of useful info to the terminal. NOte that an effort has been made to put
 most of the printing of key data into the class amDisplayRunningConfiguration() but some data must be
 generated inside this sketch to get the correct information abot this sketch and not the class file. This 
 is why the file name and line number informaiton must be called from this routine and not the class.
 ***********************************************************************************************************/
void display_Running_Sketch()
{
  Serial.print("[display_Running_Sketch] Sketch Name: ");
  Serial.println(__FILE__); //Get file name of sketch being run
  aboutThisSketch.display(); // Show additional information about this sketch in the console
} //display_Running_Sketch()

/***********************************************************************************************************
 This function initializes the EMIC2 text to speech module. When the Emic 2 powers on, it takes about 3 
 seconds for it to successfully initialize. It then sends a ":" character to indicate it's ready to accept
 commands. If the Emic 2 is already initialized, a CR will also cause it to send a ":"
 ***********************************************************************************************************/
void initEMIC2()
{
  Serial.println("[initEMIC2] Initialize the EMIC2 text to speech module");
  EMIC2Serial.begin(9600); //Parallax EMC2 text to speech serail interface
  EMIC2Serial.print('\n'); //Send a CR in case the system is already up
  while (EMIC2Serial.read() != ':'); // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10); // Short delay
  Serial.println("[initEMIC2] Tell EMIC2 which voice we want to use");
  EMIC2Serial.print('N2'); //Voice options are Nx where x = 0 to 8
  EMIC2Serial.print('\n'); //Indicate that text string is at an end
  while (EMIC2Serial.read() != ':'); // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10); // Short delay
  Serial.println("[initEMIC2] Set max volume");
  EMIC2Serial.print('V18'); //Set audio volume (dB): Vx where x = -48 to 18
  EMIC2Serial.print('\n'); //Indicate that text string is at an end
  while (EMIC2Serial.read() != ':'); // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10); // Short delay
/* Need to figure out how to get this info to display on the console
  Serial.println("[setup] Ask EMIC2 for version information");
  EMIC2Serial.print('C'); //Voice options are Nx where x = 0 to 8
  EMIC2Serial.print('\n'); //Indicate that text string is at an end
  while (EMIC2Serial.read() != ':'); // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10); // Short delay
  Serial.println("[setup] Ask EMIC2 for its text to speech settings");
  EMIC2Serial.print('I'); //Voice options are Nx where x = 0 to 8
  EMIC2Serial.print('\n'); //Indicate that text string is at an end
*/
  //EMIC2Serial.flush(); // Flush the receive buffer
} //initEMIC2()

/***********************************************************************************************************
 Standard Arduino setup function that runs at the start of the sketch. Runs once.
 ***********************************************************************************************************/
void setup() 
{
  pinMode(ledPin, OUTPUT); //Ready onbaord LED for output
  digitalWrite(ledPin, LOW); //Turn onboard LED off
  initConsole(); //Set up the console for trace messages
  display_Running_Sketch(); //Dump useful sketch information to the console
  initEMIC2(); //Set up the EMIC2 text to speech module
} //setup()

/***********************************************************************************************************
 Standard Arduino main loop. Runs as a continuous loop.
 ***********************************************************************************************************/
void loop() 
{
  Serial.println("[loop] Tell EMIC2 that we want to talk");
  EMIC2Serial.print('S'); //Tell EMIC 2 that I want to talk
  Serial.println("[loop] Send EMIC2 text we want spoken");
  // Send the desired string to convert to speech
  EMIC2Serial.print("Hello. My name is twip-pea. Can you understand what I am saying?");  
  EMIC2Serial.print('\n'); //Indicate that text string is at an end
  digitalWrite(ledPin, HIGH); //Turn on LED while Emic is outputting audio
  Serial.println("[loop] Wait for EMIC2 to indicate that it is done speaking");
  while (EMIC2Serial.read() != ':'); //Wait here until the Emic 2 responds with a ":" indicating it's ready to 
                                 //accept the next command
  digitalWrite(ledPin, LOW); //Turn onboard LED off indicating that speaking is done
  delay(500);    // 1/2 second delay  
  // Sing a song
  /*
  Serial.println("[loop] Tell EMIC2 to sing canned song #1");
  EMIC2Serial.print("D1\n"); //EMAC2 has pre-recorded songs that you can access
  digitalWrite(ledPin, HIGH); //Turn on LED while Emic is outputting audio
  Serial.println("[loop] Wait for EMIC2 to indicate that it is done singing");
  while (EMIC2Serial.read() != ':'); //Wait here until the Emic 2 responds with a ":" indicating it's ready to 
                                 //accept the next command
  digitalWrite(ledPin, LOW); //Turn onboard LED off indicating that speaking is done
  Serial.println("[loop] Show is over. Go into an endless loop.");
  */
  while(1)      // Demonstration complete!
  {
    delay(500);
    digitalWrite(ledPin, HIGH);
    delay(500);              
    digitalWrite(ledPin, LOW);
  }
} //loop()