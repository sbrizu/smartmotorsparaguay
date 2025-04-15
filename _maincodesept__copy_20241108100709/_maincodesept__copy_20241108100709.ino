#include <Wire.h>
#include <Servo.h>
#include "Ultrasonic.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants for pin connections
const int servoPin = 5;
const int servoPin2 = 6;
const int potPin = A3;
const int potPin2 = A2;
const int buttonPin = 2;
const int upbuttonPin = 3;
const int downbuttonPin = 4;
const int ultrasonicPin = 9;
const int lightSensorPin = A1; // Pin for the light sensor

Servo myservo;
Servo myservo2;
Ultrasonic ultrasonic(ultrasonicPin);