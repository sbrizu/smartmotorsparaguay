

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

