#include <Arduino.h>
#include <amDisplayRunningConfiguration.h> // Library class that dislays useful info about this sketch
#include <Adafruit_SSD1306.h> // https://github.com/adafruit/Adafruit_SSD1306
#include <Adafruit_GFX.h> // https://github.com/adafruit/Adafruit-GFX-Library
#include <Wire.h> // For I2C buses
#include <normalEyes.h> // Old COZMO inspired eyes
#include <otherEyes.h> // Old COZMO inspired eyes
/***********************************************************************************************************
  Synopsis:
  This sketch tests the Adafruit OLED 0.96inch 128x64 Pixel device. The idea of having animated OLED eyes
  came from seeing the Cozmo robot and then seeing this project: 
  https://www.instructables.com/id/DIY-Cozmo-Robot/
  
  NOTE!!! /Users/andrewmitchell/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-i2c.c
  has custom lines in it that differ from the ESP-IDF stable release. Make sure that these lines are 
  maintained when updating to the latest PlatformIO ESP-IDF files. Failure to do so will result in unrelaible
  I2C and hardware timer behavior. See the clsojng comments for issue https://github.com/va3wam/TWIPi/issues/14
  for full details.

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
  - Adafruit OLED
    Dimensions: 0.96inch
    Resolution: 128×64 Pixels
    I2C address: 0x7A or 0x7B depending on resistor placement
    Arduino Library: Adafruit_SSD1306.cpp (graphics) and Adafruit_SSD1306.h (communication)
    Operating voltage: 3-5V

  History
*/   
  char my_ver[] = "1.0.1"; // Semantic Versioning (https://semver.org/)
/*
  Version YYYY-MM-DD Description
  ------- ---------- ---------------------------------------------------------------------------------------
  1.0.1   2018-11-30 Added second OLED, renamed variables to make things more reader friendly, and moved the
                     eye character array definitions out of this file and into to include files.
  1.0.0   2018-11-29 Code base created.
 ***********************************************************************************************************/
amDisplayRunningConfiguration aboutThisSketch(my_ver); // Define a function that displays info about this 
                                                       // sketch

/***********************************************************************************************************
 Define first I2C bus and associated constants and global variables. SDA1 is on GPIO 23 (physical pin 17) 
 and SCL1 is on GPIO pin 22 (physical pin 18). 
 ***********************************************************************************************************/
#define SDA1 23 // Define SDA I2C pin for first I2C bus (wire)
#define SCL1 22 // Define SCL I2C pin for first I2C bus (wire)

/***********************************************************************************************************
 Define OLED constants and global variables. 
 ***********************************************************************************************************/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 rightOLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Define right OLED object
Adafruit_SSD1306 leftOLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Define left OLED object
#define LOGO_HEIGHT   64
#define LOGO_WIDTH    88
int rightOLED_I2C_Address = 0x3C; // I2C address of right OLED display (robot perspective)
int leftOLED_I2C_Address = 0x3D; // I2C address of left OLED display (robot perspective)

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
 This function starts and scans the I2Cone buse for attached devices. This code was taken from 
 http://playground.arduino.cc/Main/I2cScanner then updated with this 
 https://github.com/espressif/arduino-esp32/issues/977 in order to support 2 buses. SCL and SDA on both 
 buses should have a 2.2K or 2.4K pull up resitor (3.3VDC). Here is a list of all the I2C addresses that
 Adafruit devices use: https://learn.adafruit.com/i2c-addresses/the-list.
 ***********************************************************************************************************/
void startI2Cone()
{   
    Serial.print("[startI2Cone] Initialize I2C bus. Source code line: ");
    Serial.println(__LINE__);
    Wire.begin(SDA1,SCL1,400000); // 400KHz, uppder speed limit for ESP32 I2C
    uint8_t cnt=0;
    for(uint8_t i=0;i<128;i++)
    {
        Wire.beginTransmission(i);
        uint8_t ec=Wire.endTransmission(true);
        if(ec==0)
        {
            if(i<16)Serial.print('0');
            Serial.print(i,HEX);
            cnt++; 
        } //if
        else Serial.print("..");
        Serial.print(' ');
        if ((i&0x0f)==0x0f)Serial.println();
    } //for

    Serial.print("[startI2Cone] Scan Completed, ");
    Serial.print(cnt);
    Serial.println(" I2C Devices found.");
} // startI2Cone()

/***********************************************************************************************************
 This function initializes the OLED display
 ***********************************************************************************************************/
void initOLEDs()
{
  Serial.print("[initOLED] Initialize OLED display");
  if(!rightOLED.begin(SSD1306_SWITCHCAPVCC, rightOLED_I2C_Address)) //Initialize OLED using its I2C address
  { 
    Serial.println(F("[initOLED] SSD1306 allocation of right OLED failed"));
    for(;;); // Don't proceed, loop forever
  } //if
  if(!leftOLED.begin(SSD1306_SWITCHCAPVCC, leftOLED_I2C_Address)) //Initialize OLED using its I2C address
  { 
    Serial.println(F("[initOLED] SSD1306 allocation of left OLED failed"));
    for(;;); // Don't proceed, loop forever
  } //if
} //display_Running_Sketch()

/***********************************************************************************************************
 This function draws the normal eyes bitmap to the OLED display.
 ***********************************************************************************************************/
void drawNormalEyes(void) 
{
  Serial.println("[drawNormalEyes] Display normal eyes bitmap ");
  rightOLED.clearDisplay();
  rightOLED.drawBitmap(
    (rightOLED.width()  - LOGO_WIDTH ) / 2,
    (rightOLED.height() - LOGO_HEIGHT) / 2,
    normal_eyes, LOGO_WIDTH, LOGO_HEIGHT, 1);
  rightOLED.display();
  leftOLED.clearDisplay();
  leftOLED.drawBitmap(
    (leftOLED.width()  - LOGO_WIDTH ) / 2,
    (leftOLED.height() - LOGO_HEIGHT) / 2,
    normal_eyes, LOGO_WIDTH, LOGO_HEIGHT, 1);
  leftOLED.display();
  delay(1000);
} //drawNormalEyes() 

/***********************************************************************************************************
 This function draws the normal eyes bitmap to the OLED display.
 ***********************************************************************************************************/
void drawOtherEyes(void) 
{
  Serial.println("[drawOtherEyes] Display other eyes bitmap ");
  rightOLED.clearDisplay();
  rightOLED.drawBitmap(
    (rightOLED.width()  - LOGO_WIDTH ) / 2,
    (rightOLED.height() - LOGO_HEIGHT) / 2,
    other_eyes, LOGO_WIDTH, LOGO_HEIGHT, 1);
  rightOLED.display();
  leftOLED.clearDisplay();
  leftOLED.drawBitmap(
    (leftOLED.width()  - LOGO_WIDTH ) / 2,
    (leftOLED.height() - LOGO_HEIGHT) / 2,
    other_eyes, LOGO_WIDTH, LOGO_HEIGHT, 1);
  leftOLED.display();
  delay(1000);
} //drawOtherEyes() 

/***********************************************************************************************************
 This function draws hello world accross both OLED displays.
 ***********************************************************************************************************/
void writeText()
{
  rightOLED.clearDisplay(); 
  rightOLED.setTextColor(WHITE); 
  rightOLED.setCursor(35,30); 
  rightOLED.setTextSize(1); 
  rightOLED.println("Hello"); 
  rightOLED.display();
  
  leftOLED.clearDisplay(); 
  leftOLED.setTextColor(WHITE); 
  leftOLED.setCursor(35,30); 
  leftOLED.setTextSize(1); 
  leftOLED.println("World"); 
  leftOLED.display();
} // writeText()

/***********************************************************************************************************
 This function draws text of different sizes on both OLED displays.
 ***********************************************************************************************************/
void sizeText()
{
  rightOLED.clearDisplay(); 
  rightOLED.setTextColor(WHITE); 
  rightOLED.setCursor(0,0); 
  rightOLED.setTextSize(1); 
  rightOLED.print("A"); 
  rightOLED.setTextSize(2); 
  rightOLED.print("A"); 
  rightOLED.setTextSize(3); 
  rightOLED.print("A"); 
  rightOLED.setTextSize(4); 
  rightOLED.print("A"); 
  rightOLED.setTextSize(5); 
  rightOLED.print("A"); 
  rightOLED.setTextSize(6); 
  rightOLED.print("A"); 
  rightOLED.display();

  leftOLED.clearDisplay(); 
  leftOLED.setTextColor(WHITE); 
  leftOLED.setCursor(0,0); 
  leftOLED.setTextSize(1); 
  leftOLED.print("A"); 
  leftOLED.setTextSize(2); 
  leftOLED.print("A"); 
  leftOLED.setTextSize(3); 
  leftOLED.print("A"); 
  leftOLED.setTextSize(4); 
  leftOLED.print("A"); 
  leftOLED.setTextSize(5); 
  leftOLED.print("A"); 
  leftOLED.setTextSize(6); 
  leftOLED.print("A"); 
  leftOLED.display();
} //sizeText()

/***********************************************************************************************************
 Standard Arduino setup function that runs at the start of the sketch. Runs once.
 ***********************************************************************************************************/
void setup() 
{
  initConsole(); //Set up the console for trace messages
  display_Running_Sketch(); //Dump useful sketch information to the console
  startI2Cone(); // Start the first I2C bus, which the servo controller is connected to
  initOLEDs(); // Set up the OLED display
} //setup()

/***********************************************************************************************************
 Standard Arduino main loop. Runs as a continuous loop.
 ***********************************************************************************************************/
void loop() 
{
  writeText();
  delay(2000);
  sizeText();
  delay(2000);
  drawNormalEyes();    // Draw a small bitmap image
  delay(2000);
  drawOtherEyes();
  delay(2000);
} //loop()