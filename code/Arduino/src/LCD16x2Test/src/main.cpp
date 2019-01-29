#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//If you use the I2C LCD Shield by catalex, you should change 0x27 to 0x38 or 0x20
//LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x38,16,2);  //if the LCD does not work, please use the other address:0x38

void setup()
{
  lcd.init();                      // initialize the lcd 

  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Hello, world!");
}

void loop()
{
}