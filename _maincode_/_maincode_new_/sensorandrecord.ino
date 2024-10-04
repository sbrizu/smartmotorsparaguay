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
int potValues1[3]; 
int potValues2[3]; 


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
bool initialized = false;
bool displayOptions = false;

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

  if (!initialized) {
    if (buttonState == HIGH) {
      displayOptions = true;
      initialized = true;
      displaySensorOptions();
      delay(2000); // Debounce delay
    }
  } else if (displayOptions) {
    if (downbuttonState == HIGH) {
      useDistanceSensor = true;
      trainingmode = true;
      useLightSensor = false;
      displayOptions = false;
    } else if (upbuttonState == HIGH) {
      useLightSensor = true;
      trainingmode = true;
      useDistanceSensor = false;
      displayOptions = false;
    }
  } else {
    if (useDistanceSensor && trainingmode) {
      trainDistanceSensor();
    }
    if (useLightSensor && trainingmode) {
      trainLightSensor();
    }
  }

  // Check memory periodically
  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 10000) { // every 10 seconds
    checkMemory();
    lastMemoryCheck = millis();
  }
}
