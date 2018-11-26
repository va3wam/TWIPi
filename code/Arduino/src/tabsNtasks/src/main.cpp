
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
#define timer_number_0 0 // Timer number can be 0 to 3, since we have 4 hardware timers on ESP32
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
//#define SDA1 23
//#define SCL1 22
#define SDA2 4
#define SCL2 5
//TwoWire I2Cone = TwoWire(0);
//TwoWire I2Ctwo = TwoWire(1);

/***********************************************************************************************************
 This function runs as its own FreeRTOS task (thread)
 ***********************************************************************************************************/
void Task1( void * parameter )
{
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
    cntTimer0++ ; // Count one more t0 int seen in this second
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
 This is the setup function for Arduino sketches. It runs first.
 ***********************************************************************************************************/
void setup() 
{
  Serial.begin(115200); // Ensure that platformio.ini has the line "monitor_speed = 115200" added
  delay(500); // Small delay to allow serial to begin properly
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
    Serial.print("[Setup] Task1 assigned priority = ");
    Serial.print(uxTaskPriorityGet(TaskA));
  // Ref: http://esp32.info/docs/esp_idf/html/db/da4/task_8h.html#a25b035ac6b7809ff16c828be270e1431
  xTaskCreatePinnedToCore(
    Task2,
    "Workload2",
    1000,
    NULL,
    1,
    &TaskB,
    1);
    Serial.print("[Setup] Task2 assigned priority = ");
    Serial.println(uxTaskPriorityGet(TaskB));
  
  Wire1.begin(SDA2,SCL2,400000); // 400KHz, uppder speed limit for ESP32 I2C  
  startTimer0(); // Define and kick-off Timer0 execution

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
  Serial.print("cntLoop = ");
  Serial.print(cntLoop);
  Serial.print(" cntTask1 = ");
  Serial.print(cntTask1);
  Serial.print(" cntTask2 = ");
  Serial.print(cntTask2);
  Serial.print(" cntTimer0 = ");
  Serial.println(cntTimer0);
  delay(10) ;
} //loop()