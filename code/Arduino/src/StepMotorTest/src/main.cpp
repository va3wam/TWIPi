#include <Arduino.h>
static const uint8_t leftMotorDir = 25;
static const uint8_t leftMotorStep = 26;
static const uint8_t rightMotorDir = 14;
static const uint8_t rightMotorStep = 32;

void setup() 
{
  pinMode(leftMotorDir, OUTPUT);
  pinMode(leftMotorStep, OUTPUT);
  pinMode(rightMotorDir, OUTPUT);
  pinMode(rightMotorStep, OUTPUT);
  Serial.begin(115200); // Open a serial connection at 115200bps
  Serial.println("*****");
  digitalWrite(leftMotorDir, 1);   // Set motor direction
  digitalWrite(rightMotorDir, 1);   // Set motor direction

}

void loop() 
{
  digitalWrite(leftMotorStep, 1);   // Set step pin high
  digitalWrite(rightMotorStep, 1);   // Set step pin high
  Serial.println("high");
  delay(1);                  // wait 1 milli second
  digitalWrite(leftMotorStep, 0);   // Set step pin low
  digitalWrite(rightMotorStep, 0);   // Set step pin low
  Serial.println("low");
  delay(20);                  // wait 20 milli seconds
}