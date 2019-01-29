#include <Arduino.h>
/***********************************************************************************************************
  Synopsis:
  This sketch is the used to test the DRV8825 stepper motor controller. 
  NOTE!!! /Users/andrewmitchell/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-i2c.c
  has custom lines in it that differ from the ESP-IDF stable release. Make sure that these lines are 
  maintained when updating to the latest PlatformIO ESP-IDF files. Failure to do so will result in unrelaible
  I2C and hardware timer behavior. See the clsojng comments for issue https://github.com/va3wam/TWIPi/issues/14
  for full details.

  Environment:
  - Platform: Espressif 32 (https://www.espressif.com/en/products/hardware/esp32/overview)
  - Framework: Arduino 
  - Embedded board: Adafruit HUZZAH32 Feather (https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts)
  - SOC: WROOM32 module (https://dl.espressif.com/doc/esp-idf/latest/hw-reference/modules-and-boards.html)
  - IDE: Visual Studio Code v1.27.2 (https://code.visualstudio.com/)
  - VSC plugin: platformIO v3.6.1a4 (https://platformio.org/platformio-ide)
  - RTOS: FreeRTOS (https://www.freertos.org/Documentation/RTOS_book.html)
  - Language: C++ (Arduino)

  History
  Version YYYY-MM-DD Description
*/   
  String my_ver = "1.0.0";
/*  char my_ver[] = "1.0.0"; // Semantic Versioning (https://semver.org/)
    Given a version number MAJOR.MINOR.PATCH, increment the:
      MAJOR version when you make incompatible API changes,
      MINOR version when you add functionality in a backwards-compatible manner, and
      PATCH version when you make backwards-compatible bug fixes.

  ------- ---------- ---------------------------------------------------------------------------------------
  1.0.0   2019-01-20 Code base created.
 ***********************************************************************************************************/
static const uint8_t leftMotorDir = 25;
static const uint8_t leftMotorStep = 26;
static const uint8_t rightMotorDir = 14;
static const uint8_t rightMotorStep = 32;
unsigned long runTime;
float duty_cycle,period,freq,uptime,downtime;

/***********************************************************************************************************
 Set up function
 ***********************************************************************************************************/
void setup() 
{
  pinMode(leftMotorDir, OUTPUT);
  pinMode(leftMotorStep, OUTPUT);
  pinMode(rightMotorDir, OUTPUT);
  pinMode(rightMotorStep, OUTPUT);
  Serial.begin(115200); // Open a serial connection at 115200bps
  delay(2);
  Serial.println("*****");
  digitalWrite(leftMotorDir, 1);   // Set motor direction
  digitalWrite(rightMotorDir, 1);   // Set motor direction
  duty_cycle = 50; // Up time per cycle (percentage)
  freq = 250; // Frequency in kHz
  period = 1/freq*1000; // Length of cycle in microseconds
  uptime = period*(duty_cycle/100); // Time to leave signal high in microseconds
  downtime = period-uptime; // Time to leave signal low in microseconds
  Serial.println("");
  Serial.printf("[setup] Parameters for this test...\n");
  Serial.printf("[setup] Frequency = %f kHz (period of %f microseconds)\n",freq,period);
  Serial.printf("[setup] Duty cycle = %f percent\n",duty_cycle);
  Serial.printf("[setup] Uptime = %f microseconds. Downtime = %f microseconds\n",uptime,downtime);
  runTime = micros(); // Note the time that this microprocessor has been running in microseconds
} //setup()

/***********************************************************************************************************
 Main loop
 ***********************************************************************************************************/
void loop() 
{
  digitalWrite(leftMotorStep, 1);   // Set step pin high
  digitalWrite(rightMotorStep, 1);   // Set step pin high
  delayMicroseconds(uptime);
  digitalWrite(leftMotorStep, 0);   // Set step pin low
  digitalWrite(rightMotorStep, 0);   // Set step pin low
  delayMicroseconds(downtime);
} //loop()