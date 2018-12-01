#include <Arduino.h>
static const uint8_t leftMotorDir = 25;
static const uint8_t leftMotorStep = 26;
static const uint8_t rightMotorDir = 14;
static const uint8_t rightMotorStep = 32;
volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

/**********************************************************************************
 * Function that is called when hardwre timer0 meets its alarm state, which is once
 * every 1ms.
 **********************************************************************************/  
void IRAM_ATTR onTimer() 
{
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  digitalWrite(leftMotorStep, !digitalRead(leftMotorStep));   // Toggle bit on and off
  digitalWrite(rightMotorStep, !digitalRead(rightMotorStep)); // Toggle bit on and off
  portEXIT_CRITICAL_ISR(&timerMux);

} //onTimer()

/**********************************************************************************
 *
 **********************************************************************************/  
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
  int timerOrder = 0; // We only have 4 timers so the order can be 0,1,2,3
  int timerDivider = 80; // ESP32 main clock is 80MHZ so every tick will take T = 1/(80MHZ/80) = 1 microsecond
  bool timerCountUp = true; // Make this a countup timer 
  timer = timerBegin(timerOrder, timerDivider, timerCountUp); // Ticks every 1 microsecond
  timerAttachInterrupt(timer, &onTimer, true); // Attach our onTimer() function to our timer
  timerAlarmWrite(timer, 10, true); // Trigger alarm every 1ms and repeat the alarm (true)
  timerAlarmEnable(timer); // Start the alarm

} //setup()

/**********************************************************************************
 *
 **********************************************************************************/  
void loop() 
{

  if (interruptCounter > 0) 
  {

    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;

    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);

  } //if

} //loop()
