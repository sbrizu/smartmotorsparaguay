#include <Servo.h>
#include "Ultrasonic.h" // specify address 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

// Constants for pin connections
const int servoPin = 2;
const int servoPin2 = 3;
const int potPin = A3;
const int potPin2 = A2;
const int buttonPin = 5;
const int upbuttonPin = 6;
const int downbuttonPin = 7;
const int ultrasonicPin = 4;
const int light_sensor = A1; 


Servo myservo;
Servo myservo2;
Ultrasonic ultrasonic(ultrasonicPin);

// sensor readings
int val;
int val2;
long RangeInCentimeters;
int buttonState;
int upbuttonState;
int downbuttonState;
int lightLevel;
int lastButtonState = LOW;
unsigned long buttonPressStartTime;

// record data
const int maxRecords = 20;
int recordedPositions1 [maxRecords];
int recordedPositions2 [maxRecords];
long recordedDistances[maxRecords];
int recordedLightLevels[maxRecords];
int recordCount = 0;
bool recording = false;
bool playingBack = false;
long targetDistance = -1;
int actualLightLevel = -1;
bool useLightSensor = false;
bool useDistanceSensor = false;
bool trainingmode = false;
bool playbackmode = false;

void setup() {
  
  Serial.begin(9600);
  myservo.attach(servoPin);
  myservo2.attach(servoPin2);
  pinMode(buttonPin, INPUT);
  pinMode(upbuttonPin, INPUT);
  pinMode(downbuttonPin, INPUT);

  // display set up
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Bienvenidos"));

 // display.setTextSize(2);
  //display.setCursor(0, 16);
  display.println(F("COMENZAR"));

 // display.setTextSize(1);
  //display.setCursor(0, 50);
  //display.println(F("Smart Motors Paraguay"));
/*
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Bienvenidos"));
  
  int16_t x1, y1;
  uint16_t w, h;

  display.setTextSize(2);
  display.getTextBounds("COMENZAR", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 16);
  display.println("COMENZAR");
  display.drawRect((128 - w) / 2 - 2, 16 + y1 - 2, w + 4, h + 4, SSD1306_WHITE);


  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println("Smart Motors Paraguay");
*/
  display.display();
  delay(2000);
  display.clearDisplay();

}


//void displayTrainLight(){



//  // Read the distance from the ultrasonic sensor
  //angeInCentimeters = ultrasonic.MeasureInCentimeters();
// read  light sensor
 // int raw_light = analogRead(light_sensor);
 // lightLevel = map(raw_light, 0, 1023, 0, 100);
