
void trainDistanceSensor() {
for (int i = 0; i < 3; i++) {
  potValues1[i] = analogRead(potPin);
  potValues2[i] = analogRead(potPin2);
  delay(0.1); // Reduce delay to 50ms
}

// Sort arrays
for (int i = 0; i < 2; i++) {
  for (int j = i + 1; j < 3; j++) {
    if (potValues1[i] > potValues1[j]) {
      int temp = potValues1[i];
      potValues1[i] = potValues1[j];
      potValues1[j] = temp;
    }
    if (potValues2[i] > potValues2[j]) {
      int temp = potValues2[i];
      potValues2[i] = potValues2[j];
      potValues2[j] = temp;
    }
  }
}

// Use the median value
val = map(potValues1[1], 0, 1023, 0, 180);
val2 = map(potValues2[1], 0, 1023, 0, 180);


  RangeInCentimeters = ultrasonic.MeasureInCentimeters();

  int distancePercentage = map(RangeInCentimeters, 0, 100, 0, 100);
  
  displaySensorValue(distancePercentage, F("Distancia"), val, val2, playingBack);

  if (buttonState == HIGH && lastButtonState == LOW) {
    buttonPressStartTime = millis();
  } else if (buttonState == LOW && lastButtonState == HIGH) {
    unsigned long pressDuration = millis() - buttonPressStartTime;
    if (pressDuration >= 2000) {
      if (recordCount > 0) {
        playingBack = true;
        display.clearDisplay(); // Clear display when switching to playback mode
      }
    } else if (pressDuration >= 100 && pressDuration < 2000) {
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
