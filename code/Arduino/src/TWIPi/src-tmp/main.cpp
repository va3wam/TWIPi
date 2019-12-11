
#include <Arduino.h>              // Required for platformIO Intellisense? Standard Arduino functions?


/***********************************************************************************************************
 This is the setup function for Arduino sketches
 ***********************************************************************************************************/
void setup() 
{
    Serial.begin(115200); // Open a serial connection at 115200bps
    Serial.println("");


} //setup()


/***********************************************************************************************************
 This is the main function for Arduino sketches
 ***********************************************************************************************************/
void loop() 
{
    cntLoop++;
digitalWrite(13, 1); // Set LED GPIO HIGH, which turn the LED on
delay(300);
digitalWrite(13, 0); // Set LED GPIO HIGH, which turn the LED on
delay(300);
} //loop()