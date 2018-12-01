#include <Arduino.h> //Standard Arduino libraries
#include <amDisplayRunningConfiguration.h> //Class that dislays useful info about this sketch
#include <Wire.h> //For I2C buses
#include <Adafruit_PWMServoDriver.h>
/***********************************************************************************************************
  Synopsis:
  This sketch tests the Adafruit PCA9685 16 channel Servo driver. Details n circuit set up are located
  here: https://learn.adafruit.com/16-channel-pwm-servo-driver/hooking-it-up.
  
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
  - Adafruit PCA9685 16 channel servo driver
    I2C address: 0x40 for board 0, bridging jumpers adds 1 to the address. Can support many of these at once
    Arduino Library: Adafruit_SSD1306.cpp (graphics) and Adafruit_SSD1306.h (communication)
    Operating voltage: 3-5VDC for the board and 5VDC for the motors
  - SG90 servo motors
    Voltage: 4.8VDC - 6VDC
    PWM Period: 20ms (50Hz)
    Position "0": 1.5 ms pulse 
    Middle "90" (full right): ~2ms pulse
    Position "180" (full left): ~1ms pulse)

  History
*/   
  char my_ver[] = "1.0.0"; // Semantic Versioning (https://semver.org/)
/*
  Version YYYY-MM-DD Description
  ------- ---------- ---------------------------------------------------------------------------------------
  1.0.0   2018-03-30 Code base created.
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
 Define servo motor controller constants and global variables. Depending on your servo make, the pulse width 
 min and max may vary, you want these to be as small/large as possible without hitting the hard stop for max 
 range. Note that the three SG90 micro servo motors we are using all seem to vary slightly in their MAX/MIN
 values (likely due to variation inthe POTs used to determine position) so values have been selected that 
 seem to work for all three motors, meaning that we are likly not using full range of motion on each motor. 
 ***********************************************************************************************************/
#define SERVOMIN  130 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  520 // this is the 'maximum' pulse length count (out of 4096)
#define PWMFreq 50 // The SG90 servos run at 50 Hz updates
uint8_t ServoController_I2C_Address = 0x40; //I2C address of servo controller
Adafruit_PWMServoDriver ServoMotor = Adafruit_PWMServoDriver(&Wire, ServoController_I2C_Address);
uint8_t servoMotor0 = 0;
uint8_t servoMotor1 = 1;
uint8_t servoMotor2 = 2;

/***********************************************************************************************************
 This function initializes the console where trace information is sent during sketch execution.
 ***********************************************************************************************************/
void initConsole()
{
  Serial.begin(115200); // NOTE: Be sure to put monitor_speed = 115200 in platformio.ini
  delay(5); // Give serial time to initialize before using it
  Serial.println("[initConsole] **************************************************************************"); 
} // initConsole()

/***********************************************************************************************************
 This function dumps a bunch of useful info to the terminal. Note that an effort has been made to put
 most of the printing of key data into the class amDisplayRunningConfiguration() but some data must be
 generated inside this sketch to get the correct information abot this sketch and not the class file. This 
 is why the file name and line number informaiton must be called from this routine and not the class.
 ***********************************************************************************************************/
void display_Running_Sketch()
{
  Serial.print("[display_Running_Sketch] Start of sketch. Source code line: ");
  Serial.println(__LINE__); // Get currrent line of code being executed
  Serial.print("[display_Running_Sketch] Sketch Name: ");
  Serial.println(__FILE__); // Get file name of sketch being run
  aboutThisSketch.display(); // Show additional information about this sketch in the console
} // display_Running_Sketch()

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
 This function initializes the servo motor controller
 ***********************************************************************************************************/
void initServoController()
{
  Serial.println("[initServoController] Initialize servo controller");
  startI2Cone(); // Start the first I2C bus, which the servo controller is connected to
  ServoMotor.begin(); // Initialize servo motor controller
  ServoMotor.setPWMFreq(60); // Set update frequency for servo motors
} // initServoController()


/***********************************************************************************************************
 This function sets the pulse length in seconds e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse 
 width. It is not precise.
 ***********************************************************************************************************/
void setServoPulse(uint8_t n, double pulse) 
{
  double pulselength;
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print("[setServoPulse] Pulse length = ");
  Serial.print(pulselength); 
  Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print("[setServoPulse] Pulse length = ");
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert to us
  pulse /= pulselength;
  Serial.println(pulse);
  ServoMotor.setPWM(n, 0, pulse);
} // setServoPulse()

/***********************************************************************************************************
 Standard Arduino setup function that runs at the start of the sketch. Runs once.
 ***********************************************************************************************************/
void setup() 
{
  initConsole(); // Set up the console for trace messages
  display_Running_Sketch(); // Dump useful sketch information to the console
  initServoController(); // Set up the OLED display
} // setup()

/***********************************************************************************************************
 Standard Arduino main loop. Runs as a continuous loop.
 ***********************************************************************************************************/
void loop() 
{
  for(uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
  {
    ServoMotor.setPWM(servoMotor0, 0, pulselen);
    ServoMotor.setPWM(servoMotor1, 0, pulselen);
    ServoMotor.setPWM(servoMotor2, 0, pulselen);
  } // for
  delay(500);
  for(uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
  {
    ServoMotor.setPWM(servoMotor0, 0, pulselen);
    ServoMotor.setPWM(servoMotor1, 0, pulselen);
    ServoMotor.setPWM(servoMotor2, 0, pulselen);
  } // for
  delay(500);
} // loop()