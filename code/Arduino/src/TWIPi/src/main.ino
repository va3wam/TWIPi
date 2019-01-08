/* Table of Contents ... egrep '^void|static void|volatile void' main.ino

void flashLCD();  // from http://forum.arduino.cc/index.php?topic=42835.0
static void writeLED(bool); // Define function that sets the onbard LED, accepts 0 or 1
void IRAM_ATTR onTimer0()
void display_Running_Sketch()
void writeLED(bool LEDon)
void startI2Cone()
void startI2Ctwo()
void initializeLCD()
void sendLCD(String LCDmsg, byte LCDline)
void scrollLCD(String LCDmsg, byte LCDline)
void flashLCD()
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
void startWiFi()
void startDNS()
volatile void handleRoot()
volatile void handleNotFound()
void startWebServer()
void startWebsocket()
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
void process_Client_JSON_msg(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
void sendClientPing(uint8_t num)
void sendClientLEDState(uint8_t num)
void sendClientLCDState(uint8_t num)
void reportTel(String msg)
void sendClientBalanceGraph(uint8_t num,float a1, float a2, float a3, float a4)
void sendClientVariableValue(uint8_t num, String variable)
void initializeIMU()
void set_gyro_registers()
void read_mpu_6050_data()
void startTimer0()
void initializeMotorControllers()
void setup()
void monitorWebsocket(void * parameter)
void monitorWeb(void * parameter)
void balanceRobot()
void loop()
*/

#include <Arduino.h>              // Required for platformIO Intellisense? Standard Arduino functions?
#include <WiFi.h>                 // Required to connect to WiFi network
#include <WiFiMulti.h>            // Allows us to try connecting to multiple Access Points
#include <ESP32WebServer.h>       // https://github.com/Pedroalbuquerque/ESP32WebServer
#include <ESPmDNS.h>              // Required for domain name service
#include <WebSocketsServer.h>     // https://github.com/Links2004/arduinoWebSockets
#include <Wire.h>                 // Needed for I2C communication
#include <LiquidCrystal_I2C.h>    // https://github.com/marcoschwartz/LiquidCrystal_I2C
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson

/***********************************************************************************************************
  Synopsis:
  This is the main sketch for the TWIPi robot. 

  NOTE!!! /Users/andrewmitchell/.platformio/packages/framework-arduinoespressif32/cores/esp32/esp32-hal-i2c.c
  has custom lines in it that differ from the ESP-IDF stable release. Make sure that these lines are 
  maintained when updating to the latest PlatformIO ESP-IDF files. Failure to do so will result in unrelaible
  I2C and hardware timer behavior. See the closing comments for issue https://github.com/va3wam/TWIPi/issues/14
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
  String my_ver = "2.2.9";
/*  char my_ver[] = "2.2.1"; // Semantic Versioning (https://semver.org/)

    Given a version number MAJOR.MINOR.PATCH, increment the:
      MAJOR version when you make incompatible API changes,
      MINOR version when you add functionality in a backwards-compatible manner, and
      PATCH version when you make backwards-compatible bug fixes.

  ------- ---------- ---------------------------------------------------------------------------------------
  2.2.9   2019-01-04 - generate some telemetry data related to balancing every 4 milliseconds
          -re-enable the I value in PID - it had been previously commented out
          -todo: accelerometer calibration value? 
             remove crude compensation by +8 -done
             remove or fix pid_error_temp adjustment that's asymmetrical -done. removed
             reduce Pid_I_gain -done, from 1.2 to .4
             reduce Pid_D_gain - done, from 30 to 10
             consider re-introducing I_mem fading - deferred until more telemmetry analysed
             change bot_fast back to 250 for higher top end speed - test other changes first
             minimize impact of unstable angle_acc number. [only in anti drift calc (.996 x + .004 y)]
             clean out numchuk related code - mostly done
             set up balance point by:
                -getting bot at it's balance point (build a nifty jig?)
                -note value displayed on startup after text: "[initializeIMU_2]: Balance value...."
                -using this value in startWiFi() for MAC address 30:AE:A4:37:54:F8 for acc_calibration_value
                -this will result in the balance goal of zero degrees accurately reflecting off-center possibility
             move ISR param update until after the 4 Msec sync spin, for consistency (but a delay?)
             count step pulses per 4msec loop
             figure out why telemetry has long gaps of multiple 4msec intervals. PC? ESP32? algorithm?
             convert LINE() macro calls to LINE_TIME() which logs text arg and appends source code line and millis() - done
             allow robot to continue and balance if it can't find a WiFi network. se variable WiFi_active
             recreate the original "linear behaviour" for PID > wheel control mapping
  2.2.8   2019-01-01 - return IMU warmup to 11 seconds so it's stable before calibration, 
          -split IMS initialization into two parts to allow network startup to run in parallel
          -add timeline macro to output timestamps + line numbers during setup()
          -add crude table of contents at top of file to help find routines in source code
          -simplify the I2C bus scan output,  discarding double dots.
          -do quicker speed changes by having balanceRobot zero the throttle_*_motor_memory variables
          -create LINET function that outputs string argument, followed by source code line number and milis timestamp
          -convert output of URL in startDNS to escape slashes so compiler won't think they're comments
          -move initializeIMU_1 earlier in setup to maximize overlapping processing, and minimize boot time
  2.2.7   2018-12-11 -add crude compensation for both gyro and accel angle measurements, because reading were off
          by 9 degees. also, gyro and accel don't agree - off by about 1.5 degrees
          -change bot fast speed control from 250 to 300 due to stalling motors
  2.2.6   2018-12-09 -reduce time spent in boot sequence from 42 to 13 seconds (to IP address display)
          -note for bot specific calibration purposes, original TWIPi has MAC address: 30:ae:a4:37:54:f8
          -remove 6 degree compensation after IMU fell off header during re-assembly
          -disable pid I memory fading
          -un-comment the setting of pid_i_mem. It was never being used our PID was actually PD.
          -now TWIPi's hyper reactive. reducing gain parameters. Originally 30, 1.5, 30
          -10, 1.2, 10: still seems to get into an oscillation. robot's right wheel making funny noises, even idle
          -bot_fast changed from 250 to 300, guessing oscillation is driving motors too fast
          -still getting oscillation at fast=400, but not at 500. 
          -some trace capability at 4 Msec level would really help, but difficult to capture
          -options to troubleshoot wheel noise:
              -swap position of DRV8825 & see if it changes wheels
              -swap stepper motors
              -scope out the control and power lines for that motor
              -use different GPIO's for step and dir on that wheel (hardware challenge)
              -try motors on a bench / breadboard system & see if problem can be reproduced, fixed...
              -research flakey GPio performance due to hidden Espressif activity
          -doesn't seem to be IMU, but could undo boot speed up changes to see if it makes a difference
  2.2.5   2018-12-08 -add 6 degree compensation for IMU mounting error
  2.2.4   2018-12-07 -recognize Doug's WiFi so boot up doesn't stall
          -fic esp32-hal-i2c.c in Doug's PlatformIO library to get timer0 interrupts to work for code from his PC
  2.2.3   2018-12-06 -recode the embedded web server html string so the compiler won't give errors
          -change string output in process_Client_JSON_msg to avoid compile errors
          -remove obsolete commented out code (can still be retrieved from previous versions)
  2.2.2   2018-11-28 Changed reference to Gyro and accelerometer sensors to jive with the new IMU 
          orientation inside the robot's chasis. X (roll) has become Y (pitch), Y (pitch) has become Z (Yaw).
          Also had to reverse the front and back motor rotation value to align with orientaiton of the motors 
          and IMU in the new chasis.  
  2.2.1   2018-11-25 Replaced all reference to TwoWire objects I2cOne and I2CTwo with Wire and Wire1 in an
          effort to remove hardware conflicts between timer0 and I2CTwo. This did not help. In the end the
          issues turned out to be a problem with the PlatformIO esp32-hal-i2c.c file. See issue Inconsistent 
          variable behavior #14 in our TWIPi repositories (https://github.com/va3wam/TWIPi/issues/14).
  2.2.0   2018-11-25 Altered setup() sequence and added mre LCD messages to track bootup. Also created 
          handles for FreeRTOS tasks created to run in different threads so we can reference them later.
  2.1     2018-11-14 Moved LED used for remote control from the built-in LED (13) to a pin for a seperate
          LED viewable outside the robot chasis (pin 15). Comments and variable name suggesting that we use
          the onboard or built-in LED are now misleading. Also moved motor pins to be compatible with the 
          TWIPI-PB1A-MIRROR board. NOTE, this code is MB1 compatable. To be MB2 compatible the motor
          pin numbers must be changed. 
  2.0     2018-10-11 Separated the I2C devices onto their own I2C bus. I2Cone has the LCD attached and 
          LCDtwo has the MPU attached to it.
  1.6     2018-04-13 The 1.5 fix made things worse. The error happend sooner. Commenting out WiFi all
          together to see if I2C becomes more reliable. The error is i2cWrite(): Busy Timeout! Addr: 68. 
          The I2C address 0x68 is assigned to the MPU6050 runing on the GY-521 board. This seems to have
          made the I2C bus communication reliable at last. Need to sort out how to have network services
          like WiFi, mDNS, websockets and web server running at the sae time without messing up the 
          reliability of the I2C bus. Also note that external 4.7K pull-up resistors were added around 
          version 1.4 and while they did not improve things they were left in and are part of the currently 
          stable configuration.
  1.5     2018-04-13 Pulled web service polling out of their own threads and put them in the main line in
          order to address an I2C error where the wire() calls result in a HAL error because the line is
          busy. The theory is that the WiFi activity is causing delays with writes and reads which cause 
          this error. There are others reporting issues like this and a new library fix is in the works to 
          address timing issues. 
  1.4     2018-04-13 Updated LINE macro to not TAB but rather label code line number more clearly. A cleanup
          of the short-cut macros would be handy as there is some redundancy now
  1.3     2018-04-02 Added motor control and PID logic from SBS Mark 2
  1.2     2018-04-01 Added DNS, Web and Websocket services
  1.1     2018-03-31 Moved init of IMU to its own FreeRTOS thread to and moved it up further in the boot 
          procss to allow for faster boot up by running the IMU init in parallel with other boot up tasks.
  1.0     2018-03-27 Code base created.
 ***********************************************************************************************************/

/***********************************************************************************************************
 Debug printing shortcuts.
 ***********************************************************************************************************/

#define LINE_TIME(name) sp(name); sp(" Line:");sp(__LINE__);sp(" millis:");spl(millis());
// convention is that on main entry to a routine, the text arg is in the format:
//      "<name of routine> general function."
// for subsequent calls providing more information, text arg is:
//      " [name of routine] event summary."
// note the leading space in above text arg, which indents entries other than main routine entrypoints

#define sp Serial.print                     // Shortform print no carrige return
#define spl Serial.println                  // Shortform print with carrige return
#define spc Serial.print(", ")              // Shortform print comma and space
#define spf Serial.printf                   // Shortform formatted printing with no line return
#define spdl(label,val) sp(label); spl(val) // Displays one labelled variable, ending with new-line 
                                            // example: spdl(" param2= ",param2);     
#define spd(label,val) sp(label); sp(val)   // Suitable for displaying multiple variables per line
                                            // example: spd("left motor= ",left_motor); 
                                            // spdl("  right motor= ",right_motor);
/***********************************************************************************************************
 Defines needed when using PLatformIO in Microsoft Visual Studio Code instead of the native Adduino IDE
 ***********************************************************************************************************/
#define LED_BUILTIN 15 // Pin that the onboard LED is connected to on the Huzzah32 board

/***********************************************************************************************************
 Define two I2C buses 
  I2C bus 1 (I2Cone): SDA1 on pin 23 and SCL1 on pin 22
  I2C bus 2 (I2Ctwo): SDA2 on pin 4 and SCL2 on pin 5
 ***********************************************************************************************************/
#define SDA1 23
#define SCL1 22
#define SDA2 4
#define SCL2 5

/***********************************************************************************************************
 Define timer0 variables and objects
 ***********************************************************************************************************/
#define timer_number_0 0 // Timer number can be 0 to 3, since we have 4 hardware timers on ESP32
#define timer_prescaler_0 80 // Prescaler, divides 80 MHz by 80 = 1 MHz, or interrupt every microsecond
hw_timer_t * timer0 = NULL; // Pointer used to configure the timer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; // Synchronize variable updates shared by ISR and main
#define timer_cnt_up true // Used to tell timer to count up
#define timer_cnt_down false // Used to tell timer to count down 
volatile long cntTimer0; 
volatile long t0_per_sec; 

/***********************************************************************************************************
 Define telemetry debug related variables
 ***********************************************************************************************************/
bool telClientConnected = false;  

/***********************************************************************************************************
 Define FreeRTOS task variables and objects
 ***********************************************************************************************************/
TaskHandle_t monWebSocket; // Handle for task that monitors webSockets
TaskHandle_t monWeb; // Handle for task that monitors HTTP 
TaskHandle_t monBalance; // Handle for task that balances robot
long cntBalance, cntLoop;    // not re-initialized, so not too useful

void flashLCD();  // from http://forum.arduino.cc/index.php?topic=42835.0
/***********************************************************************************************************
 Define motor control variables and constants
 ***********************************************************************************************************/
long pcnt; // This is used for some JSON message ping test timing
int acc_calibration_value; // Balance point of robot when balancing on its wheels (may not be vertical)
                           // To get this value, get the robot balancing, and watch the serial trace during boot up.
                           // Note that this variable is set in startWiFi() based on MAC address of ESP32.
                           // The MAC address read from the hardware selects one of these values:
int balance_37_54_f8 = 0;  // TWIPi MAC address: 30:AE:A4:37:54:F8 has this acc_calibration_value 

const volatile int speed = -1; // for initial testing of interrupt driven steppers
                               // speed = -1 enables IMU based balancing. 
                               // speed = n enables fixed forward speed interval of n
                               //         0 is brakes on
const int bot_slow = 2300; // # of interrupts between steps at slowest workable bot speed
const int bot_fast = 300; // # of interrupts between steps at fastest workable bot speed

/***********************************************************************************************************
 Define PID control variables and constants
 ***********************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Various PID settings
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// was 30, 1.2, 30 before 2018-12-10
// back to above 2018-12-11
// based on telemetry, on jan 7: 15, 0.4, 10
float pid_p_gain = 15;          // Gain setting for the P-controller (15)
float pid_i_gain = 0.4;         // Gain setting for the I-controller (1.5)
float pid_d_gain = 10;          // Gain setting for the D-controller (30)
const long usec_per_t0_int = 20; // Number of microseconds between t0 timer interrupts
const int pid_max = 400;        // Upper bound for pid values
const int pid_min = -400;       // Lower bound for pid values
const float PID_I_fade = .80;   // How much of pid_i_mem history to retain

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the GPIO pins that perform functions on DVR8825 motor controller. Changed as follows:
// Left step moved from 12 to 32
// Left dir stays the same at 14 
// Right step moved from 15 to 26
// Right dir moved from 13 to 25
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define pin_left_step 32  // GPIO 32 initiates a step on motor on robot's left side 
#define pin_left_dir 14 // GPIO 14 controls the direction of left motor
#define pin_right_step 26 // GPIO 26 initiates a step on motor on robot's right side
#define pin_right_dir 25 // GPIO 25 controls the direction of right motor

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Declaring global variables and setting some values (YABR cut and paste)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte start;    // controls whether or not we've moving wheels to attempt to balance the robot
               // initially 0, set to 1 when robot angle is close to zero, set back to 0 if offset exceeds 30 degrees
volatile int left_motor, throttle_left_motor, throttle_counter_left_motor, throttle_left_motor_memory;
volatile int right_motor, throttle_right_motor, throttle_counter_right_motor, throttle_right_motor_memory;
int receive_counter;
int gyro_pitch_data_raw, gyro_yaw_data_raw, gyro_roll_data_raw, accelerometer_data_raw;
long gyro_yaw_calibration_value, gyro_pitch_calibration_value, balance_calibration_value, gyro_roll_calibration_value;
unsigned long loop_timer;
float angle_gyro, angle_acc, angle;
float pid_error_temp, pid_i_mem, gyro_input, pid_output, pid_last_d_error;
float pid_output_left, pid_output_right;
float hold, hold1, hold2, hold3, hold4, hold5;
long mics, loop_mics, last_millis; // Measure length of balanceRobot() loop, and print occasionally
int t0_count;
long t,t_old,d;
int millis_now, i=1;
long mil, mic;
volatile long int_time;

/***********************************************************************************************************
 Define on-board LED definitions. GPIO13 is where the onboard LED is located for the HUZZAH32 board. Other 
 board's (Adafruit ESP8266 for example) the LED might be on GPIO0.
 ***********************************************************************************************************/
const int LEDPIN = 15; // GPIO pin the the onboard LED is connected to
bool LEDStatus; // Current LED status
const char LEDON[] = "ledon"; // Turn onboard LED ON
const char LEDOFF[] = "ledoff"; // Turn onboard LED OFF
static void writeLED(bool); // Define function that sets the onboard LED, accepts 0 or 1

/***********************************************************************************************************
 Define network objects and services.
 ***********************************************************************************************************/
ESP32WebServer server(80); // Define web server object listening on port 80
WiFiMulti wifiMulti; // Allows us to connect to one of a number of known Access Points
WebSocketsServer webSocket = WebSocketsServer(81); // Define Websocket server object 
static const char ssid0[] = "MN_BELL418"; // The name of a WiFi network AP to connect to
static const char ssid1[]= "MN_WORKSHOP_2.4GHz"; // The name of a WiFi network AP to connect to
static const char ssid2[] = "MN_DS_OFFICE_2.4GHz"; // The name of a WiFi network AP to connect to
static const char ssid3[] = "MN_OUTSIDE"; // The name of a WiFi network AP to connect to
static const char password[] = "5194741299"; // The password for all of the WiFi network APs
static const char ssid4[] = "borfpiggle"; // The name of Doug's WiFi network AP to connect to
static const char password4[] = "de15ab00be"; // The password for Doug's WiFi network AP

MDNSResponder mdns; // DNS Service Discovery object used for client handshaking
static const char *wsEvent[] = { "WStype_DISCONNECTED", "WStype_CONNECTED", "WStype_TEXT", "WStype_BIN"};

/***********************************************************************************************************
 Define non-device specific I2C related variables. Device specific variables like addresses are found in 
 device specific sections.
 ***********************************************************************************************************/
byte I2C_UNKNOWN = false; // Flag if unknown device found on I2C bus

/***********************************************************************************************************
 Define MPU6050 related variables
 ***********************************************************************************************************/
#define MPU6050_WHO_AM_I 0x75 // Read only register on IMU with info about the device
#define MPU_address 0x68 // MPU-6050 I2C address. Note that AD0 pin on the board cannot be left floating and 
                         // must be connected to the Arduino ground for address 0x68 or be connected to VDC 
                         // for address 0x69.
bool IMU_FOUND = false; // Flag to see if MPU found on I2C bus
int acc_x; // Read raw low and high byte to the MPU acc_x register
int acc_y; // Read raw low and high byte to the MPU acc_y register
int acc_z; // Read raw low and high byte to the MPU acc_z register
int gyro_x; // Read raw low and high byte to the MPU gyro_x register
int gyro_y; // Read raw low and high byte to the MPU gyro_y register
int gyro_z; // Read raw low and high byte to the MPU gyro_z register
int temperature; // Read raw low and high byte to the MPU temperature register
volatile bool imuReady = false; // flag used to note when imu boot sequence is complete
int IMU_warmup_start;               // used to track beginning of IMU warmup period
int IMU_warmup_interval = 11000;    // length of IMU warmup period in milliseconds

/***********************************************************************************************************
 Define LCD related variables. 
 ***********************************************************************************************************/
#define LCD_NO_MESSAGE "" // Blank message to scroll old messages off screen 
#define SCROLL 2 // Tell LCD to scroll full screen, both lines 
#define LINE1 0 // Tell LCD to display message on line 1
#define LINE2 1 // Tell LCD to display message on line 2
const byte lcdAddr = 0x38; // LCD I2C address for old LCD 
//const byte lcdAddr = 0x3F; // LCD I2C address for new LCD 
const byte lcdCols = 16; // LCD number of characters in a row
const byte lcdRows = 2; // LCD number of lines
const unsigned int scrollDelay = 500; // Miliseconds before scrolling next char
LiquidCrystal_I2C lcd(lcdAddr,lcdCols,lcdRows); // Define LCD object. Value re-assigned in WiFiStart()
byte LCD_FOUND = false; // Flag to see if LCD found on I2C bus
String LCDmsg0 = ""; // Track message displayed in line 1 of LCD
String LCDmsg1 = ""; // Track message displayed in line 2 of LCD

/***********************************************************************************************************
 Define main loop workflow related variables
 ***********************************************************************************************************/
#define JSON_DEBUG true // Flag to control JSON debug messages
#define LoopDelay 4000 // This is the target time in milliseconds that each iteration of balanceRobot() should take.
                        // this is built into angle calculations, and can't be easily changed

String sendTelemetry = "flagoff"; // Flag to control if balance telemetry data is sent 
                                  // to connected clients
bool WiFi_active = false;   // flag to say whether or not attempy to connect to a known WiFi network was successful

/***********************************************************************************************************
 Declare INDEX_HTML as a FLASH memory string containing a web page. Note that details on programming ESP32 
 PROGMEM are found here: http://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html. Details on how to 
 write a Websocket Javascript client can be found here: 
 https://www.tutorialspoint.com/websockets/websockets_send_receive_messages.htm. This is the code delivered 
 to any browser that connects to the robot. 
 ***********************************************************************************************************/

// following line is equivalent to previous use of raw literal, following technique described in 
//     https://techtutorialsx.com/2017/12/16/esp32-arduino-async-http-server-serving-a-html-page-from-flash-memory/
// ...but does not produce compiler errors

static const char INDEX_HTML[] = "<!DOCTYPE html> <html> <head> <meta charset=\"utf-8\"/> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\"> <title>SBS Mark2 Remote Home Page</title> <style>\"body{background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000;}\" </style> <script type=\"text/javascript\" > var websock; function start(){websock=new WebSocket('ws://' + window.location.hostname + ':81/'); websock.onopen=function(evt){console.log('websock open');}; websock.onclose=function(evt){console.log('websock close');}; websock.onerror=function(evt){console.log(evt);}; websock.onmessage=function(evt){console.log('[SegbotSTEP] evt=' + evt.data); var msg=JSON.parse(evt.data); console.log('[SegbotSTEP] msg.item=' + msg.item); if (msg.item==='LED'){var e=document.getElementById('ledstatus'); console.log('[SegbotSTEP] msg.value=' + msg.value); if (msg.value==='ledon'){e.style.color='red'; console.log('[SegbotSTEP] set ledstatus color to red');}else if (msg.value==='ledoff'){e.style.color='black'; console.log('[SegbotSTEP] set ledstatus color to black');}else{console.log('[SegbotSTEP] unknown LED value. evt.data=' + evt.data); item unknow}}else if (msg.item==='LCD'){var e1=document.getElementById('lcd1'); var e2=document.getElementById('lcd2'); console.log('[SegbotSTEP] update LCD line 1 with ' + msg.line1); console.log('[SegbotSTEP] update LCD line 2 with ' + msg.line2); e1.value=msg.line1; e2.value=msg.line2;}else if (msg.item==='ping'){websock.send(evt.data);}else if (msg.item==='balGraph'){console.log('[SegbotSTEP] Web client does not support graphing, ignore message');}else{console.log('[SegbotSTEP] unknown item (case sensative). evt.data=' + evt.data);}};}function ledControl(e){var msg={item: \"LED\", action: \"set\", value: e.id}; websock.send(JSON.stringify(msg)); console.log('[SegbotSTEP] sent this to server: ' + JSON.stringify(msg));}function lcdControl(e){if (e.id==='getlcd'){var x=\"get\";}else{var x=\"set\";}var l1=document.getElementById('lcd1'); var l2=document.getElementById('lcd2'); var msg={item: \"LCD\", action: x, line1: l1.value, line2: l2.value}; websock.send(JSON.stringify(msg)); console.log('[SegbotSTEP] sent this to server: ' + JSON.stringify(msg));}</script> </head> <body onload=\"javascript:start();\"> <h1>SBS Mark2 Web-Based Control Center</h1> Note: Balancing telemetry graph intentionally not included here due to complications serving up onjects such as canvasjs from the ESP32 <div id=\"ledstatus\"><b>LED</b></div><button id=\"ledon\" type=\"button\" onclick=\"ledControl(this);\">On</button> <button id=\"ledoff\" type=\"button\" onclick=\"ledControl(this);\">Off</button> <p><b>1650 LCD</b><br><input type=\"text\" id=\"lcd1\" style=\"background:GreenYellow; color:black;text-align:center;\" maxlength=\"16\"/><br><input type=\"text\" id=\"lcd2\" style=\"background:GreenYellow; color:black;text-align:center;\" maxlength=\"16\"/><br><button id=\"getlcd\" type=\"button\" onclick=\"lcdControl(this);\">Get</button> <button id=\"setlcd\" type=\"button\" onclick=\"lcdControl(this);\">Set</button> </body> </html>\n";

/***********************************************************************************************************
 This is the interrupt handler for hardware timer 0. This routine coordinates a variable with main.  
 ***********************************************************************************************************/
void IRAM_ATTR onTimer0() 
{
    portENTER_CRITICAL_ISR(&timerMux); // Prevent anyone else from updating the variable
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Left motor pulse calculations 
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    throttle_counter_left_motor ++; // Increase the throttle_counter_left_motor variable by 1 every time 
                                    // this routine is executed
    if(throttle_counter_left_motor > throttle_left_motor_memory) // If the number of loops is larger then the 
                                                                 // throttle_left_motor_memory variable
    {
        throttle_counter_left_motor = 0; // Reset the throttle_counter_left_motor variable
        throttle_left_motor_memory = throttle_left_motor; // Load the next throttle_left_motor variable
        if(throttle_left_motor_memory < 0) // If the throttle_left_motor_memory is negative
        {
            digitalWrite(pin_left_dir, LOW); // Change left wheel rotation to the reverse direction
            throttle_left_motor_memory *= -1; // negate the throttle_left_motor_memory variable
        } //if
        else digitalWrite(pin_left_dir, HIGH); // Otherwise set left wheel rotation to the forward direction
    } //if
    else if(throttle_counter_left_motor == 1)digitalWrite(pin_left_step, HIGH); // Set left motor step pin high 
                                                                                // to start a pulse for the 
                                                                                // stepper controller
    else if(throttle_counter_left_motor == 2)digitalWrite(pin_left_step, LOW);  // One interrupt later, lower 
                                                                                // the pin because the pulse 
                                                                                // only has to last for 20us 
                                                                                // Wiring of M1, M2, and M3 pins 
                                                                                // on A4988 controls step type - 
                                                                                // we default to SINGLE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // right motor pulse calculations 
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    throttle_counter_right_motor ++; // Increase the throttle_counter_right_motor variable by 1 every time 
                                     // the routine is executed
    if(throttle_counter_right_motor > throttle_right_motor_memory) // If the number of loops is larger then 
                                                                  // the throttle_right_motor_memory variable
    {
        throttle_counter_right_motor = 0; // Reset the throttle_counter_right_motor variable
        throttle_right_motor_memory = throttle_right_motor; // Load the next throttle_right_motor variable
        if(throttle_right_motor_memory < 0) // If the throttle_right_motor_memory is negative
        {
            digitalWrite(pin_right_dir, LOW); // Change right wheel rotation to the reverse direction
            throttle_right_motor_memory *= -1; // negate the throttle_right_motor_memory variable
        } //if
        else digitalWrite(pin_right_dir, HIGH); // Otherwise set right wheel rotation to the forward direction
    } //if 
    else if(throttle_counter_right_motor == 1)digitalWrite(pin_right_step, HIGH); // Set right motor step pin 
                                                                                  // high to start a pulse for 
                                                                                  // the stepper controller
    else if(throttle_counter_right_motor == 2)digitalWrite(pin_right_step, LOW); // One interrupt later, lower 
                                                                                 // the pin because the pulse 
                                                                                 // only has to last for 20us 
                                                                                 // Wiring of M1, M2, and M3 
                                                                                 // pins on A4988 controls 
                                                                                 // step type - we default to 
                                                                                 // SINGLE
    //timer0_write(ESP.getCycleCount() + t0_count -1 ); // Prime next interrupt to go off after proper interval

    t0_per_sec++ ;                          // Count one more t0 int seen in this second
    portEXIT_CRITICAL_ISR(&timerMux);       // Allow anyone else to update the variable
    cntTimer0++;
 
} //onTimer0()

/***********************************************************************************************************
 This function dumps a bunch of useful info to the terminal. This code is based on an example we found at 
 this URL: https://stackoverflow.com/questions/14143517/find-the-name-of-an-arduino-sketch-programmatically                                   
 ***********************************************************************************************************/
void display_Running_Sketch()
{                                 
    LINE_TIME("<display_Running_Sketch> Displaying basic running environment.");
    sp(" [display_Running_Sketch] Sketch Name: ");spl(__FILE__);
    sp(" [display_Running_Sketch] Sketch Version: "); spl(my_ver);
    sp(" [display_Running_Sketch] Sketch compilation date: ");sp(__DATE__);sp(" at ");spl(__TIME__);
    spdl(" [display_Running_Sketch] ESP32 SDK used: ", ESP.getSdkVersion());
} //display_Running_Sketch()

/***********************************************************************************************************
 This function controls the onboard Huzzah32 LED. Note inverted logic for Adafruit ESP8266 board does NOT 
 apply for the Feather32 board. THis logic needed to be revered as part of port to the Feather32.
 ***********************************************************************************************************/
void writeLED(bool LEDon)
{
    LINE_TIME("<writeLED> write value to LED.");
    LEDStatus = LEDon; // Track status of LED
    if (LEDon) // If request is to turn LED on
    {
        digitalWrite(LEDPIN, 1); // Set LED GPIO HIGH, which turn the LED on
        spl(" [writeLED] Set LED GPIO HIGH (turn LED on)");
    } //if
    else 
    {
        digitalWrite(LEDPIN, 0); // Set LED GPIO LOW, which turn the LED off
        spl(" [writeLED] Set LED GPIO LOW (turn LED off)");
    } //else

} //writeLED()

/***********************************************************************************************************
 This function starts and scans the I2Cone buse for attached devices. This code was taken from 
 http://playground.arduino.cc/Main/I2cScanner then updated with this 
 https://github.com/espressif/arduino-esp32/issues/977 in order toi support 2 buses. 
 The LCD should be attached to I2Cone and the MPU should be attached to I2Ctwo. SCL and SDA on both buses
 should have a 2.2K or 2.4K pull up resitor (3.3VDC). 
 ***********************************************************************************************************/
void startI2Cone()
{
    LINE_TIME("<startI2Cone> Initialize I2C bus.");
    Wire.begin(SDA1,SCL1,400000); // 400KHz, upper speed limit for ESP32 I2C
    sp(" [startI2Cone] Active bus addresses: ");
    uint8_t cnt=0;
    for(uint8_t i=0;i<128;i++)
    {
        Wire.beginTransmission(i);
        uint8_t ec=Wire.endTransmission(true);
        if(ec==0)
        {
            if(i<16)Serial.print('0');
            Serial.print(i,HEX);
            Serial.print(' ');           // print trailing space, but no dots
            cnt++; 
        } //if
//        else Serial.print("..");
//        Serial.print(' ');
//        if ((i&0x0f)==0x0f)Serial.println();
    } //for

    Serial.println();
    Serial.print(" [startI2Cone] Scan Completed, ");
    Serial.print(cnt);
    Serial.println(" I2C Devices found.");

    if(cnt==0)
    {
        Serial.print(String(" [startI2Cone] ERROR! No I2C device found on I2Cone. Expected LCD at 0x38."));
        LCD_FOUND = false;
    } //if
    else
    {
        spl(" [startI2Cone] LCD found on I2Cone as expected at 0x38.");
        LCD_FOUND = true;
    } //else

} //startI2Cone()

/***********************************************************************************************************
 This function starts and scans the I2Ctwo bus for attached devices. This code was taken from 
 http://playground.arduino.cc/Main/I2cScanner then updated with this 
 https://github.com/espressif/arduino-esp32/issues/977 in order toi support 2 buses. 
 The LCD should be attached to I2Ctwo and the MPU should be attached to I2Ctwo. SCL and SDA on both buses
 should have a 2.2K or 2.4K pull up resitor (3.3VDC). 
 ***********************************************************************************************************/
void startI2Ctwo()
{
    LINE_TIME("<startI2Ctwo> Initialize I2C bus.");
    Wire1.begin(SDA2,SCL2,400000); // 400KHz, upper speed limit for ESP32 I2C
    sp(" [startI2Ctwo] Active bus addresses: ");
    uint8_t cnt=0;
    for(uint8_t i=0;i<128;i++)
    {
        Wire1.beginTransmission(i);
        uint8_t ec=Wire1.endTransmission(true);
        if(ec==0)
        {
            if(i<16)Serial.print('0');
            Serial.print(i,HEX);
            Serial.print(' ');      // print trailing space, but no dots
            cnt++;
        } //if
//        else Serial.print("..");
//        Serial.print(' ');
//        if ((i&0x0f)==0x0f)Serial.println();
    } //for

    Serial.println();
    Serial.print(" [startI2Ctwo] Scan Completed, ");
    Serial.print(cnt);
    Serial.println(" I2C Devices found.");

    if(cnt==0)
    {
        Serial.print(" [startI2Ctwo] ERROR! No I2C device found on I2Ctwo. Expected MPU at 0x68.");
        IMU_FOUND = false;
    } //if
    else
    {
        spl(" [startI2Ctwo] LCD found on I2Ctwo as expected at 0x68.");
        IMU_FOUND = true;
    } //else

} //startI2Ctwo()

/***********************************************************************************************************
 This function initializes the Open Smart 1602 LCD Display
 ***********************************************************************************************************/
void initializeLCD()                                             
{
    LINE_TIME("<initializeLCD> Initialize LCD.");
    lcd.clear(); // Clear the LCD screen
    lcd.init(); // Initialize the LCD object 
    lcd.backlight(); // Turn on the LCD backlight
    flashLCD(); // Flash the LCD backlight
  
} //initializeLCD()                                                          

/***********************************************************************************************************
 This function sends messages you pass to it to the LCD and displays it centered.
 ***********************************************************************************************************/
void sendLCD(String LCDmsg, byte LCDline)
{
    LINE_TIME("<sendLCD> Send text to LCD.");
    byte textLen = LCDmsg.length(); // Length of message to send
    byte LCDcolumn = 0; // Starting column of message 
    if(LCDline > 1) LCDline=1; // Ensure line argument is not too large                                   
    if(LCDline < 0) LCDline=0; // Ensure line argument is not too small
    LCDcolumn=(lcdCols-textLen)/2; // Figure out starting point to center message         
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Clear line by sending blank message starting at first column, this ensures that previous messages 
    // are gone completely
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    lcd.setCursor(0,LCDline); // Set cursor to correct location (line 1 or 2)
    lcd.print("                "); // Send blank message to clear previous message 
    delay(5); // Allow time for LCD to process message
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Print actual message centered
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    lcd.setCursor(LCDcolumn,LCDline); // Set cursor to correct location (line 1 or 2)
    lcd.print(LCDmsg); // Send message to LCD 
    delay(5); // Allow time for LCD to process message
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Global variables track content of LCD line 1 & 2 to send to clients via JSON message
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(LCDline == 0) // If this is the first line of the LCD
    {
        LCDmsg0 = LCDmsg; // Track current message in line 1 global variable
    } // if
    else // If this is the second line of the LCD
    {
        LCDmsg1 = LCDmsg; // Track current message in line 2 global variable    
    } // else
   
} //sendLCD()

/***********************************************************************************************************
 This function scrolls a message from left to right on the LCD. Note that both lines of the display scroll. 
 You can send a blank message to this function to scroll the current messages displayed on both lines of 
 the LCD screen. Note that the 2nd argument passed to this function is not used if a null message if passed.
 ***********************************************************************************************************/
void scrollLCD(String LCDmsg, byte LCDline)
{
    LINE_TIME("<scrollLCD> Scrolling text on LCD.");
    byte textLen = LCDmsg.length(); // Length of message to send
    byte LCDcolumn = 0; // Starting column of message 
    if(LCDline > 1) LCDline=1; // Ensure line argument is not too large                                   
    if(LCDline < 0) LCDline=0; // Ensure line argument is not too small
    if(LCDmsg != LCD_NO_MESSAGE) // If this is not a blank message display it
    {
        lcd.setCursor(LCDcolumn,LCDline); // Set LCD cursor to correct location 
        lcd.print(LCDmsg); // Send message to LCD
    } // if 
    for (byte positionCounter = 0; positionCounter < (textLen + lcdCols); positionCounter++) 
    {
        lcd.scrollDisplayRight(); // Scroll entire row to the right
        delay(scrollDelay); // Pause between scrolls
    } // for

} //scrollLCD()

/***********************************************************************************************************
 This function flashes the LCD backlight.
 ***********************************************************************************************************/
void flashLCD()    // from http://forum.arduino.cc/index.php?topic=42835.0
{
    LINE_TIME("<flashLCD> Flashing back light of LCD.");
    for (byte cnt = 0; cnt < 4; cnt++)      // max was 10, but sped it up
    {
        lcd.backlight(); // Turn on the LCD backlight
        delay(100);
        lcd.noBacklight(); // Turn off backlight
        delay(100);
    } // for
    lcd.backlight(); // Turn on the LCD backlight
    delay(100);

} //flashLCD()
/***********************************************************************************************************
 This function handles hexdumps that come in over a websocket.
 ***********************************************************************************************************/
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) 
{
    LINE_TIME("<hexdump> HEX dump from websocket.");
    const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[hexdump] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) 
    {
	    if(i % cols == 0) 
        {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		} // if
		Serial.printf("%02X ", *src);
		src++;
	} // for
	Serial.println("\n");

} //hexdump()

/***********************************************************************************************************
 This function connects to the local Access Point and then starts up a a socket server to listen for client 
 connections. This code is based on an exmaple we found at this URL: 
 https://stackoverflow.com/questions/14143517/find-the-name-of-an-arduino-sketch-programmatically      
 STATUS return codes: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/include/wl_definitions.h    
 typedef enum {
    WL_NO_SHIELD        = 255 (for compatibility with WiFi Shield library)
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6
} wl_status_t;  
 ***********************************************************************************************************/
void startWiFi()
{
    LINE_TIME("<startWiFi> Scanning/connecting to strongest known AP signal.");
    //WiFi.mode();
    wifiMulti.addAP(ssid0, password); // Add Wi-Fi AP we may be able to connect to
    wifiMulti.addAP(ssid1, password); // Add Wi-Fi AP we may be able to connect to
    wifiMulti.addAP(ssid2, password); // Add Wi-Fi AP we may be able to connect to
    wifiMulti.addAP(ssid3, password); // Add Wi-Fi AP we may be able to connect to
    wifiMulti.addAP(ssid4, password4); // Add Wi-Fi AP we may be able to connect to - Doug's WiFi
    spl(" [startWiFi] Connecting Wifi"); 
    WiFi_active = false; // initial assumption is that we aren't able to connect to WiFi
    sp(" [startWifi] initial wifiMulti.run() returned ");
    sp(wifiMulti.run());
    for ( int cnt=1; cnt<20; cnt -- )
    {   if(WiFi_active == false) 
        {   if (wifiMulti.run() == WL_CONNECTED)
            {   WiFi_active = true ;           // note that we did manage to connect to WiFi                                                               
                sp(" [startWiFi] Connected to Access Point ");                                        
                spl(WiFi.SSID()); // Name of AP to which the ESP32 is connected to
                sp(" [startWiFi] IP address: ");                                   
                spl(WiFi.localIP());  // IP address assigned to ESP32 by AP
                sp(" [startWiFi] MAC address: ");                                   
                spl(WiFi.macAddress()); // IP address of the ESP32
            } // if (wifiMulti...
            else
            {   delay(20); // Wait a little before trying again // was 100 msec, but sped it up
            }
        }  // if Wifi_active...      
    }  // for cnt...
// the boolean variable WiFi_active now indicates whether or not we connected to a network

// do the accelerometer calibration even if we can't connect to a WiFi network    
    if(WiFi.macAddress()=="30:AE:A4:37:54:F8")          // Is that you, TWIPi ?4
    {   spl(" [startWiFi] configuring for bot TWIPi ");
        acc_calibration_value = balance_37_54_f8 ;      // this does the bot-specific balance calibration
    }  //replicate this if statement for other bots

 // TWIPi MAC address: 30:AE:A4:37:54:F8 
  
} //startWiFi()

/*************************************************************************************************************************************
 This function Start the mDNS service which handles mapping IP ports and provided services to connecting clients according to
 https://media.readthedocs.org/pdf/arduino-esp8266/docs_to_readthedocs/arduino-esp8266.pdf, mDNS implements a simple DNS server that 
 can be used in both STA and AP modes.  The DNS server currently supports only one domain (for all other domains it will reply with 
 NXDOMAIN or custom status code).  With it, clients can open a web server running on ESP8266 using a domain name, not an IP address.
 *************************************************************************************************************************************/
void startDNS()
{
    LINE_TIME("<startDNS> Start the DNS service.");
    if (mdns.begin("esp32")) // Start mDNS service
    {
        sp(" [startDNS] MDNS responder started. ");
        sp("Adding HTTP service to port 80 ");
        mdns.addService("http", "tcp", 80); // If successfully started add web service on port 80
        spl("and WS service to port 81");                    
        mdns.addService("ws", "tcp", 81); // If successfully started add websocket service on port 81
        sp(" [startDNS] Clients can connect to either ");
        sp("http:\057\057esp32.local or http:\057\057");  // put the slashes in as ASCII to avoid comment interpretation
        spl(WiFi.localIP()); // Send message to console advising URI for clients to use

   } //if
   else // If mDNS service fails to start
   {
      spl(" [startDNS] mdns.begin failed"); // Issue message
   } //else
  
} //startDNS()

/***********************************************************************************************************
 This function handles the web server service for valid HTTP GET and POST requests for the root (home) page. 
 The rely simply sends the web page defined in the EEPROM (non-volitile memory of the ESP8266 chip), pointed 
 to by INDEX_HTML[] 
 ***********************************************************************************************************/
volatile void handleRoot()
{
    LINE_TIME("<handleRoot> handleRoot web service event triggered.");
    server.send_P(200, "text/html", INDEX_HTML); // Send the HTML page defined in INDEX_HTML
    spl(" [handleRoot] Home page  requested via HTTP request on port 80. Sent EEPROM defined document page to client");                              
   
} //handleRoot()

/***********************************************************************************************************
 This function handles the web server service for invalid HTTP GET and POST requests. If the requested file 
 or page doesn't exist, return a 404 not found error to the client 
 ***********************************************************************************************************/
volatile void handleNotFound()
{
    LINE_TIME("<handleNotFound> File not found web service event triggered.");
    String message = "File Not Found\n\n"; // Build string with 404 file not found message
        message += "URI: ";
        message += server.uri();
        message += "\nMethod: ";
        message += (server.method() == HTTP_GET)?"GET":"POST";
        message += "\nArguments: ";
        message += server.args();
        message += "\n";
    for (uint8_t i=0; i<server.args(); i++) // Append actual HTTP error elements to message
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; 
    } //for
    server.send(404, "text/plain", message); // Send 404 error message back to client
    spl(" [handleNotFound] Sent 404 error to client");                              

} //handleNotFound()

/***********************************************************************************************************
 This function starts the web service which sends HTML and Javascript client code to remote Web browser clients
 ***********************************************************************************************************/
void startWebServer()
{
    LINE_TIME("<startWebServer> Starting Webserver service.");
    server.on("/", handleRoot); // Attach function for handling root page (/)
    server.on ( "/inline", []() // Attach simple inline page to test web server
    {
        server.send ( 200, "text/plain", "this works as well" );
    } ); //server.on(/inline)
    server.onNotFound(handleNotFound); // Attach function for handling unknown page
    server.begin(); // Start web service
    spl(" [startWebServer] Started web server");                               

} //startWebServer()

/***********************************************************************************************************
 This function initializes a websocket service
 ***********************************************************************************************************/
void startWebsocket()
{
    LINE_TIME("<startWebsocket> Starting Websocket service.");
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);    

} //startWebsocket()

/***********************************************************************************************************
 This function handles websocket events
 ***********************************************************************************************************/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) 
{
    LINE_TIME("<webSocketEvent> Websocket event detected.");
    if(JSON_DEBUG) // If JSON message debug flag set  
    {
        spf(" [webSocketEvent] Event detected: "); // Show event details in terminal   
        spf("num = %d, type = %d (", num, type); // Show event details in terminal
        sp(wsEvent[type-1]);spl(")"); // Show event details in terminal
    } //if

    switch(type) // Handle each event by type
    {
        case WStype_DISCONNECTED: // Client disconnect event
            spf(" [webSocketEvent] Client NUM [%u] disconnected\r\n", num);
            telClientConnected = false;
            writeLED(telClientConnected);
            break;
        case WStype_CONNECTED: // Client connect event
        {
            IPAddress ip = webSocket.remoteIP(num);
            telClientConnected = true;
            writeLED(telClientConnected);
            spf(" [webSocketEvent] [%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
//            sendClientLEDState(num); // Send new client state of onboard LED
//            sendClientLCDState(num); // Send new client text displayed on LCD
//            sendClientVariableValue(num,"PID"); // Send new client PID tuning values
//            sendClientPing(num); // Send new client a ping message (test only)
        } //case
            break;                                                   
        case WStype_TEXT: // Client sent text event
            if(JSON_DEBUG) // If JSON message debug flag set  
            {
                spf(" [webSocketEvent] Client NUM: [%u], sent TEXT: %s\r\n", num, payload);
            } //if
            process_Client_JSON_msg(num, type, payload, length); // Process recieved message using JSON library
            break;
        case WStype_BIN: // Client sent binary data event
            spf(" [webSocketEvent] [%u] get binary length: %u\r\n", num, length);
            hexdump(payload, length); // Dump ESP8266 memory in hex to the console
            webSocket.sendBIN(num, payload, length); // echo data back to browser
            break;
        default: // Unknown websocket event
            spf(" [webSocketEvent] Invalid WStype [%d]\r\n", type);
            break;
    } //switch()

} //webSocketEvent()

/*************************************************************************************************************************************
 This function processes incoming client messages. Messages are in JSON format. This code is based on
 https://techtutorialsx.com/2016/07/30/esp8266-parsing-json/
 Primer on JSON messaging format: http://www.json.org/
 About the JSON message format used. All messages have the first value:
    item     [variable/property name] - e.g. LED or LCD
 After this the JSON message format changes depending upon the value of ITEM. Each use case is detailed below:   

 1. ITEM = LED. Message format is as follows:
    action   [set,get]
    value    [ledon,ledoff]

 2. ITEM = LCD. Message format is as follows:
    action   [set,get]
    line1    [message]
    line2    [message]

 3. ITEM = ping. Message format is as follows:
    line     [32 bytes of data]

 4. ITEM = BAL-TEL. Message format is as follows:
    action   [set,get]
    value    [flagon,flagoff]

 5. ITEM = PID. Message format is as follows:
    action   [set,get]
    gain1    [pGain,pid_p_gain]
    gain2    [iGain,pid_i_gain]
    gain3    [dGain,pid_d_gain]

 6. ITEM = balGraph. Message format is as follows:
    action   [set,get]
    angle    [calculated value]. See sendClientBalanceGraph()
    pid      [calculated value]. See sendClientBalanceGraph()
    gspeed   [calculated value]. See sendClientBalanceGraph()
    pmem     [calculated value]. See sendClientBalanceGraph()
    
 Handy additional notes. How to handle data types: 
    Most of the time, you can rely on the implicit casts. In other case, you can do root["time"].as<long>();
 See also:
    The website arduinojson.org contains the documentation for all the functions used. It also includes an FAQ that will help you 
    solve any deserialization problem. Please check it out at: https://arduinojson.org/. The book "Mastering ArduinoJson" contains a 
    tutorial on deserialization. It begins with a simple example, like the one above, and then adds more features like deserializing 
    directly from a file or an HTTP request. Please check it out at: https://arduinojson.org/book/
 *************************************************************************************************************************************/
void process_Client_JSON_msg(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
   String payload_str = String((char*) payload);
   StaticJsonBuffer<500> jsonBuffer;                                         // Use arduinojson.org/assistant to compute the capacity
   JsonObject& root = jsonBuffer.parseObject(payload_str);                   // Create JSON object tree with reference handle "root"
   if (!root.success())                                                      // Test if parsing succeeds 
   {
      sp(" [process_Client_JSON_msg] WARNING, parseObject() failed on message from client [");
      sp(num);spl("]. Message ignored");
      return;
   } //if
   String item = root["item"];                                               // This is the item that the client is interested in
   if(JSON_DEBUG)                                                            // If JSON message debug flag set  
   {
      sp(" [process_Client_JSON_msg] message from client regarding item: ");sp(item);
   } //if
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Message handling for LED related commands
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   if(item == "LED")                                                         // Check if client is interested in the LED
   {
      spf(" [process_Client_JSON_msg] Client NUM [%u] messaging about LED\r\n", num);
      String action = root["action"];                                        // This is what the client wants to do with the LED
      String value = root["value"];                                          // This is the value detail for the LED
      if (action == "set")                                                   // If client wants to SET LED value
      {
         if (value == LEDON)                                                 // If client wants LEDON
         {
            if(JSON_DEBUG)                                                   // If JSON message debug flag set  
            {
               sp(" [process_Client_JSON_msg] Client requested LED on: ");
            } //if
            writeLED(true);                                                  // Call function to turn GPIO LED off
            sendClientLEDState(99);                                          // Request broadcast (99) of the current value of the LED
         } //if
         else if(value == LEDOFF)                                            // If client wants LEDOFF 
         {
            if(JSON_DEBUG)                                                   // If JSON message debug flag set  
            {
               sp(" [process_Client_JSON_msg] Client requested LED off: ");
            } //if
            writeLED(false);                                                 // Call function to turn GPIO LED on
            sendClientLEDState(99);                                          // Request broadcast (99) of the current value of the LED
         } //else if
         else                                                                // If client wants any other value it is invalid
         {
            sp(" [process_Client_JSON_msg] Client [");sp(num);sp("] has sent unrecognized VALUE [");sp(value);spl("] for LED. Message ignored");
         } //else       
      } //if ACTION
      else if (root["action"] == "get")                                      // If client wants to GET LED value
      {
         spf(" [process_Client_JSON_msg] Client [%u] requesting current LED value\r\n", num);
         sendClientLEDState(num);                                            // Send client the current value of the LED
      } //else if ACTION
      else                                                                   // Any other action the client wants for LED is invalid
      {
         sp(" [process_Client_JSON_msg] Client [");sp(num);sp("] has sent unrecognized ACTION [");sp(action);spl("]. Message ignored");
      } //else ACTION
   } //if
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Message handling for LCD related commands
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(item == "LCD")                                                    // Check if client is interested in the LCD
   {
      spf(" [process_Client_JSON_msg] Client NUM [%u] messaging about LCD\r\n", num);
      String action = root["action"];                                        // This is what the client wants to do with the LCD
      String line1 = root["line1"];                                          // Message text for top line of LCD
      String line2 = root["line2"];                                          // Message text for bottom line of LCD
      if (action == "set")                                                   // If client wants to SET LCD value
      {
         spf(" [process_Client_JSON_msg] Client [%u] wants to set LCD message\r\n", num);         
         lcd.clear();                                                        // Clear the LCD screen
         sendLCD(line1, 0);                                                  // Update line 1 of LCD with client message
         sendLCD(line2, 1);                                                  // Update line 2 of LCD with client message
         sendClientLCDState(99);                                             // Send client the current value of the LED        
      } // if
      else if(action == "get")                                               // If client wants to GET LCD value
      {
         spf(" [process_Client_JSON_msg] Client [%u] requesting current LCD message\r\n", num);
         sendClientLCDState(num);                                            // Send client the current value of the LED        
      } //else if
      else                                                                   // If ACTION is not one of the above values
      {
         sp(" [process_Client_JSON_msg] Client [");sp(num);sp("] has sent unrecognized ACTION [");sp(action);spl("]. Message ignored");        
      } //else
   } //else if ITEM LCD
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Message handling for PING related commands. Server echoes messages back to client. Useful when developing a new client. NOTE:
   // Client MUST send JSON formatted message with at least one element defined (item). When building a new client you can cause the 
   // server to send the PING JSON message to the client by uncommenting the call to the ping function in the start() function.
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(item == "ping")                                                   // Check if client is sending ping test message
   {
      spf(" [process_Client_JSON_msg] Client NUM [%u] sent ping message\r\n", num);
      sendClientPing(num);                                                   // Send client the current value of the LED        
   } //else if ITEM ping
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Message handling for balance telemetry related commands
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(item == "BAL-TEL")                                                // Check if client is sending telemetry config message
   {
      spf(" [process_Client_JSON_msg] Client NUM [%u] messaging about telemetry data\r\n", num);
      String action = root["action"];                                        // This is what the client wants to do with the flag
      String value = root["value"];                                          // This is the value detail for the flag
      if (action == "set")                                                   // If client wants to SET telemetry flag value
      {
         spf(" [process_Client_JSON_msg] Client [%u] wants to set telemetry flag\r\n", num);   
         if(value == "flagon")                                               // If client wants to recieve telemetry data
         {
            sp(" [process_Client_JSON_msg] Client [");sp(num);sp("] has requested telemetry data be sent. Setting flag ON"); 
            sendTelemetry = "flagon";       
         } //if
         else                                                                // Any other value other then ON turns off telemetry data
         {
            sp(" [process_Client_JSON_msg] Client [");sp(num);sp("] has requested telemetry data be sent. Setting flag OFF"); 
            sendTelemetry = "flagoff";               
         } //else if               
      } // if
      else if(action == "get")                                               // If client wants to GET telemetry flag value
      {
         spf(" [process_Client_JSON_msg] Client [%u] requesting current telemetry flag value message\r\n", num);
         sendClientVariableValue(num,"sendTelemetry");                       // Send client the current value of the BAL-TEL flag        
      } //else if
      else                                                                   // If ACTION is not one of the above values
      {
         sp(" [process_Client_JSON_msg] Client [");sp(num);sp("] has sent unrecognized ACTION [");sp(action);spl("]. Message ignored");        
      } //else
   } //else if ITEM BAL-TEL
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Message handling for balance telemetry related commands
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else if(item == "PID") // Check if client is sending PID config message
   {
        spf(" [process_Client_JSON_msg] Client NUM [%u] messaging about PID values\r\n", num);
        String action = root["action"]; // This is what the client wants to do with the flag
        if (action == "set") // If client wants to SET PID values
        {
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // am: For now just log what would be done without actually changing the values. Need some logic around verifying values.
   // Example of how to convert string (JSON message format) to float using toFloat() below.
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
   String s_val1 = "123.456789";
   float f;
   Serial.print("Taking String with 3 digit and 6 decimal places and putting in float: ");
// suspect that in the following code, the 6 should be the second argument of toFloat, not Serial.println   
   f = s_val1.toFloat();
   Serial.println(f,6);  // the number in the second argument tells how many digits of precision to use
*/

         spf(" [process_Client_JSON_msg] Client [%u] wants to set the following PID variables:\r\n", num);
//         spf("pid_p_gain = [%s]",root["pGain"]);                             // Assign value sent from cleint for P gain
//         spf("pid_i_gain = [%s]",root["iGain"]);                             // Assign value sent from cleint for I gain
//         spf("pid_d_gain = [%s]",root["dGain"]);                             // Assign value sent from cleint for D gain
         String pGain = root["pGain"];                                         // Assign value sent from client for P gain
         String iGain = root["iGain"];                                         // Assign value sent from client for I gain
         String dGain = root["dGain"];                                         // Assign value sent from client for D gain
           spdl("pid_p_gain = ",pGain);                                        // just display them for now
           spdl("pid_i_gain = ",iGain);
           spdl("pid_d_gain = ",dGain);
      } //if
      else if(action == "get")                                               // If client wants to GET PID values
      {
         spf(" [process_Client_JSON_msg] Client [%u] requesting current PID variable values\r\n", num);
         sendClientVariableValue(num,"sendPID");                            // Send client the current value of the PID variables        
      } //else if
      else                                                                   // If ACTION is not one of the above values
      {
         sp(" [process_Client_JSON_msg] Client [");sp(num);sp("] has sent unrecognized ACTION [");sp(action);spl("]. Message ignored");        
      } //else
    
   } //else if ITEM PID
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // If the item field has not been accounted for in the IF/ELSE structure here then handle as an unknown command
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   else                                                                      // Unknown item being referenced by client
   {
      sp(" [process_Client_JSON_msg] Client NUM [");sp(num);sp("] messaging about unknown ITEM [");sp(item);spl("]. Message ignored");
   } //else ITEM UNKNOWN

} //process_Client_JSON_msg()

/*************************************************************************************************************************************
 This function is used to test round trip timing of messages to the client.    
 *************************************************************************************************************************************/
void sendClientPing(uint8_t num)
{
   pcnt++;
   String msg = "";                                                          // String to hold JSON message to be transmitted
   StaticJsonBuffer<200> jsonBuffer;                                         // Memory pool for JSON object tree
                                                                             // Use arduinojson.org/assistant to compute the capacity
   JsonObject& root = jsonBuffer.createObject();                             // Create the root of the object tree 
   root["item"] = "ping";                                                    // Element 1 of JSON message
   root["line"] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";                        // Element 2 of JSON message
   root.printTo(msg);                                                        // Convert JSON object tree to string
   if(pcnt%1000==0) 
   {
       sp(" [sendClientPing] time after 1000 pings = ");spl(millis());
   } //if
   webSocket.sendTXT(num, msg);                                              // Send JSON message to server (robot)

} //sendClientPing()

/*************************************************************************************************************************************
 This function sends the current state of the onboard LED a specific client (or all clients if argumwnt is 99).    
 *************************************************************************************************************************************/
void sendClientLEDState(uint8_t num)
{
   String msg = "";                                                          // String to hold JSON message to be transmitted
   StaticJsonBuffer<200> jsonBuffer;                                         // Memory pool for JSON object tree
                                                                             // Use arduinojson.org/assistant to compute the capacity
   JsonObject& root = jsonBuffer.createObject();                             // Create the root of the object tree 
   root["item"] = "LED";                                                     // Element 1 of JSON message
   root["action"] = "set";                                                   // Element 2 of JSON message
   if (LEDStatus)                                                            // Check flag that tracks current state of the onboard LED  
   {
      root["value"] = "ledon";                                               // Element 3 of JSON message
   } //if
   else                                                                      // If client wants to turn LED off
   {
      root["value"] = "ledoff";                                              // Element 3 of JSON message
   } //else
   root.printTo(msg);                                                        // Convert JSON object tree to string
   if(num==99)                                                               // If client num is 99 we want to multicast 
   {
      sp(" [sendClientLEDState] broadcast this to all clients: ");spl(msg);
      webSocket.broadcastTXT(msg);                                           // Send payload data to all connected clients
   } //if
   else                                                                      // Otherwise we unicast to a specific client
   {
      sp(" [sendClientLEDState] unicast this to [");sp(num);sp("]: ");spl(msg);
      webSocket.sendTXT(num, msg);                                           // Send JSON message to server (robot)
   } //else

} //sendClientLEDState()

/*************************************************************************************************************************************
 This function sends the current state of the LCD to a specific client (or all clients if argumwnt is 99)
 *************************************************************************************************************************************/
void sendClientLCDState(uint8_t num)
{
   String msg = "";                                                          // String to hold JSON message to be transmitted
   StaticJsonBuffer<200> jsonBuffer;                                         // Memory pool for JSON object tree
                                                                             // Use arduinojson.org/assistant to compute the capacity
   JsonObject& root = jsonBuffer.createObject();                             // Create the root of the object tree 
   root["item"] = "LCD";                                                     // Element 1 of JSON message
   root["action"] = "set";                                                   // Element 2 of JSON message
   root["line1"] = LCDmsg0;                                                  // Element 3 of JSON message
   root["line2"] = LCDmsg1;                                                  // Element 4 of JSON message    
   root.printTo(msg);                                                        // Convert JSON object tree to string
   if(num==99)                                                               // If client num is 99 we want to multicast 
   {
      sp(" [sendClientLCDState] broadcast this to all clients: ");spl(msg);
      webSocket.broadcastTXT(msg);                                           // Send payload data to all connected clients
   } //if
   else                                                                      // Otherwise we unicast to a specific client
   {
      sp(" [sendClientLCDState] unicast this to [");sp(num);sp("]: ");spl(msg);
      webSocket.sendTXT(num, msg);                                           // Send JSON message to server (robot)
   } //else

} //sendClientLCDState()

/*************************************************************************************************************************************
 THis function sends an array of characters to the websocket
 *************************************************************************************************************************************/
void reportTel(String msg)
{
    if(telClientConnected)
    {
        webSocket.broadcastTXT(msg); // Send payload data to all connected clients
    } //if
    else
    {
        //Serial.println(" [reportTel] Cannot send telemetry, no connected client");
    } //else
} //reportTel()

/*************************************************************************************************************************************
 This function calculates data that can be used to assess how well the robot is balancing and sends data to the client in order to 
 graph balance performance. This is a key tool for PID tuning. Expnation of the data calculated is as follows:
 
 num = client wesocket ID
 a1 = Bot variable angle_gyro
 a2 = pid_output_left, the value from PID formula calculation
 a3 = # of clock interrupts between steps
 a4 = Bot variable pid_i_mem, the "I" part of PID

 About b3 (ground speed) logic below:
 Spreadsheet formula is =IF(D9=0,0,ROUND($F$1 * (1000000/(D9*20))*3.1415926*4/200+25,1))
 There are 2 cases (avoiding a division by zero by treating them separately)
    if motor is stopped, i.e. left_motor is zero, then ground speed is zero. easy.
    if motor is stepping after the specified number of clock interrupts...
       speed = rotations per second> * <distance per rotation>
       graphable_speed = scale_factor * <steps per second> * <distance per step>
       = 100 * < (usec in a second) / (usec for a step) > * < ( wheel circumference ) * (circle fraction per step)
       = 100 * < ( 1,000,000 ) / left_motor * 20 usec per interrupt) > * < ( pi * diameter ) * ( 1/200) >
       = 100 * < ( 1,000,000 ) / left_motor * 20 usec per interrupt) > * < ( 3.1415... * 4 inches ) * ( 1/200) >
 *************************************************************************************************************************************/
void sendClientBalanceGraph(uint8_t num,float a1, float a2, float a3, float a4)
{
   String msg = "";                                                          // String to hold JSON message to be transmitted
   StaticJsonBuffer<200> jsonBuffer;                                         // Memory pool for JSON object tree
                                                                             // Use arduinojson.org/assistant to compute the capacity
   JsonObject& root = jsonBuffer.createObject();                             // Create the root of the object tree 
   float b1;                                                                 // Robot's current forward/backward angle, in degrees, 
                                                                             // scaled by 100 to make it easily visible in graph 
   float b2;                                                                 // PID from bot's calculations
   float b3;                                                                 // Robot ground speed 
   float b4;                                                                 // PID from bot's calculations

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Doug's calculations 
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   b1 = a1 *100;                                                             // Robot's current forward/backward angle, in degrees, 
                                                                             // scaled by 100 to make it easily visible in graph 
   b2 = a2;                                                                  // PID from bot's calculations. No further processing needed
   if ( a3 == 0 )                                                            // If ground speed is 0 
   { 
      b3 = 0; 
   } //if
   else                                                                      // If motor is stepping after the specified number of clock interrupts
   { 
      b3 = 100 * ( ( 1000000 / (a3 * 20) ) * ( (3.1415926 * 4 ) / 200) ) ;
   } //else 
   b4 = a4;                                                                  // PID from bot's calculations. No further processing needed

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Build JSON message. Note that floating values are converted to string and get an 'f' appended to them. This is done because our
   // JSON library cannot handle floating point variables. All floating point variables are given 4 digits of precision via the 
   // second argument of the String() function.
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   root["item"] = "balGraph";                                                // Element 1 of JSON message is type of JSON message
   root["angle"] = String(b1, 4)+"f";                                        // Element 2 of JSON message is angle of robot times 100
   root["pid"] = String(b2, 4)+"f";                                          // Element 3 of JSON message is PID value
   root["gspeed"] = String(b3, 4)+"f";                                       // Element 4 of JSON message is calculated ground speed
   root["pmem"] = String(b4, 4)+"f";                                         // Element 5 of JSON message is PID memory, the "I" part of PID
   root.printTo(msg);                                                        // Convert JSON object tree to string

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Send JSON message to specified target client(s)
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   if(num==99)                                                               // If client num is 99 we want to multicast 
   {
      sp(" [sendClientLEDState] broadcast this to all clients: ");spl(msg);
      webSocket.broadcastTXT(msg);                                           // Send payload data to all connected clients
   } //if
   else                                                                      // Otherwise we unicast to a specific client
   {
      sp(" [sendClientLEDState] unicast this to [");sp(num);sp("]: ");spl(msg);
      webSocket.sendTXT(num, msg);                                           // Send JSON message to server (robot)
   } //else
 
} //sendClientBalanceGraph()

/*************************************************************************************************************************************
 This function sends the client the current value of a variable
 *************************************************************************************************************************************/
void sendClientVariableValue(uint8_t num, String variable)
{
   String msg = "";                                                          // String to hold JSON message to be transmitted
   StaticJsonBuffer<200> jsonBuffer;                                         // Memory pool for JSON object tree
                                                                             // Use arduinojson.org/assistant to compute the capacity
   JsonObject& root = jsonBuffer.createObject();                             // Create the root of the object tree 

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Build JSON message. Note that floating values are converted to string and get an 'f' appended to them. This is done because our
   // JSON library cannot handle floating point variables. 
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   if(variable == "sendTelemetry")                                           // Send client the current value of the telemetry flag  
   {
      root["item"] = "BAL-TEL";                                              // Element 1 of JSON message
      root["action"] = "get";                                                // Element 2 of JSON message
      root["value"] = sendTelemetry;                                         // Element 3 of JSON message (is type Boolean)     
   } //if
   else if(variable == "PID")                                                // Send client the PID tuning data
   {
      spl(" [sendClientVariableValue] PID values are float, which cannot be sent via JSON message, so we convert them to Strings");
      sp(" [sendClientVariableValue] pid_p_gain = ");spl(pid_p_gain,4);       // Print float to 4 places of precision in console
      sp(" [sendClientVariableValue] pid_i_gain = ");spl(pid_i_gain,4);       // Print float to 4 places of precision in console
      sp(" [sendClientVariableValue] pid_d_gain = ");spl(pid_d_gain,4);       // Print float to 4 places of precision in console
      root["item"] = "PID";                                                  // Element 1 of JSON message
      root["action"] = "get";                                                // Element 2 of JSON message
      root["pGain"] = String(pid_p_gain, 4)+"f";                             // Element 3 of JSON message is pid_p_gain with 4 digits of precision and an f appended
      root["iGain"] = String(pid_i_gain, 4)+"f";                             // Element 4 of JSON message is pid_p_gain with 4 digits of precision and an f appended
      root["dGain"] = String(pid_d_gain, 4)+"f";                             // Element 5 of JSON message is pid_p_gain with 4 digits of precision and an f appended
   } //else if
   else                                                                      // If the variable specified is not coded for
   {
      root["item"] = "unknownVariable";                                      // Element 1 of JSON message
      root["action"] = "get";                                                // Element 2 of JSON message
      root["value"] = "server bug: see sendClientVariableValue()";           // Element 3 of JSON message (is type String)         
   } //else
   root.printTo(msg);                                                        // Convert JSON object tree to string

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Send JSON message to specified target client(s)
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   if(num==99)                                                               // If client num is 99 we want to multicast 
   {
      sp(" [sendClientLEDState] broadcast this to all clients: ");spl(msg);
      webSocket.broadcastTXT(msg);                                           // Send payload data to all connected clients
   } //if
   else                                                                      // Otherwise we unicast to a specific client
   {
      sp(" [sendClientLEDState] unicast this to [");sp(num);sp("]: ");spl(msg);
      webSocket.sendTXT(num, msg);                                           // Send JSON message to server (robot)
   } //else
   
} //sendClientVariableValue()

/***********************************************************************************************************
 This function returns a String version of the local IP address
 ***********************************************************************************************************/
String ipToString(IPAddress ip)
{
    LINE_TIME("<ipToString> Converting IP address to String.");
    String s="";
    for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
    return s;

} //ipToString()

/***********************************************************************************************************
 This function checks to see if the IMU is at the expected address on the I2C bus. If it is, we then initialize 
 the IMU, and note the start time for the IMU warmup period. Later in setup(), after we do some network stuff in parallel,
 the initializeIMU_2 routine makes time if necessary to ensure we've allowed the warmup period of 
 IMU_warmup_interval (in milliseconds) since IMU_warmup_start (which is a millis() timesstamp). After that, 
 it completes the IMU initialization and calibration.
 
  Return codes for speaking to the IMU at endTransmission are as follows:
 0:success
 1:data too long to fit in transmit buffer
 2:received NACK on transmit of address
 3:received NACK on transmit of data
 4:other error
 ***********************************************************************************************************/

void initializeIMU_1()
{
    byte error, lowByte;
    String tmsg;

    LINE_TIME("<initializeIMU_1> Initializing the MPU6050 IMU part 1.");
    sp(" [initializeIMU_1]: Checking to see if the IMU is found on I2Ctwo at expected I2C address of 0x");
    spl(MPU_address,HEX);
    Wire1.beginTransmission(MPU_address);
    error = Wire1.endTransmission(true);
    if (error == 0)
    {
        sp(" [initializeIMU_1] I2C device found on I2Ctwo at address 0x");
        spl(MPU_address,HEX);
        spl(" [initializeIMU_1] The IMU MPU-6050 found at expected address");
        Wire1.beginTransmission(MPU_address);
        Wire1.write(MPU6050_WHO_AM_I);
        Wire1.endTransmission();
        spl(" [initializeIMU-1] Send Who am I request to IMU...");
        Wire1.requestFrom(MPU_address, 1);
        while(Wire1.available() < 1); // Wait for reply from IMU slave on I2C bus                                     
        lowByte = Wire1.read();
        if(lowByte != MPU_address)   // if there was an error...
        {
           sp(" [initializeIMU_2] Wrong Who Am I response: 0x");
           if (lowByte<16)Serial.print("0");
           spl(lowByte, HEX);
           spl(" [initializeIMU_2] Initialization of IMU failed");    
        }
        else
        {    
            sp(" [initializeIMU_1] Who Am I response is ok: 0x");
            spl(lowByte, HEX);        
            spl(" [initializeIMU_1] Set up the Gyro registers in the IMU");
            set_gyro_registers();
            spl(" [initializeIMU_1] Gyro started and configured");
            spl(" [initializeIMU_1] Waiting to allow MPU to settle down");
        } //if(lowByte)...

        IMU_warmup_start = millis() ; // remember when the IMU warmup started for initializeIMU_2, called later
                                      // just fall out of initializeIMU_1, & wait for InitializeIMU_2 to be called
    }   // if(error)...

    else   // if IMU didn't respond at expected I2C address
    {
        sp(" [initializeIMU_2] MPU-6050 query returned error code ");
        spl(error);
        spl(" [initializeIMU_2] ERROR! Robot will not be able to balance");
    } //else


//  The rest of the IMU initialization is done in routine initializeIMU_2, which follows
} //initializeIMU_1()

//  The second part of the IMU initialization is done in the following routine initializeIMU_2, which
//  is called a while after initializeIMU_1, from setup(), after some network setup is done in parallel.
//  The variable IMU_warmup_start and IMU_warmup_interval are used to ensure the IMU gets a long enough warmup,
//  but lets other routines run in the meantime.  

void initializeIMU_2()
{
    int receive_counter;
    int temp,tcnt1, tcnt2;
    String tmsg;
    
    LINE_TIME("<initializeIMU_2> Initializing the MPU6050 IMU part 2.");
    sp(" [initializeIMU_2]: Continuing IMU warmup and calibration for address ");
    spl(MPU_address,HEX);
    temp = millis() - IMU_warmup_start;         // this is how many msec IMU has been warming up
    if( temp < IMU_warmup_interval)             // if there's still time left in the warmup      
    {   for(int x = IMU_warmup_interval - temp; x > 0; x--)   // then kill time until warmup's done
        {  if( x == (floor(x / 1000)) * 1000)          // if we're at a one second multiple...
           {  tmsg = " [initializeIMU_2] Delay countdown in msec: " + String(x);
              spl(tmsg);                        // display time to go on console in msec
           } // if 
           delay(1);                            // just passing time while IMU warms up
        } //for
    }  //if
            read_mpu_6050_data(); // Read MPU registers                
            spl(" [initializeIMU_2] Create Gyro pitch and yaw offset values by averaging 500 sensor readings...");
            tcnt1 = 0; tcnt2 = 10;
            for(receive_counter = 0; receive_counter < 500; receive_counter++) // Create 500 loops
            {
                tcnt1 ++;
                if(tcnt1 > 50)
                {
                    tcnt1 = 0;
                    tcnt2 --;
                    tmsg = " [initializeIMU_2] Gyro calc countdown: " + String(tcnt2);
                    spl(tmsg);              
                } //if

                Wire1.beginTransmission(MPU_address); // Start communication with the gyro
//                Wire1.write(0x45); // Start reading the GYRO Y and Z registers
                Wire1.write(0x43); // Start reading the GYRO X and Y registers
                Wire1.endTransmission(); // End the transmission
                Wire1.requestFrom(MPU_address, 4); // Request 2 bytes from the gyro
                temp = Wire1.read()<<8|Wire1.read(); // Combine the two bytes to make one integer, that could be negative
                if(temp > 32767) temp = temp - 65536; // if it's really a negative number, fix it
//                gyro_pitch_calibration_value += temp; // 16 bit Y value from gyro, accumulating in 32 bit variable, sign extended
                gyro_roll_calibration_value += temp; // 16 bit Y value from gyro, accumulating in 32 bit variable, sign extended
                temp = Wire1.read()<<8|Wire1.read(); // Combine the two bytes to make one integer, that could be negative
                if(temp > 32767) temp = temp - 65536; // if it's really a negative number, fix it
//                gyro_yaw_calibration_value += temp; // 16 bit Z value from gyro, accumulating in 32 bit variable, sign extended
                gyro_pitch_calibration_value += temp; // 16 bit Z value from gyro, accumulating in 32 bit variable, sign extended
                delay(2);                              // delay was 20, but sped it up

                Wire1.beginTransmission(MPU_address); // Start communication with the IMU
//                Wire1.write(0x3F); // Get the MPU6050_ACCEL_ZOUT_H value
                Wire1.write(0x3D); // Get the MPU6050_ACCEL_ZOUT_H value
                Wire1.endTransmission(); // End the transmission with the gyro
                Wire1.requestFrom(MPU_address,2);
                temp = Wire1.read()<<8|Wire1.read(); // Read the 16 bit number from IMU

                if(temp > 32767) temp = temp - 65536; // If it's really a negative number, fix it
                balance_calibration_value += temp; // 16 bit Z value from accelerometer, accumulating in 32 bit variable, sign extended
                delay(2);                           // delay was 20, but sped it up
            } //for   
            if(gyro_roll_calibration_value == -500 && 
                gyro_pitch_calibration_value == -500) // If calibration numbers are characteristically weird
            {
                while( 1==1) // request an IMU reset, because it's not working
                {
                    spl(" [setup] ******* IMU is stuck and needs a reset **********");
                    delay(3000);
                } //while
            } //if
            gyro_roll_calibration_value /= 500; // Divide the total value by 500 to get the avarage gyro pitch offset
            gyro_pitch_calibration_value /= 500; // Divide the total value by 500 to get the avarage gyro yaw offset
            balance_calibration_value /=500; // Divide the total value by 500 to get the avarage balance value (Z accelerometer)            
            sp(" [initializeIMU_2] gyro_pitch_calibration_value= "); 
            spl(gyro_pitch_calibration_value);
            sp(" [initializeIMU_2] gyro_roll_calibration_value= "); 
            spl(gyro_roll_calibration_value);
            sp(" [initializeIMU_2] speed override value= "); spl(speed);    
            sp(" [initializeIMU_2]: Balance value. NOTE - this is the balance value only if the robot is standing upright: ");
            spl(balance_calibration_value);
            sp(" [initializeIMU]: acc_calibration_value currently set to ");
            spl(acc_calibration_value);
    spl(" [initializeIMU_2] IMU initialization complete.");
    imuReady = true ;
    //vTaskDelete( NULL );
} //initializeIMU_2()
/***********************************************************************************************************
 This function configures the MPU6050 using settings recommended by Joop Brokking
 ***********************************************************************************************************/
void set_gyro_registers()
{
    LINE_TIME("<set_gyro_registers> Configure the MPU6050.");
    spl(" [set_gyro_registers] Wake up MPU"); // By default the MPU-6050 sleeps. So we have to wake it up.

    Wire1.beginTransmission(MPU_address); // Start communication with the address found during search.
    Wire1.write(0x6B); // We want to write to the PWR_MGMT_1 register (6B hex)
    Wire1.write(0x00); // Set the register bits as 00000000 to activate the gyro
    Wire1.endTransmission(); // End the transmission with the gyro.

    // Set the full scale of the gyro to +/- 250 degrees per second
    spl(" [set_gyro_registers] Set the full scale of the gyro to +/- 250 degrees per second");
    Wire1.beginTransmission(MPU_address); // Start communication with the address found during search.
    Wire1.write(0x1B); // We want to write to the GYRO_CONFIG register (1B hex)
    Wire1.write(0x00); // Set the register bits as 00000000 (250dps full scale)
    Wire1.endTransmission(); // End the transmission with the gyro
        
    // Set the full scale of the accelerometer to +/- 4g.
    spl(" [set_gyro_registers] Set the full scale of the accelerometer to +/- 4g");
    Wire1.beginTransmission(MPU_address); // Start communication with the address found during search.
    Wire1.write(0x1C); // We want to write to the ACCEL_CONFIG register (1A hex)
    Wire1.write(0x08); // Set the register bits as 00001000 (+/- 4g full scale range)
    Wire1.endTransmission(); // End the transmission with the gyro

    // Set some filtering to improve the raw data.
    spl(" [set_gyro_registers] Set Set Digital Low Pass Filter to ~43Hz to improve the raw data");
    Wire1.beginTransmission(MPU_address); // Start communication with the address found during search
    Wire1.write(0x1A); // We want to write to the CONFIG register (1A hex)
    Wire1.write(0x03); // Set the register bits as 00000011 (Set Digital Low Pass 
                      // Filter to ~43Hz)
    Wire1.endTransmission(); // End the transmission with the gyro 

} //set_gyro_registers

/***********************************************************************************************************
 This function reads the accelerometer and gyro info from the MPU6050. Note that we are expweriencing 
 issues with writing to the I2C bus
 ***********************************************************************************************************/
void read_mpu_6050_data()                                              
{
    LINE_TIME("<read_mpu_6050_data> Read MPU6050 registers.");
    Wire1.beginTransmission(MPU_address); // Start communicating with the MPU-6050
    Wire1.write(0x3B); // Send the requested starting register
    Wire1.endTransmission(); // End the transmission
    Wire1.requestFrom(MPU_address,14); // Request 14 bytes from the MPU-6050
    while(Wire1.available() < 14); // Wait until all the bytes are received
    acc_x = Wire1.read()<<8|Wire1.read(); // Add the low and high byte to the acc_x variable
    if(acc_x > 32767) acc_x = acc_x - 65536; // if it's really a negative number, fix it
    acc_y = Wire1.read()<<8|Wire1.read(); // Add the low and high byte to the acc_y variable
    if(acc_y > 32767) acc_y = acc_y - 65536; // if it's really a negative number, fix it
    acc_z = Wire1.read()<<8|Wire1.read(); // Add the low and high byte to the acc_z variable
    if(acc_z > 32767) acc_z = acc_z - 65536; // if it's really a negative number, fix it
    temperature = Wire1.read()<<8|Wire1.read(); // Add the low and high byte to the temperature variable
    if(temperature > 32767) temperature = temperature - 65536; // if it's really a negative number, fix it
    gyro_x = Wire1.read()<<8|Wire1.read(); // Add the low and high byte to the gyro_x variable
    if(gyro_x > 32767) gyro_x = gyro_x - 65536; // if it's really a negative number, fix it
    gyro_y = Wire1.read()<<8|Wire1.read(); // Add the low and high byte to the gyro_y variable
    if(gyro_y > 32767) gyro_y = gyro_y - 65536; // if it's really a negative number, fix it
    gyro_z = Wire1.read()<<8|Wire1.read(); // Add the low and high byte to the gyro_z variable
    if(gyro_z > 32767) gyro_z = gyro_z - 65536; // if it's really a negative number, fix it

} //read_mpu_6050_data()

/***********************************************************************************************************
 Setup for timer0 interrupt
 ***********************************************************************************************************/
void startTimer0()
{
    LINE_TIME("<startTimer0> Initialize and start timer0.");
    timer0 = timerBegin(timer_number_0, timer_prescaler_0, timer_cnt_up); // Pointer to hardware timer 0
    timerAttachInterrupt(timer0, &onTimer0, true); // Bind onTimer function to hardware timer 0
    timerAlarmWrite(timer0, 20, true); // Interrupt generated is of edge type not level (third arg) at 
                                            // count of 1000000 (second argument)
    yield();  //This yield was mentioned as a way to fix uissues starting timer multiple times                                        
    timerAlarmEnable(timer0); // Enable hardware timer 0

} //startTimer0()

/***********************************************************************************************************
 Setup for A4988 motor controllers
 ***********************************************************************************************************/
void initializeMotorControllers()
{
    LINE_TIME("<initializeMotorControllers> Set up GPIO pins connected to motors.");
    pinMode(pin_left_dir,OUTPUT); // Note that our motor control pins are Outputs
    pinMode(pin_left_step,OUTPUT);
    pinMode(pin_right_dir,OUTPUT);
    pinMode(pin_right_step,OUTPUT);
    digitalWrite(pin_left_dir, HIGH); // Init direction and step to known starting points
    digitalWrite(pin_left_step, LOW); // i.e. Forward (High) and not stepping (Low)
    digitalWrite(pin_right_dir, HIGH);
    digitalWrite(pin_right_step, LOW);

} //initializeMotorControllers()

/***********************************************************************************************************
 This is the setup function for Arduino sketches
 ***********************************************************************************************************/
void setup() 
{
    Serial.begin(115200); // Open a serial connection at 115200bps
    Serial.println("");
    LINE_TIME("<setup> Start of sketch.");
    sp(" [setup] PIN attached to onboard LED = "); spl(LED_BUILTIN);
    pinMode(LED_BUILTIN, OUTPUT); // Take control on onboard LED
    writeLED(false); // Turn onboard LED off
    display_Running_Sketch(); // Show environment details in console
//    Serial.setDebugOutput(true); // http://esp8266.github.io/Arduino/versions/2.0.0/doc/reference.html
        Serial.flush(); // Wait for message to clear buffer
        delay(200); // Allow time to pass   // was 1000 msec/loop, but sped it up
    startI2Cone(); // Scan the first I2C bus for LCD
    startI2Ctwo(); // Scan the second I2C bus for MPU   AM: This is the issue
    initializeLCD(); // Initialize the Open Smart 1602 LCD Display
    sendLCD("Boot Sequence",LINE1); // Boot message to LCD line 1
    spl(" [setup] Initialize IMU part 1");
    sendLCD("Init IMU part 1",LINE2); // Send more boot message to LCD line 2
    initializeIMU_1();         // move this earlier to maximize overlapping processing
    sendLCD("Init Motors",LINE2); // Send more boot message to LCD line 2
    initializeMotorControllers(); // Initialize the motor controllers
    sendLCD("Start WiFi",LINE2); // Starting WiFi message to LCD line 2
    startWiFi(); // Start WiFi and connect to AP
    startDNS(); // Start domain name service
    sendLCD("Start Webserver",LINE2); // Starting Webserver message to LCD line 2
    startWebServer(); // Start web service 
    sendLCD("Start Websockets",LINE2); // Starting Websockets message to LCD line 2
    startWebsocket(); // Include libaries in Websockets.h missing. Need to fix this.
    sendLCD("Monitor Socket",LINE2); // Send more boot message to LCD line 2
    xTaskCreatePinnedToCore(monitorWebsocket, // Create FreeRTOS task. This will monitor websocket activity in a parallel thread
        "monitorWebsocket", // String with name of task for debug purposes
        10000, // Stack size in words. Tried lower number but causes reboots
        NULL, // Parameter passed as input of the task
        2, // Priority of the task
        &monWebSocket, // Task handle
        0); // Specify which of the two CPU cores to pin this task to
    sendLCD("Monitor Web",LINE2); // Send more boot message to LCD line 2
    xTaskCreatePinnedToCore(monitorWeb, // Create FreeRTOS task. This will monitor web activity in a parallel thread
        "monitorWeb", // String with name of task for debug purposes
        10000, // Stack size in words. Tried lower number but causes reboots
        NULL, // Parameter passed as input of the task
        2, // Priority of the task
        &monWeb, // Task handle
        0); // Specify which of the two CPU cores to pin this task to
    spl(" [setup] Initialize IMU part 2");
    sendLCD("Init IMU part 2",LINE2); // Send more boot message to LCD line 2
    initializeIMU_2();
    sendLCD("Balance Task",LINE2); // Send more boot message to LCD line 2
    
/*
    xTaskCreatePinnedToCore(balanceRobot, // Create FreeRTOS task. This will keep the robot balanced
        "balanceRobot", // String with name of task for debug purposes
        10000, // Stack size in words.
        NULL, // Parameter passed as input of the task
        9, // Priority of the task
        &monBalance, // Task handle
        1); // Specify which of the two CPU cores to pin this task to
*/
    sendLCD(ipToString(WiFi.localIP()),LINE1); // Send IP address to LCD line 1
    sendLCD("Version: " + my_ver,LINE2); // Send version of code to LCD line 2
    startTimer0();
    LINE_TIME("<setup> initialization process complete");
    loop_timer = micros() + LoopDelay; // Set the loop_timer variable at the next end loop time

} //setup()

/***********************************************************************************************************
 This function monitors the websocket service for client activity. It runs as its own FreeRTOS task. 
 Variables and objects used in here need to be declared volitile in order to be accessible by the main
 FreeRTOS task.
 ***********************************************************************************************************/
void monitorWebsocket(void * parameter) 
{
    while(1)
    {
        webSocket.loop(); // Poll for websocket client events
        delay(10); // required delay to prevent watchdog timer gping off
    } //while
    vTaskDelete( NULL );

} //monitorWebsocket()

/***********************************************************************************************************
 This function monitors the web service for client activity. It runs as its own FreeRTOS task. 
 Variables and objects used in here need to be declared volitile in order to be accessible by the main
 FreeRTOS task.
 ***********************************************************************************************************/
void monitorWeb(void * parameter) 
{
    while(1)
    {
        server.handleClient(); // Poll for web server client
        delay(10); // required delay to prevent watchdog timer gping off
    } //while
    vTaskDelete( NULL );

} //monitorWebsocket()

/***********************************************************************************************************
 This function balances the robot. It runs as its own FreeRTOS task. Variables and objects used in here need 
 to be declared volitile in order to be accessible by the main FreeRTOS task.
 ***********************************************************************************************************/
//void balanceRobot(void * parameter)
void balanceRobot()
{
   //while(1)
   //{

        int temp; // Interim placeholder variable
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Note time at start of loop
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        mics = micros(); // Spot check the length of the main loop
        millis_now = millis();
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check to see if telemetry data needs to be sent to connected clients
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        if(sendTelemetry == "flagon") // Check to see if any client has requested telemetry info
        {
            sendClientBalanceGraph(99,            // Send data to clients used to assess balancing performance
                                angle_gyro,       // AM: target specific client instead of broadcast?
                                pid_output_left,
                                left_motor,pid_i_mem
                                ); 
        } //if                         
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // angle calculations   
        //////////////////////////////////////////////////////////////////////////////////////////////////////

        Wire1.beginTransmission(MPU_address); // Start communication with the gyro
//        Wire1.write(0x3F); // Start reading at register 3F (ACCEL_ZOUT_H)
        Wire1.write(0x3D); // Start reading at register 3D (ACCEL_YOUT_H)
        Wire1.endTransmission(); // End the transmission
        Wire1.requestFrom(MPU_address, 2); // Request 2 bytes from the gyro
        temp = Wire1.read()<<8|Wire1.read(); // Combine the two bytes to make one integer

        if(temp > 32767) temp = temp - 65536; // if it's really a negative number, fix it
        accelerometer_data_raw = temp;
        accelerometer_data_raw += acc_calibration_value; // Add the accelerometer calibration value
        if(accelerometer_data_raw > 8192)accelerometer_data_raw = 8192; // Prevent division by zero by 
                                                                        // limiting the acc data to +/-8200;
        if(accelerometer_data_raw < -8192)accelerometer_data_raw = -8192; // Prevent division by zero by 
                                                                        // limiting the acc data to +/-8200;
// crude fix adding 8 degress in next line was removed                                                                       
        angle_acc = asin((float)accelerometer_data_raw/8192.0)* 57.296; // Calculate the current angle 
                                                                        // according to the accelerometer
        if(start == 0 && angle_acc > -0.5&& angle_acc < 0.5) // If the accelerometer angle is almost 0
        {
            angle_gyro = angle_acc;                 // Load the accelerometer angle in the angle_gyro variable
            start = 1;                              // Set the start variable to start the PID controller
        } //if

        Wire1.beginTransmission(MPU_address); // Start communication with the gyro
//        Wire1.write(0x43); // Start reading at register 43
        Wire1.write(0x45); // Start reading at register 45
        Wire1.endTransmission(); // End the transmission
        Wire1.requestFrom(MPU_address, 4); // Request 4 bytes from the gyro
        temp = Wire1.read()<<8|Wire1.read(); // Combine the two bytes read to make one 16 bit signed integer
        if(temp > 32767) temp = temp - 65536; // if it's really a negative number, fix it
//        gyro_yaw_data_raw = temp; // and use result as raw data, which is yaw degrees/sec * 65.5
        gyro_pitch_data_raw = temp; // and use result as raw data, which is pitch degrees/sec * 65.5
        temp = Wire1.read()<<8|Wire1.read(); // Combine the two bytes read to make one 16 bit signed integer
        if(temp > 32767) temp = temp - 65536; // if it's really a negative number, fix it
//        gyro_pitch_data_raw = temp; // and use result as raw data, which is pitch degrees/sec * 65.5
//        gyro_pitch_data_raw -= gyro_pitch_calibration_value; // Add the gyro calibration value
//        angle_gyro += gyro_pitch_data_raw * 0.000030534; // Calculate the traveled during this loop angle 
                                                        // and add this to the angle_gyro variable
        gyro_roll_data_raw = temp; // and use result as raw data, which is pitch degrees/sec * 65.5
        gyro_roll_data_raw -= gyro_roll_calibration_value; // Add the gyro calibration value
        angle_gyro += gyro_roll_data_raw * 0.000030534; // Calculate the traveled during this loop angle 
                                                        // and add this to the angle_gyro variable
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // MPU-6050 offset compensation. Not every gyro is mounted 100% level with the axis of the robot. This 
        // can be cause by misalignments during manufacturing of the breakout board. As a result the robot
        // will not rotate at the exact same spot and start to make larger and larger circles. To compensate
        // for this behavior a VERY SMALL angle compensation is needed when the robot is rotating. Try  
        // 0.0000003 or -0.0000003 first to see if there is any improvement.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
//        gyro_yaw_data_raw -= gyro_yaw_calibration_value; // Add the gyro calibration value
        gyro_pitch_data_raw -= gyro_pitch_calibration_value; // Add the gyro calibration value
        // Uncomment the following line to make the compensation active
        // re-comment the line below to see if angle calibration gets more accurate
//        angle_gyro -= gyro_yaw_data_raw * 0.0000003; // Compensate the gyro offset when the robot is rotating
//        angle_gyro += gyro_pitch_data_raw * 0.0000003; // Compensate the gyro offset when the robot is rotating

//angle_gyro = angle_gyro * 0.9996 + angle_acc * 0.0004; // Correct drift of gyro angle with the accelerometer angle
        angle_gyro = angle_gyro * 0.996 + angle_acc * 0.004; // Correct drift of gyro angle with the accelerometer angle

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // PID controller calculations. The balancing robot is angle driven. First the difference between the 
        // desired angel (setpoint) and actual angle (process value) is calculated. 
        //////////////////////////////////////////////////////////////////////////////////////////////////////
   
        pid_error_temp = angle_gyro ;  // balance point is at zero degrees, including balance calibration...
                                       // done via the acc_calibration_value paremeter in startWiFi

// following line works differently for +ve and -ve pid_error_temp values.
// cases seen where it converts a small -ve error to a positive error, and drives motors in wrong direction
// just remove it for now. could also fix it
//       if(pid_output > 10 || pid_output < -10)pid_error_temp += pid_output * 0.015;

        pid_i_mem += pid_i_gain * pid_error_temp;       // Calculate the I-controller value and add it to the  
                                                        // pid_i_mem variable
        hold1 = pid_i_gain * pid_error_temp;            // current I controller increment, for debug
        hold2 = pid_i_mem; // grab it for debugging before it gets changed

// try to reduce longevity of pid_i_mem, which gets big, and stays big
//        pid_i_mem =temp + PID_I_fade * pid_i_mem;     // allow impact of past pid_i_mem history to fade out over time
        if(pid_i_mem > pid_max)pid_i_mem = pid_max;     // Limit the I-controller to the parameterized maximum  
                                                        // controller output
        else if(pid_i_mem < pid_min)pid_i_mem = pid_min;
        //Calculate the PID output value
        pid_output = pid_p_gain * pid_error_temp + pid_i_mem + pid_d_gain * (pid_error_temp - pid_last_d_error);
        hold3 = pid_output;
        hold4 = pid_p_gain * pid_error_temp ;           // the P in PID
        hold5 = pid_d_gain * (pid_error_temp - pid_last_d_error);
        if(pid_output > pid_max)pid_output = pid_max;   // Limit the PI-controller to the maximum controller output
        else if(pid_output < pid_min)pid_output = pid_min;
        pid_last_d_error = pid_error_temp;              // Store the error for the next loop

        if(pid_output < 5 && pid_output > -5)pid_output = 0; // Create a dead-band to stop the motors when the robot 
                                                            // is balanced

        if(angle_gyro > 30 || angle_gyro < -30 || start == 0 ) // If the robot tips over or the start variable is zero
        {
            pid_output = 0; // Set the PID controller output to 0 so the motors stop moving
            pid_i_mem = 0; // Reset the I-controller memory
            start = 0; // Set the start variable to 0
            throttle_left_motor = 0; // stop the wheels from moving
            throttle_right_motor = 0;
        } //if

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Control Calculations (does nothing without numchuk controller to provide "received_byte", which  
        // stays at zero
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        pid_output_left = pid_output;  // Copy the controller output to the pid_output_left variable for the  
                                       // left motor
        pid_output_right = pid_output; // Copy the controller output to the pid_output_right variable for the 
                                       // right motor
// numchuk related adjustments here were deleted

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Motor Pulse calculations. To compensate for the non-linear behaviour of the stepper motors the 
        // following calculations are needed to get a linear speed behaviour.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // if(pid_output_left > 0)pid_output_left = 405 - (1/(pid_output_left + 9)) * 5500;
        // else if(pid_output_left < 0)pid_output_left = -405 - (1/(pid_output_left - 9)) * 5500;
        // if(pid_output_right > 0)pid_output_right = 405 - (1/(pid_output_right + 9)) * 5500;
        // else if(pid_output_right < 0)pid_output_right = -405 - (1/(pid_output_right - 9)) * 5500;
        // Calculate the needed pulse time for the left and right stepper motor controllers
        // if(pid_output_left > 0)left_motor = 400 - pid_output_left;
        // else if(pid_output_left < 0)left_motor = -400 - pid_output_left;
        // else left_motor = 0;
        // if(pid_output_right > 0)right_motor = 400 - pid_output_right;
        // else if(pid_output_right < 0)right_motor = -400 - pid_output_right;
        // else right_motor = 0;
        // save a value for once a second debugging before we overwrite it
        hold = pid_output_left;
        // modifications for A4988 based motor controllers:
        //  - ignore linearity considerations. Don't think the difference is substantial
        //  - map the pid output directly to speed range of motors.
        //  - reliable speeds are from 300 steps/sec (fast) to 2300 steps/sec (slow)
        //  - pid range is 1(slow) to 400 (fast)
        //  - all of this applies to both directions, + and - values
        //  - so, linearly map (400 > 0) to (300 > 2300)
        //
        // Calculate the needed pulse time for the left and right stepper motor controllers
        if(pid_output_left > 0)left_motor = bot_slow - (pid_output_left/400)*(bot_slow - bot_fast);
        else if(pid_output_left < 0)left_motor = -1*bot_slow - (pid_output_left/400)*(bot_slow - bot_fast);
        else left_motor = 0;
        if(pid_output_right > 0)right_motor = bot_slow - (pid_output_right/400)*(bot_slow - bot_fast);
        else if(pid_output_right < 0)right_motor = -bot_slow - (pid_output_right/400)*(bot_slow - bot_fast);
        else right_motor = 0;
        //Copy the pulse time to the throttle variables so the interrupt subroutine can use them
//        left_motor *= -1;            //reverse the direction on TWIPi
//        right_motor *= -1;           // for both wheels
          
        if (speed >= 0) // if we're overriding IMU to force a constant test speed
        {
            noInterrupts(); // ensure interrupt can't happen when only one wheel is updated
            throttle_left_motor = speed; // overwrite the calculated wheel intervals
            throttle_right_motor = speed; // ...with fixed value, maybe zero to brake for vertical calibration
            interrupts();                // by briefly disabling interrupts
        } //if
        else // restructure conditional so we don't have a brief wrong setting
        {
            noInterrupts(); // ensure interrupt can't happen when only one wheel is updated
            throttle_left_motor = left_motor; 
            throttle_left_motor_memory = 0;           // force new speed into immediate effect  
            throttle_right_motor = right_motor; 
            throttle_right_motor_memory = 0;          // force new speed into immediate effect  
            interrupts();                           
        } //else

        // generate telemetry data at the end of the 4 msec loop

            String sTmp = String(micros()) ;            // timestamp in microseconds
            sTmp += "," + String(angle_gyro) ;          // sensed angle via gyro
            sTmp += "," + String(angle_acc) ;           // sensed angle via accelerometer
            sTmp += "," + String(start) ;               // our wheel enable control
            sTmp += "," + String(left_motor) ;          // step interval counter passed to ISR
            sTmp += "," + String(pid_error_temp) ;      // current angle error (=angle)
            sTmp += "," + String(hold3) ;               // raw total pid output
            sTmp += "," + String(pid_output) ;          // cleaned up total pid output
            sTmp += "," + String(pid_i_mem) ;           // I component in PID, incl gain
            sTmp += "," + String(hold1) ;               // increment in the I component, incl gain
            sTmp += "," + String(hold2) ;               // raw pid_i_mem before cleaning, incl gain
            sTmp += "," + String(hold4) ;               // P component in PID, incl gain
            sTmp += "," + String(hold5) ;               // D component in PID, incl gain

            reportTel(sTmp); // Example of how to send telemetry 


        // do some debug output to serial monitor to see whats going on
        //count 4 millesecond loops to get to a second, and dump debug info once a second
        if (i++ > 250) // if a full second has passed, display debug info
        {
            i = 0; // prepare to count up to next second
//            float dtmp = angle_gyro; // temp cheat to help tune balance point
            spd("--pid_error_temp= ",pid_error_temp); spd("  angle_gyro= ",angle_gyro); 
//            spd("  dtmp= ",dtmp); 
            spd(" start= ",start);
//            spd("  throttle_left_motor= ",throttle_left_motor); 
            spd("  left_motor= ",left_motor); 
            spd("  pid_i_mem= ",pid_i_mem);
            spd("  angle_acc= ",angle_acc);
            spl(); 
 //           spd("t0_per_sec = ", t0_per_sec); spl();  
            sendLCD(String(angle_gyro),1);           
            noInterrupts(); // ensure interrupt can't happen when updating ISR varaible
            t0_per_sec = 0;  
            interrupts(); // Re-enable interrupts
        } //if
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Poll network services for client activity. Note each of these polls are meant to run in a separate 
        // thread, which may be commented out in setup()
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //server.handleClient(); // Poll for web server client
        //webSocket.loop(); // Poll for websocket client events

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // loop_timer: The angle calculations are tuned for a loop time of 4 milliseconds. To make sure every loop 
        // is exactly 4 milliseconds a wait loop is created by setting the loop_timer variable to +4000 
        // microseconds every loop.
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        last_millis = millis()-millis_now; // track previous loop's length as well
        if(loop_timer > micros()) // if the target loop end time is still in the future..
        {
            while(loop_timer > micros()) {}; // spin in this while until micros() catches up with loop_timer
        } //if                               // no spin time needed if we're already past old target loop end
        loop_timer = micros() + 4000;        // time next target loop end time is 4 msec from now.  
        cntBalance++;  
  // } //while    

} //balanceRobot()

/***********************************************************************************************************
 This is the main function for Arduino sketches
 ***********************************************************************************************************/
void loop() 
{
    cntLoop++;

    balanceRobot();
/*
    Serial.print("t0_per_sec = ");
    Serial.print(t0_per_sec);
    Serial.print(" cntTimer0 = ");
    Serial.print(cntTimer0);
    Serial.print(" cntBalance = ");
    Serial.print(cntBalance);
    Serial.print(" cntLoop = ");
    Serial.println(cntLoop);
*/
} //loop()