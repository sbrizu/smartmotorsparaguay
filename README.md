** main code was written by a trained ChatGPT assistant, and can be accessed through this link https://chatgpt.com/g/g-qxPnfhw55-adafruit-assistant **

``` C++
Code is written in C++
```

# Libraries utilized
All of these libraries are included in the Arduino IDE. If using another IDE, download the files from the repository.

``` C++
#include <Wire.h>
#include <Servo.h>
#include "Ultrasonic.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
```

# Files description
Main code under "_maincode_". Other files are for testing and learning purposes. The main code is made for an Arduino-based tool that utilizes 2 servo motors, 1 grove analog sensor (light), 1 grove digital sensor 
(ultrasonic), an OLED I2C 164x64 screen and 3 push buttons. If re-designing the PCB, remember to edit the pin numbers accordingly. 

# Modifying the code 
Download the main file and make a copy if making modifications. 

# Building your Smart Motor
The list of the main components and the PCB design file can be found here: https://shorturl.at/sO3Ds
Steps:
1. Purchase the components needed
2. Manufacture the PCB
3. Once the PCB is ready, solder all of the components
4. Put your PCB on top on your Arduino board
5. Plug your Arduino to your computer and upload the code
6. Your Smart Motor is ready to go!

# Collaboration
If creating new circuits (using only 1 motor and 1 sensor for example) feel free to add your example code to the repository, along with your PCB design or other ideas
