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
  