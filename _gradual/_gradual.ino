#include <Servo.h>
#include "Ultrasonic.h" 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
const int maxRecords = 10; // Reduced to save memory
int recordedPositions1[maxRecords];
int recordedPositions2[maxRecords];
long recordedDistances[maxRecords];
int recordedLightLevels[maxRecords];
int recordCount = 0;
bool playingBack = false;
long targetDistance = -1;
int actualLightLevel = -1;
bool useLightSensor = false;
bool useDistanceSensor = false;
bool trainingmode = false;
bool playbackmode = false;

void checkMemory() {
  extern int __heap_start, *__brkval;
  int v;
  int free_memory = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
  Serial.print(F("Free memory: "));
  Serial.println(free_memory);
}

void setup() {
  Serial.begin(9600);
  myservo.attach(servoPin);
  myservo2.attach(servoPin2);
  pinMode(buttonPin, INPUT);
  pinMode(upbuttonPin, INPUT);
  pinMode(downbuttonPin, INPUT);

  // display set up
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Bienvenidos"));
  
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.println(F("COMENZAR"));
  
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println(F("Smart Motors Paraguay"));
  
  display.display();
  delay(2000);
  display.clearDisplay();

  checkMemory(); // Check free memory at the end of setup
}

void loop() {
  // Read sensor values
  val = analogRead(potPin);
  val2 = analogRead(potPin2);

  val = map(val, 0, 1023, 0, 180);
  val2 = map(val2, 0, 1023, 0, 180);

  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  lightLevel = analogRead(light_sensor);

  // Update servos
  myservo.write(val);
  myservo2.write(val2);

  // Display values on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Pot1: ")); display.println(val);
  display.print(F("Pot2: ")); display.println(val2);
  display.print(F("Dist: ")); display.println(RangeInCentimeters);
  display.print(F("Light: ")); display.println(lightLevel);
  display.display();

  delay(1000); // Small delay to allow for display updates

  // Check memory periodically
  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 10000) { // every 10 seconds
    checkMemory();
    lastMemoryCheck = millis();
  }
}


