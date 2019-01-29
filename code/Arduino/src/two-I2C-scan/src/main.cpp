/* I2C slave Address Scanner
for 5V bus
 * Connect a 4.7k resistor between SDA and Vcc
 * Connect a 4.7k resistor between SCL and Vcc
for 3.3V bus
 * Connect a 2.4k resistor between SDA and Vcc
 * Connect a 2.4k resistor between SCL and Vcc
Kutscher07: Modified for TTGO TQ board with builtin OLED
 */

#include <Wire.h>
#include <Arduino.h>
#include <dummy.h>  //for esp32

#define SDA1 23
#define SCL1 22

#define SDA2 4
#define SCL2 5


TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);

void scan1(){
Serial.println("Scanning I2C Addresses Channel 1");
uint8_t cnt=0;
for(uint8_t i=0;i<128;i++){
  I2Cone.beginTransmission(i);
  uint8_t ec=I2Cone.endTransmission(true);
  if(ec==0){
    if(i<16)Serial.print('0');
    Serial.print(i,HEX);
    cnt++;
  }
  else Serial.print("..");
  Serial.print(' ');
  if ((i&0x0f)==0x0f)Serial.println();
  }
Serial.print("Scan Completed, ");
Serial.print(cnt);
Serial.println(" I2C Devices found.");

}
void scan2(){
Serial.println("Scanning I2C Addresses Channel 2");
uint8_t cnt=0;
for(uint8_t i=0;i<128;i++){
  I2Ctwo.beginTransmission(i);
  uint8_t ec=I2Ctwo.endTransmission(true);
  if(ec==0){
    if(i<16)Serial.print('0');
    Serial.print(i,HEX);
    cnt++;
  }
  else Serial.print("..");
  Serial.print(' ');
  if ((i&0x0f)==0x0f)Serial.println();
  }
Serial.print("Scan Completed, ");
Serial.print(cnt);
Serial.println(" I2C Devices found.");

}
void setup(){
Serial.begin(115200);
I2Cone.begin(SDA1,SCL1,400000); // SDA pin 21, SCL pin 22 TTGO TQ
I2Ctwo.begin(SDA2,SCL2,400000); // SDA pin 5, SCL pin 4 builtin OLED

}
void loop(){
scan1();
Serial.println();
delay(100);
scan2();
Serial.println();
delay(5000);


}