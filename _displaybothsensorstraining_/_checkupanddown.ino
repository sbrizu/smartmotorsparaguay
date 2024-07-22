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
