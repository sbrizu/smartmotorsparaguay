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
const int buttonPin = 5;
const int ultrasonicPin = 4;

Servo myservo;
Servo myservo2;
Ultrasonic ultrasonic(ultrasonicPin);


int val;
int val2;
long RangeInCentimeters;
int buttonState;
int lastButtonState = LOW;
unsigned long buttonPressStartTime;

const int maxRecords = 100;
int recordedPositions1 [maxRecords];
int recordedPositions2 [maxRecords];
long recordedDistances[maxRecords];
int recordCount = 0;
bool recording = false;
bool playingBack = false;
long targetDistance = -1;

void setup() {
  
  Serial.begin(9600);
  myservo.attach(servoPin);
  myservo2.attach(servoPin2);
  pinMode(buttonPin, INPUT);
  
}

void loop() {
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
        recordData(val, val, RangeInCentimeters); // Record the servo position and distance
       
        // Stop recording after setting the target distance
      }
    }
  }

  // Update the last button state
  lastButtonState = buttonState;

  // If playing back, set servo based on recorded distances
  if (playingBack) {
    playbackServoPosition(RangeInCentimeters);
   
  } else {
    // Set the servo position if not playing back
    myservo.write(val);
    myservo2.write(val);
  }

  // Small delay to stabilize readings
  delay(15);
}

// Function to record data
void recordData(int servoPosition, int servoPosition2, long distance) {
  if (recordCount < maxRecords) {
    recordedPositions1[recordCount] = servoPosition;
    recordedPositions2[recordCount] = servoPosition2;
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
 myservo.write(recordedPositions1[closestIndex]);
    myservo2.write(recordedPositions2[closestIndex]);
    Serial.print("Playback Servo Position 1: ");
    Serial.print(recordedPositions1[closestIndex]);
    Serial.print("and Servo Position 2: ");
    Serial.print(recordedPositions2[closestIndex]);
    Serial.print(" degrees, Distance: ");
    Serial.print(recordedDistances[closestIndex]);
    Serial.println(" cm");
  }
}


