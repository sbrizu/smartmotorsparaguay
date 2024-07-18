#include <Servo.h>

// Constants for pin connections
const int servoPin = 2;
const int servoPin2 = 3;
const int potPin = A3;
const int buttonPin = 5;
const int light_sensor = A1; 

// Create servo object
Servo myservo;
Servo myservo2;

// Variables for sensor readings
int val;
int val2;
int lightLevel;
int buttonState;
int lastButtonState = LOW;
unsigned long buttonPressStartTime;

// Variables for recording data
const int maxRecords = 100;
int recordedPositions1 [maxRecords];
int recordedPositions2 [maxRecords];
int recordedLightLevels[maxRecords];
int recordCount = 0;
bool recording = false;
bool playingBack = false;
int actualLightLevel = -1;

// Setup function
void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Attach the servo to the pin
  myservo.attach(servoPin);
  myservo2.attach(servoPin2);

  // Initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);
}

// Loop function
void loop() {
  // Read the value from the potentiometer
  val = analogRead(potPin);

  // Map the potentiometer value to the servo position (0-180 degrees)
  val = map(val, 0, 1023, 0, 180);

  // Read the light level from the light sensor
  int raw_light = analogRead(light_sensor);
  lightLevel = map(raw_light, 0, 1023, 0, 100);

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
        actualLightLevel = lightLevel;
        Serial.print("actual light level set to: ");
        Serial.println(actualLightLevel);
        recordData(val, val, lightLevel); // Record the servo position and light level
      }
    }
  }

  // Update the last button state
  lastButtonState = buttonState;

  // If playing back, set servo based on recorded light levels
  if (playingBack) {
    playbackServoPosition(lightLevel);
  } else {
    // Set the servo position if not playing back
    myservo.write(val);
    myservo2.write(val);
  }

  // Small delay to stabilize readings
  delay(15);
}

// Function to record data
void recordData(int servoPosition, int servoPosition2, int lightLevel) {
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

// Function to play back recorded data based on current light level
void playbackServoPosition(int currentLightLevel) {
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

  // Set the servo to the position corresponding to the closest light level
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
