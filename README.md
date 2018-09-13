# TWIPi
This repository contains all the information on the fourth iteration of the SegbotSTEP family of robot designs. This repository contains C++ source code intended to run on the Adafruit Huzzah32 development board; Java code intended to run on a remote client such as a MAC or PC; EagleCAD schematic and layout files containing the circuit designs of the robot; Fusion 360 files (or pointers to those files on the AutoCAD cloud) used to mill the robot chassis; and all the libraries used by the IDEs that make up the tool chain used to create the robot.  

## Getting Started

Full details about this project can be read on our [wiki](https://github.com/va3wam/TWIPi/wiki). 

### Prerequisites

In order to make use of the robot circuit board and schematic diagrams you will need AutoCAD's EagleCAD application. In order to make use of the CAD and CAM diagrams of the robot chassis you will need AutoCAD's Fusion 360 application. From a software perspective, the Java and C++ (Arduino) code was written using Microsoft's Visual Source Code IDE.

### SOC Hardware Compatability

The Arduino C++ code was written to run on the Espressif ESP WROOM32 "Software On a Chip" (SOC). The development board used for this SOC is the Adafruit Huzzah32 featherboard.  

```
Note that this code will NOT run on an ESP8266. 
```

### Other Hardware Compatability

Aside from the code and development you will need to build a TWIPi compatible chassis and PCB. This
repository has EagleCAD diagrams to help you build the PCB. Also included are Fusion360 
CAD/CAM diagrams and associated GCODE for a Tormach 770 CNC mill to create the robot chassis. 
Full details for this project are located on the wiki associated with this repository.

### Installing

There are numerous tutorials on how to install Arduino code onto an ESP WROOM32 from Visual Source Code. Details and relevant links can be found on this respositories wiki. 

## Running the tests

At this time there are no automated tests for this system

## Deployment

This Arduino code was developed using the following set up.

### Built With

* [Visual Studio Code](https://code.visualstudio.com/) - The IDE
* [Platformio](https://platformio.org/) - Editor plugin
* Library Framework - Arduino
* Hardware platform - espressif32
* Board - Adafruit Huzzah32 featherboard
### Arduino Libraries
The following additional libraries are used in the Arduino C++ code:
* https://github.com/Pedroalbuquerque/ESP32WebServer
* https://github.com/Links2004/arduinoWebSockets
* https://github.com/marcoschwartz/LiquidCrystal_I2C
* https://github.com/bblanchon/ArduinoJson


## Contributing

This project is being developed by a couple of buddies. We are tracking the tasks for this
effort in a series of small [projects](https://github.com/va3wam/TWIPi/projects).

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/va3wam/TWIPi/branches/all). 

## Authors

* [Doug](https://github.com/nerdoug) - *PID and motor control logic and circuit diagrams*
* [Andrew](https://github.com/va3wam) - *Initial code porting from ESP128 to ESP32, remote Java client and CAD/CAM designs* 

See also the list of [contributors](https://github.com/va3wam/TWIPi/graphs/contributors) who participated in this project.

## License

This project has no licensing terms. Use at own risk, no support provided. 

## Acknowledgments

* Big thanks to [Brokking](http://www.brokking.net/yabr_main.html) for his excellent tutorial which really helped us get going
* Thanks to the folks who developed the libraries we have made use of for I2C, JSON, Web and Websockets

