#include <Arduino.h>
//int TWIPI_LED = 36;
static const uint8_t TWIPI_LED = 15;

void setup() 
{
  // Initialize 9th pin on long side counting from USB end pin as an output
  pinMode(TWIPI_LED, OUTPUT);
  Serial.begin(115200); // Open a serial connection at 115200bps
  Serial.println("*****");

}

void loop() 
{
  digitalWrite(TWIPI_LED, 1);   // turn the LED on (HIGH is the voltage level)
  Serial.println("on");
  delay(1000);                  // wait for a second
  digitalWrite(TWIPI_LED, 0);    // turn the LED off by making the voltage LOW
  Serial.println("off");
  delay(1000);                  // wait for a second
}