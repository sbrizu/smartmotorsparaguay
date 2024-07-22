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
int lastButtonState = LOW;
unsigned long buttonPressStartTime;

// record data
const int maxRecords = 10; // Reduced to save memory
int recordedPositions1[maxRecords];
int recordedPositions2[maxRecords];
long recordedDistances[maxRecords];
int recordCount = 0;
bool playingBack = false;
bool trainingmode = false;
bool useDistanceSensor = false; // Declaration of useDistanceSensor

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
  delay(500);

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

void trainDistanceSensor() {
  val = analogRead(potPin);
  val2 = analogRead(potPin2);

  val = map(val, 0, 1023, 0, 180);
  val2 = map(val2, 0, 1023, 0, 180);

  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  
  if (buttonState == HIGH && lastButtonState == LOW) {
    buttonPressStartTime = millis();
  } else if (buttonState == LOW && lastButtonState == HIGH) {
    unsigned long pressDuration = millis() - buttonPressStartTime;
    if (pressDuration >= 2000) {
      if (recordCount > 0) {
        playingBack = true;
        Serial.println("Starting playback.");
      }
    } else if (pressDuration >= 1000 && pressDuration < 2000) {
      if (!playingBack) {
        Serial.print("Target distance set to: ");
        Serial.println(RangeInCentimeters);
        recordDistanceData(val, val2, RangeInCentimeters);
      }
    }
  }

  lastButtonState = buttonState;

  if (playingBack) {
    playbackDistanceServoPosition(RangeInCentimeters);
  } else {
    myservo.write(val);
    myservo2.write(val2);
  }

  delay(15);
}

void recordDistanceData(int servoPosition, int servoPosition2, long distance) {
  if (recordCount < maxRecords) {
    recordedPositions1[recordCount] = servoPosition;
    recordedPositions2[recordCount] = servoPosition2;
    recordedDistances[recordCount] = distance;
    recordCount++;
    Serial.print("Recorded Servo Position 1: ");
    Serial.print(servoPosition);
    Serial.print(" and Servo Position 2: ");
    Serial.print(servoPosition2);
    Serial.print(" degrees, Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("Memory Full: Cannot record more data.");
  }
}

void playbackDistanceServoPosition(long currentDistance) {
  int closestIndex = -1;
  long closestDistance = 999999;

  for (int i = 0; i < recordCount; i++) {
    long distanceDiff = abs(currentDistance - recordedDistances[i]);
    if (distanceDiff < closestDistance) {
      closestDistance = distanceDiff;
      closestIndex = i;
    }
  }

  if (closestIndex != -1) {
    myservo.write(recordedPositions1[closestIndex]);
    myservo2.write(recordedPositions2[closestIndex]);
    Serial.print("Playback Servo Position 1: ");
    Serial.print(recordedPositions1[closestIndex]);
    Serial.print(" and Servo Position 2: ");
    Serial.print(recordedPositions2[closestIndex]);
    Serial.print(" degrees, Distance: ");
    Serial.print(recordedDistances[closestIndex]);
    Serial.println(" cm");
  }
}

void loop() {
  buttonState = digitalRead(buttonPin);
  upbuttonState = digitalRead(upbuttonPin);
  downbuttonState = digitalRead(downbuttonPin);

  if (!playingBack && !trainingmode) {
    if (downbuttonState == HIGH) {
      useDistanceSensor = true;
      trainingmode = true;
      Serial.println("Using Distance Sensor for training.");
    }
  }

  if (useDistanceSensor && trainingmode) {
    trainDistanceSensor();
  }

  // Check memory periodically
  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 10000) { // every 10 seconds
    checkMemory();
    lastMemoryCheck = millis();
  }
}
