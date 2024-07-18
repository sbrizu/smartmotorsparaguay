#include <Servo.h>
#include "Ultrasonic.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C // Address for 128x64 OLED display

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants for pin connections
const int servoPin = 2;
const int potPin = A3;
const int buttonPin = 5;
const int ultrasonicPin = 4;

// Create servo object and ultrasonic sensor object
Servo myservo;
Ultrasonic ultrasonic(ultrasonicPin);

// Variables for sensor readings
int val;
long RangeInCentimeters;
int buttonState;
int lastButtonState = LOW;
unsigned long buttonPressStartTime;

// Variables for recording data
const int maxRecords = 100;
int recordedPositions[maxRecords];
long recordedDistances[maxRecords];
int recordCount = 0;
bool recording = false;
bool playingBack = false;
long targetDistance = -1;

// Setup function
void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Attach the servo to the pin
  myservo.attach(servoPin);

  // Initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);

  // Initialize the display

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds("Bienvenidos Paraguay", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 0);
  display.println("Bienvenidos Paraguay");
  display.display();
  delay(2000);
  display.clearDisplay();
}


// Loop function
void loop() {
  // Read the value from the potentiometer
  val = analogRead(potPin);

  // Map the potentiometer value to the servo position (0-180 degrees)
  val = map(val, 0, 1023, 0, 180);

  // Read the distance from the ultrasonic sensor
  RangeInCentimeters = ultrasonic.MeasureInCentimeters();

  // Read the pushbutton state
  buttonState = digitalRead(buttonPin);

  // Check if the button is pressed
  if (buttonState == HIGH && lastButtonState == LOW) {
    // Button was just pressed
    buttonPressStartTime = millis();
  } else if (buttonState == LOW && lastButtonState == HIGH) {
    // Button was just released
    unsigned long pressDuration = millis() - buttonPressStartTime;
    if (pressDuration >= 2000) {
      // Button was held for 2 seconds or more
      if (recordCount > 0) {
        playingBack = true;
        Serial.println("Starting playback.");
      }
    } else if (pressDuration >= 1000 && pressDuration < 2000) {
      // Button was held for 1 second or more but less than 2 seconds
      if (!recording) {
        targetDistance = RangeInCentimeters;
        Serial.print("Target distance set to: ");
        Serial.println(targetDistance);
        recordData(val, RangeInCentimeters); // Record the servo position and distance
        //displayTrain();
        delay(15);
      }
    }
  }

  // Update the last button state
  lastButtonState = buttonState;

  // If playing back, set servo based on recorded distances
  if (playingBack) {
    playbackServoPosition(RangeInCentimeters);
    //displayListo();
  } else {
    // Set the servo position if not playing back
    myservo.write(val);
  }

  // Small delay to stabilize readings
  delay(15);
}

// Function to record data
void recordData(int servoPosition, long distance) {
  if (recordCount < maxRecords) {
    recordedPositions[recordCount] = servoPosition;
    recordedDistances[recordCount] = distance;
    recordCount++;
    Serial.print("Recorded Servo Position: ");
    Serial.print(servoPosition);
    Serial.print(" degrees, Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("Memory Full: Cannot record more data.");
  }
  delay(15);
}

// Function to play back recorded data based on current distance
void playbackServoPosition(long currentDistance) {
  // Find the closest recorded distance to the current distance
  int closestIndex = -1;
  long closestDistance = 999999; // Arbitrary large number for initial comparison

  for (int i = 0; i < recordCount; i++) {
    long distanceDiff = abs(currentDistance - recordedDistances[i]);
    if (distanceDiff < closestDistance) {
      closestDistance = distanceDiff;
      closestIndex = i;
    }
  }

  // Set the servo to the position corresponding to the closest distance
  if (closestIndex != -1) {
    myservo.write(recordedPositions[closestIndex]);
    Serial.print("Playback Servo Position: ");
    Serial.print(recordedPositions[closestIndex]);
    Serial.print(" degrees, Distance: ");
    Serial.print(recordedDistances[closestIndex]);
    Serial.println(" cm");
  }
}

// Function to display "Listo" message
int16_t x1, y1;
uint16_t w, h;

//void displayListo() {
  //display.getTextBounds("Listo", 0, 0, &x1, &y1, &w, &h);
 // display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
  //display.println("Listo");
 // display.display();
//}

// Function to display "Entrenando..." message
//void displayTrain() {
  //display.getTextBounds("Entrenando...", 0, 0, &x1, &y1, &w, &h);
  //display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
  //display.println("Entrenando...");
 // display.display();

