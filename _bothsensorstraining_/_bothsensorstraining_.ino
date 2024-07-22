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
  Serial.println("Setup complete");
}


  // training with light sensor
void trainLightSensor () {
  val = analogRead(potPin);
  val2 = analogRead(potPin2);
//potentiometer and servo position
  val = map(val, 0, 1023, 0, 180);
  val2 = map(val2, 0, 1023, 0, 180);

  int raw_light = analogRead(light_sensor);
  lightLevel = map(raw_light, 0, 1023, 0, 100);
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
        actualLightLevel = lightLevel;
        Serial.print("actual light level set to: ");
        Serial.println(actualLightLevel);
        recordlightData(val, val2, lightLevel); // Record the servo position and light level
      }
    }
  }

  // Update the last button state
  lastButtonState = buttonState;

  // If playing back, set servo based on recorded light levels
  if (playingBack) {
    playbacklightServoPosition(lightLevel);
  } else {
    // Set the servo position if not playing back
    myservo.write(val);
    myservo2.write(val2);
  }

  // Small delay to stabilize readings
  delay(15);
}

// Function to record data
void recordlightData(int servoPosition, int servoPosition2, int lightLevel) {
  if (recordCount < maxRecords) {
    recordedPositions1[recordCount] = servoPosition;
    recordedPositions2[recordCount] = servoPosition2;
    recordedLightLevels[recordCount] = lightLevel;
    recordCount++;
    Serial.print("Recorded Servo Position 1: ");
    Serial.print(servoPosition);
    Serial.print("and servo Position 2: ");
    Serial.print(servoPosition2);
    Serial.print(" degrees, Light Level: ");
    Serial.print(lightLevel);
    Serial.println(" %");
  } else {
    Serial.println("Memory Full: Cannot record more data.");
  }
}

// Function to play back recorded data based on current distance
void playbacklightServoPosition(int currentLightLevel) {
  // Find the closest recorded light level to the current light level
  int closestIndex = -1;
  int closestLightLevel = 999999; // Arbitrary large number for initial comparison

  for (int i = 0; i < recordCount; i++) {
    int lightLevelDiff = abs(currentLightLevel - recordedLightLevels[i]);
    if (lightLevelDiff < closestLightLevel) {
      closestLightLevel = lightLevelDiff;
      closestIndex = i;
    }
  }

  // Set the servo to the position corresponding to the closest distance
  if (closestIndex != -1) {
    myservo.write(recordedPositions1[closestIndex]);
    myservo2.write(recordedPositions2[closestIndex]);
    Serial.print("Playback Servo Position 1: ");
    Serial.print(recordedPositions1[closestIndex]);
    Serial.print("Playback Servo Position 2: ");
    Serial.print(recordedPositions2[closestIndex]);
    Serial.print(" degrees, Light Level: ");
    Serial.print(recordedLightLevels[closestIndex]);
    Serial.println(" %");
  }
}
  
void trainDistanceSensor () {
  val = analogRead(potPin);
  val2 = analogRead(potPin2);

  // Map the potentiometer value to the servo position (0-180 degrees)
  val = map(val, 0, 1023, 0, 180);
  val2 = map(val2, 0, 1023, 0, 180);

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
        recordDistanceData(val, val2, RangeInCentimeters); // Record the servo position and distance
       
        // Stop recording after setting the target distance
      }
    }
  }

  // Update the last button state
  lastButtonState = buttonState;

  // If playing back, set servo based on recorded distances
  if (playingBack) {
    playbackDistanceServoPosition(RangeInCentimeters);
   
  } else {
    // Set the servo position if not playing back
    myservo.write(val);
    myservo2.write(val2);
  }

  // Small delay to stabilize readings
  delay(15);
}

void recordDistanceData(int servoPosition, int servoPosition2, long distance) {
  if (recordCount < maxRecords) {
    recordedPositions1[recordCount] = servoPosition;
    recordedPositions2[recordCount] = servoPosition2;
    recordedDistances[recordCount] = distance;
    recordCount++;
    Serial.print("Recorded Servo Position 1:  ");
    Serial.print(servoPosition);
    Serial.print("Servo 2 ");
    Serial.print(servoPosition2);
    Serial.print(" degrees, Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("Memory Full: Cannot record more data.");
  }
}

void playbackDistanceServoPosition(long currentDistance) {
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

void loop() {
  // Read the pushbutton state
  buttonState = digitalRead(buttonPin);
  upbuttonState = digitalRead(upbuttonPin);
  downbuttonState = digitalRead(downbuttonPin);

  // Only allow sensor selection when not in playback or training mode
  if (!playingBack && !trainingmode && !playbackmode) {
    if (upbuttonState == HIGH) {
      useLightSensor = true;
      useDistanceSensor = false;
      trainingmode = true;
      Serial.println("Using Light Sensor for training.");
    } else if (downbuttonState == HIGH) {
      useDistanceSensor = true;
      useLightSensor = false;
      trainingmode = true;
      Serial.println("Using Distance Sensor for training.");
    }
  }

  if (useLightSensor && trainingmode) {
    trainLightSensor();
  }

  // Distance sensor functionality
  if (useDistanceSensor && trainingmode) {
    trainDistanceSensor();
  }

  // Set playback mode based on button press duration
  if (buttonState == LOW && lastButtonState == HIGH) {
    unsigned long pressDuration = millis() - buttonPressStartTime;
    if (pressDuration >= 2000) {
      playbackmode = true;
      trainingmode = false;
      playingBack = true;
      Serial.println("Starting playback mode.");
    }
  }

  // Ensure playbackmode flag is used to manage playback
  if (playbackmode) {
    if (useLightSensor) {
      playbacklightServoPosition(lightLevel);
    }
    if (useDistanceSensor) {
      playbackDistanceServoPosition(RangeInCentimeters);
    }
  }

  // Update the last button state
  lastButtonState = buttonState;
}


//  // Read the distance from the ultrasonic sensor
  //angeInCentimeters = ultrasonic.MeasureInCentimeters();
// read  light sensor
 // int raw_light = analogRead(light_sensor);
 // lightLevel = map(raw_light, 0, 1023, 0, 100);
