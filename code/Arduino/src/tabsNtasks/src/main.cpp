
/***********************************************************************************************************
  Synopsis:
  This is a test sketch used to explore the use of include files to break code up into multiple tabs in
  VSC as well as to explore managing tasks and interrupts with the ESP32 SOC. This code was inspired by
  https://www.hackster.io/rayburne/esp32-in-love-with-both-cores-8dd948 

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
  char my_ver[] = "1.0.0"; // Semantic Versioning (https://semver.org/)
/*
  ------- ---------- ---------------------------------------------------------------------------------------
  1.0.0   2018-11-25 Code base created.
 ***********************************************************************************************************/
#include <Arduino.h>  // Makes .CPP files work like .INO files. Contains standard Arduino references  
#include <Streaming.h>  // Ref: http://arduiniana.org/libraries/streaming/
#include <Workload.h> // Inclide file created for this sketch
#include <Wire.h> // Needed for I2C communication

/***********************************************************************************************************
 Define timer0 variables and objects
 ***********************************************************************************************************/
#define timer_number_0 3 // Timer number can be 0 to 3, since we have 4 hardware timers on ESP32
#define timer_prescaler_0 80 // Prescaler, divides 80 MHz by 80 = 1 MHz, or interrupt every microsecond
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; //Synchronize variable updates shared by ISR and main
#define timer_cnt_up true // Used to tell timer to count up
#define timer_cnt_down false // Used to tell timer to count down 
hw_timer_t * timer0 = NULL; // Pointer used to configure the timer
volatile long cntTimer0; 

/***********************************************************************************************************
 Define FreeRTOS task variables and objects
 ***********************************************************************************************************/
TaskHandle_t TaskA, TaskB; // Handles for FreeRTOS tasks (threads)
long cntTask1, cntTask2, cntLoop; // Counters for each task

/***********************************************************************************************************
 Define two I2C buses 
 I2C bus 1 is I2Cone or Wire(): SDA1 on pin 23 and SCL1 on pin 22
 I2C bus 2 is I2Ctwo or Wire1(): SDA2 on pin 4 and SCL2 on pin 5
 ***********************************************************************************************************/
#define SDA1 23
#define SCL1 22
#define SDA2 4
#define SCL2 5
//TwoWire I2Cone = TwoWire(0);
//TwoWire I2Ctwo = TwoWire(1);

volatile int timer0Core,timer0Priority;
int TaskAcore, TaskBcore;

/***********************************************************************************************************
 This function runs as its own FreeRTOS task (thread)
 ***********************************************************************************************************/
void Task1( void * parameter )
{
  TaskAcore=xPortGetCoreID();
  while(1)
  {
    unsigned long start = millis();   // ref: https://github.com/espressif/arduino-esp32/issues/384
    Workload();
    //Serial << "[Task1] Task 1 complete running on Core " << (xPortGetCoreID()) << " Time = " << (millis() - start)<< " mS"  << endl;
    cntTask1++;
    delay(10);
  } //while
} //Task1()

/***********************************************************************************************************
 This function runs as its own FreeRTOS task (thread)
 ***********************************************************************************************************/
void Task2( void * parameter )
{
  TaskBcore=xPortGetCoreID();
  while(1) 
  {
    unsigned long start = millis();   // ref: https://github.com/espressif/arduino-esp32/issues/384
    Workload();
    //Serial << "[Task2] Task 2 complete running on Core " << (xPortGetCoreID()) << " Time = " << (millis() - start) << " mS"  << endl ;
    cntTask2++;
    delay(10);
  } //while
} //Task2()

/***********************************************************************************************************
 This is the interrupt handler for hardware timer 0. This routine coordinates the variable cntTimer0 with
 with main.  
 ***********************************************************************************************************/
void IRAM_ATTR onTimer0() 
{

    portENTER_CRITICAL_ISR(&timerMux); // Prevent anyone else from updating the variable
    timer0Core=xPortGetCoreID();
    cntTimer0++ ; // Count one more t0 int seen in this second
    timer0Priority=uxTaskPriorityGet(NULL);
    portEXIT_CRITICAL_ISR(&timerMux); // Allow anyone else to update the variable
 
} //onTimer0()

/***********************************************************************************************************
 Setup for timer0 interrupt
 ***********************************************************************************************************/
void startTimer0()
{

    timer0 = timerBegin(timer_number_0, timer_prescaler_0, timer_cnt_up); // Pointer to hardware timer 0
    timerAttachInterrupt(timer0, &onTimer0, true); // Bind onTimer function to hardware timer 0
    timerAlarmWrite(timer0, 20, true); // Interrupt generated is of edge type not level (third arg) at 
                                            // count of 1000000 (second argument)
    timerAlarmEnable(timer0); // Enable hardware timer 0

} //startTimer0()

/***********************************************************************************************************
 This function starts and scans the I2Cone buse for attached devices. This code was taken from 
 http://playground.arduino.cc/Main/I2cScanner then updated with this 
 https://github.com/espressif/arduino-esp32/issues/977 in order toi support 2 buses. 
 The LCD should be attached to I2Cone and the MPU should be attached to I2Ctwo. SCL and SDA on both buses
 should have a 2.2K or 2.4K pull up resitor (3.3VDC). 
 ***********************************************************************************************************/
void startI2Cone()
{    
//    I2Cone.begin(SDA1,SCL1,400000); // 400KHz, uppder speed limit for ESP32 I2C
    Wire.begin(SDA1,SCL1,400000); // 400KHz, uppder speed limit for ESP32 I2C
    uint8_t cnt=0;
    for(uint8_t i=0;i<128;i++)
    {
//        I2Cone.beginTransmission(i);
        Wire.beginTransmission(i);
//        uint8_t ec=I2Cone.endTransmission(true);
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
} //startI2Cone()

/***********************************************************************************************************
 This function starts and scans the I2Ctwo bus for attached devices. This code was taken from 
 http://playground.arduino.cc/Main/I2cScanner then updated with this 
 https://github.com/espressif/arduino-esp32/issues/977 in order to support 2 buses. 
 The LCD should be attached to I2Ctwo and the MPU should be attached to I2Ctwo. SCL and SDA on both buses
 should have a 2.2K or 2.4K pull up resitor (3.3VDC). 
 ***********************************************************************************************************/
void startI2Ctwo()
{
//    I2Ctwo.begin(SDA2,SCL2,400000); // 400KHz, uppder speed limit for ESP32 I2C
    Wire1.begin(SDA2,SCL2,400000); // 400KHz, uppder speed limit for ESP32 I2C
    uint8_t cnt=0;
    for(uint8_t i=0;i<128;i++)
    {
//        I2Ctwo.beginTransmission(i);
        Wire1.beginTransmission(i);
//        uint8_t ec=I2Ctwo.endTransmission(true);
        uint8_t ec=Wire1.endTransmission(true);
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
} //startI2Ctwo()

/***********************************************************************************************************
 This is the setup function for Arduino sketches. It runs first.
 ***********************************************************************************************************/
void setup() 
{
  Serial.begin(115200); // Ensure that platformio.ini has the line "monitor_speed = 115200" added
  delay(500); // Small delay to allow serial to begin properly
  Serial.print("[setup] *****************************************");
  Serial.print("[setup] ESP32 SDK used: ");
  Serial.println(ESP.getSdkVersion());
  Serial.print("[setup] Setup is running on core ");
  Serial.println(xPortGetCoreID());
  Serial.print(" with a priority of ");
  Serial.println(uxTaskPriorityGet(NULL));

  cntTask1 = 0;
  cntTask2 = 0;
  cntLoop = 0;
  // Ref: http://esp32.info/docs/esp_idf/html/db/da4/task_8h.html#a25b035ac6b7809ff16c828be270e1431
  xTaskCreatePinnedToCore(
    Task1,                  /* pvTaskCode */
    "Workload1",            /* pcName */
    1000,                   /* usStackDepth */
    NULL,                   /* pvParameters */
    1,                      /* uxPriority */
    &TaskA,                 /* pxCreatedTask */
    0);                     /* xCoreID */
  // Ref: http://esp32.info/docs/esp_idf/html/db/da4/task_8h.html#a25b035ac6b7809ff16c828be270e1431
  xTaskCreatePinnedToCore(
    Task2,
    "Workload2",
    1000,
    NULL,
    1,
    &TaskB,
    1);
  
  startI2Cone(); // Scan the second I2C bus for MPU   AM: This is the issue
  startI2Ctwo(); // Scan the second I2C bus for MPU   AM: This is the issue
  startTimer0(); // Define and kick-off Timer0 execution

  int tmp1, tmp2;
  portENTER_CRITICAL_ISR(&timerMux); // Prevent anyone else from updating the variable
  tmp1=timer0Core;
  tmp2=timer0Priority;
  portEXIT_CRITICAL_ISR(&timerMux); // Allow anyone else to update the variable
  
  Serial.print("[setup] timer0 core = ");
  Serial.println(tmp1);
  Serial.print("[setup] timer0 assigned priority = ");
  Serial.println(tmp2);

  Serial.print("[setup] Task1 core = ");
  Serial.println(TaskAcore);
  Serial.print("[setup] Task1 assigned priority = ");
  Serial.println(uxTaskPriorityGet(TaskA));

  Serial.print("[setup] Task2 core = ");
  Serial.println(TaskBcore);
  Serial.print("[setup] Task2 assigned priority = ");
  Serial.println(uxTaskPriorityGet(TaskB));

} //setup()

/***********************************************************************************************************
 This is the main function for Arduino sketches. This task will run in the ESP32 Arduino default FreeRTOS
 context and loops forever
 ***********************************************************************************************************/
void loop() 
{
  //unsigned long start = millis();
  cntLoop++;
  //Serial << "[loop] Task 0 complete running on Core " << (xPortGetCoreID()) << " Time = " << (millis() - start) << " mS"  << endl ;;
  Serial.print("[loop] Loop is running on core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" with a priority of ");
  Serial.print(uxTaskPriorityGet(NULL));
  Serial.print(" cntLoop = ");
  Serial.print(cntLoop);
  Serial.print(" cntTask1 = ");
  Serial.print(cntTask1);
  Serial.print(" cntTask2 = ");
  Serial.print(cntTask2);
  portENTER_CRITICAL_ISR(&timerMux); // Prevent anyone else from updating the variable
  volatile long tmp1=cntTimer0; 
  portEXIT_CRITICAL_ISR(&timerMux); // Allow anyone else to update the variable
  Serial.print(" cntTimer0 = ");
  Serial.println(tmp1);
  delay(10) ;
} //loop()