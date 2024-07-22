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
const int servoPin = 2;
const int servoPin2 = 3;
const int potPin = A3;
const int potPin2 = A2;
const int buttonPin = 5;
const int upbuttonPin = 6;
const int downbuttonPin = 7;
const int ultrasonicPin = 4;
const int lightSensorPin = A1; // Pin for the light sensor

Servo myservo;
Servo myservo2;
Ultrasonic ultrasonic(ultrasonicPin);

// sensor readings
int val;
int val2;
long RangeInCentimeters;
int lightSensorValue;
int buttonState;
int upbuttonState;
int downbuttonState;
int lastButtonState = LOW;
unsigned long buttonPressStartTime;

// record data
const int maxRecords = 10; // Reduced to save memory
int recordedPositions1[maxRecords];
int recordedPositions2[maxRecords];
long recordedSensorValues[maxRecords];
int recordCount = 0;
bool playingBack = false;
bool trainingmode = false;
bool useDistanceSensor = false; // Declaration of useDistanceSensor
bool useLightSensor = false;    // Declaration of useLightSensor

void checkMemory() {
  extern int __heap_start, *__brkval;
  int v;
  int free_memory = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__heap_start);
  Serial.print(F("Free memory: "));
  Serial.println(free_memory);
}

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Start the I2C communication

  myservo.attach(servoPin);
  myservo2.attach(servoPin2);
  pinMode(buttonPin, INPUT);
  pinMode(upbuttonPin, INPUT);
  pinMode(downbuttonPin, INPUT);

  // display set up
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(F("Bienvenidos"), 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 0);
  display.println(F("Bienvenidos"));

  display.setTextSize(2);
  display.getTextBounds(F("COMENZAR"), 0, 0, &x1, &y1, &w, &h);
  int16_t box_x = (SCREEN_WIDTH - w) / 2 - 4;
  int16_t box_y = 16 - 4;
  uint16_t box_w = w + 8;
  uint16_t box_h = h + 8;
  display.drawRect(box_x, box_y, box_w, box_h, SSD1306_WHITE);
  display.setCursor((SCREEN_WIDTH - w) / 2, 16);
  display.println(F("COMENZAR"));

  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println(F("Smart Motors Paraguay"));

  display.display();
  delay(2000);
  display.clearDisplay();

  checkMemory(); // Check free memory at the end of setup
}

void displaySensorChoice(const __FlashStringHelper* sensor) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(sensor);
  display.display();
}

void displaySensorValue(int sensorValue, const __FlashStringHelper* label) {
  int percentage = map(sensorValue, 0, 100, 0, 100);
  display.clearDisplay();
  display.drawLine(0, 47, 0, 62, SSD1306_WHITE);
  display.fillRect(1, 50, map(percentage, 0, 100, 0, SCREEN_WIDTH - 2), 10, SSD1306_WHITE);
  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(102, 50);
  display.print(percentage);
  display.println(F("%"));
  display.fillRect(1, 11, 68, 33, SSD1306_BLACK); // Clear box
  display.setCursor(5, 30);
  display.setTextSize(1);
  display.println(label);
  display.setTextSize(1);
  display.display();
}

void trainDistanceSensor() {
  val = analogRead(potPin);
  val2 = analogRead(potPin2);

  val = map(val, 0, 1023, 0, 180);
  val2 = map(val2, 0, 1023, 0, 180);

  RangeInCentimeters = ultrasonic.MeasureInCentimeters();

  //Serial.println(RangeInCentimeters);

  int distancePercentage = map(RangeInCentimeters, 0, 100, 0, 100);
  
  displaySensorValue(distancePercentage, F("Sensor de distancia"));

  if (buttonState == HIGH && lastButtonState == LOW) {
    buttonPressStartTime = millis();
  } else if (buttonState == LOW && lastButtonState == HIGH) {
    unsigned long pressDuration = millis() - buttonPressStartTime;
    if (pressDuration >= 2000) {
      if (recordCount > 0) {
        playingBack = true;
      }
    } else if (pressDuration >= 1000 && pressDuration < 2000) {
      if (!playingBack) {
        recordSensorData(val, val2, RangeInCentimeters);
      }
    }
  }

  lastButtonState = buttonState;

  if (playingBack) {
    playbackServoPosition(RangeInCentimeters);
  } else {
    myservo.write(val);
    myservo2.write(val2);
  }

  delay(15);
}

void trainLightSensor() {
  val = analogRead(potPin);
  val2 = analogRead(potPin2);

  val = map(val, 0, 1023, 0, 180);
  val2 = map(val2, 0, 1023, 0, 180);

  lightSensorValue =  map(analogRead(lightSensorPin), 0, 1023, 0, 100);
  //Serial.println(lightSensorValue);
  displaySensorValue(lightSensorValue, F("Sensor de luz"));

  if (buttonState == HIGH && lastButtonState == LOW) {
    buttonPressStartTime = millis();
  } else if (buttonState == LOW && lastButtonState == HIGH) {
    unsigned long pressDuration = millis() - buttonPressStartTime;
    if (pressDuration >= 2000) {
      if (recordCount > 0) {
        playingBack = true;
      }
    } else if (pressDuration >= 1000 && pressDuration < 2000) {
      if (!playingBack) {
        recordSensorData(val, val2, lightSensorValue);
      }
    }
  }

  lastButtonState = buttonState;

  if (playingBack) {
    playbackServoPosition(lightSensorValue);
  } else {
    myservo.write(val);
    myservo2.write(val2);
  }

  delay(15);
}

void recordSensorData(int servoPosition, int servoPosition2, long sensorValue) {
  if (recordCount < maxRecords) {
    recordedPositions1[recordCount] = servoPosition;
    recordedPositions2[recordCount] = servoPosition2;
    recordedSensorValues[recordCount] = sensorValue;
    recordCount++;
  }
}

void playbackServoPosition(long currentSensorValue) {
  int closestIndex = -1;
  long closestDifference = 999999;

  for (int i = 0; i < recordCount; i++) {
    long difference = abs(currentSensorValue - recordedSensorValues[i]);
    if (difference < closestDifference) {
      closestDifference = difference;
      closestIndex = i;
    }
  }

  if (closestIndex != -1) {
    myservo.write(recordedPositions1[closestIndex]);
    myservo2.write(recordedPositions2[closestIndex]);
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
      useLightSensor = false;
      displaySensorChoice(F("Sensor de distancia"));
    } else if (upbuttonState == HIGH) {
      useLightSensor = true;
      trainingmode = true;
      useDistanceSensor = false;
      displaySensorChoice(F("Sensor de luz"));
    }
  }

  if (useDistanceSensor && trainingmode) {
    trainDistanceSensor();
  }

  if (useLightSensor && trainingmode) {
    trainLightSensor();
  }

  // Check memory periodically
  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 10000) { // every 10 seconds
    checkMemory();
    lastMemoryCheck = millis();
  }
}
