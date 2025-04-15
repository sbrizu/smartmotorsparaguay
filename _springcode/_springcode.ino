#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_PIN 7
#define NUM_LEDS 4
#define POT_PIN A2
#define SERVO_PIN 5
#define MAIN_BUTTON 2
#define UP_BUTTON 3
#define DOWN_BUTTON 4
#define ANALOG_SENSOR_PIN A1
#define ULTRASONIC_PIN 8

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Servo myservo;

enum SystemState { SENSOR_SELECT, CONTROL };
enum SubState { SV_NUM, SV_BRIGHT, SV_COLOR };

SystemState currentState = SENSOR_SELECT;
SubState currentSubState = SV_NUM;

bool useAnalogSensor = true;

int lastMainButtonState = HIGH;
int lastUpButtonState = HIGH;
int lastDownButtonState = HIGH;

unsigned long lastMemoryCheck = 0;
unsigned long sensorDisplayTimer = 0;
unsigned long buttonPressStartTime = 0;

int lastSensorReading = 0; // used for progress bar

void checkMemory();
void displaySensorSelection(bool analogSelected);
void displaySubState(SubState state, const char* colorName = "");
void handleControlSubState();

void setup() {
  Serial.begin(9600);
  pinMode(MAIN_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);

  myservo.attach(SERVO_PIN);
  strip.begin();
  strip.show();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
  checkMemory();
}

void loop() {
  int mainButton = digitalRead(MAIN_BUTTON);
  int upButton = digitalRead(UP_BUTTON);
  int downButton = digitalRead(DOWN_BUTTON);

  // Dynamically determine sensor type
  
  long duration = 0;
  int analogVal = analogRead(ANALOG_SENSOR_PIN);
  useAnalogSensor = false;

  if (analogVal > 100) {
    useAnalogSensor = true;
    lastSensorReading = analogVal;
    
  } else {
    pinMode(ULTRASONIC_PIN, OUTPUT);
    digitalWrite(ULTRASONIC_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_PIN, LOW);
    pinMode(ULTRASONIC_PIN, INPUT);
    duration = pulseIn(ULTRASONIC_PIN, HIGH, 30000);
    if (duration > 0) {
      useAnalogSensor = false;
      
      lastSensorReading = duration / 58.0;
    }
  }

  switch (currentState) {
    case SENSOR_SELECT:
      if (millis() - sensorDisplayTimer > 500) {
        displaySensorSelection(useAnalogSensor);
        sensorDisplayTimer = millis();

        
      }

      if (mainButton == LOW && lastMainButtonState == HIGH) {
        buttonPressStartTime = millis();
      } else if (mainButton == HIGH && lastMainButtonState == LOW) {
        unsigned long pressDuration = millis() - buttonPressStartTime;
        if (pressDuration >= 1000) {
          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(20, 25);
          display.setTextColor(SSD1306_WHITE);
          display.println(F("Sensor selected"));
          display.display();
          delay(1000);
          currentState = CONTROL;
          displaySubState(currentSubState);
        }
      }
      break;

    case CONTROL:
      handleControlSubState();
      break;
  }

  lastMainButtonState = mainButton;
  lastUpButtonState = upButton;
  lastDownButtonState = downButton;

  if (millis() - lastMemoryCheck > 10000) {
    checkMemory();
    lastMemoryCheck = millis();
  }

  delay(50);
}

void checkMemory() {
  extern int __heap_start, *__brkval;
  int v;
  int free_memory = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
  Serial.print(F("Free memory: "));
  Serial.println(free_memory);
}

void displaySensorSelection(bool analogSelected) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 10);
  display.println(F("Select sensor"));
  display.setTextSize(2);
  display.setCursor(25, 25);
  if (analogSelected) {
    display.println(F("Light"));
  } else {
    display.println(F("Distance"));
  }

  if (analogSelected && lastSensorReading < 10) {
    display.setTextSize(1);
    display.setCursor(10, 52);
    display.println(F("No analog sensor detected"));
  } else if (!analogSelected && lastSensorReading == 0) {
    display.setTextSize(1);
    display.setCursor(10, 52);
    display.println(F("No distance sensor detected"));
  }

  display.display();
}

void displaySubState(SubState state, const char* colorName) {
  display.clearDisplay();
  display.fillTriangle(5, 15, 15, 15, 10, 5, SSD1306_WHITE);    // Up arrow
  display.fillTriangle(5, 50, 15, 50, 10, 60, SSD1306_WHITE);   // Down arrow
  display.setTextSize(1);
  display.setCursor(40, 20);
  if (state == SV_NUM) display.println(F("SV + Number"));
  else if (state == SV_BRIGHT) display.println(F("SV + Bright"));
  else {
    display.println(F("SV + Color"));
    display.setCursor(40, 40);
    display.print(F("Color: "));
    display.println(colorName);
  }

  // Draw progress bar for sensor reading
  int barWidth = map(constrain(lastSensorReading, 1, 100), 1, 100, 0, SCREEN_WIDTH - 42);
  display.drawRect(20, 56, SCREEN_WIDTH - 40, 6, SSD1306_WHITE);
  display.fillRect(21, 57, constrain(barWidth - 40, 0, SCREEN_WIDTH - 42), 4, SSD1306_WHITE);

  display.display();
}

void handleControlSubState() {
  // Refresh sensor reading inside substates
  long duration = 0;
  int analogVal = analogRead(ANALOG_SENSOR_PIN);
  if (analogVal > 100) {
    useAnalogSensor = true;
    lastSensorReading = analogVal;
  } else {
    pinMode(ULTRASONIC_PIN, OUTPUT);
    digitalWrite(ULTRASONIC_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_PIN, LOW);
    pinMode(ULTRASONIC_PIN, INPUT);
    duration = pulseIn(ULTRASONIC_PIN, HIGH, 30000);
    if (duration > 0) {
      useAnalogSensor = false;
      lastSensorReading = duration / 58.0;
    }
  }
  int potValue = analogRead(POT_PIN);
  int angle = map(potValue, 0, 1023, 0, 180);
  myservo.write(angle);

  const char* colorName = "";

  switch (currentSubState) {
    case SV_NUM: {
      int numLEDs = map(potValue, 0, 1023, 0, NUM_LEDS);
      for (int i = 0; i < NUM_LEDS; i++) {
        if (i < numLEDs) {
          strip.setPixelColor(i, strip.Color(255, 0, 0));
        } else {
          strip.setPixelColor(i, 0);
        }
      }
      break;
    }
    case SV_BRIGHT: {
      int brightness = map(potValue, 0, 1023, 0, 255);
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(brightness, 0, 0));
      }
      break;
    }
    case SV_COLOR: {
      int hue = map(potValue, 0, 1023, 0, 255);
      if (hue < 20) colorName = "Red";
      else if (hue < 45) colorName = "Orange";
      else if (hue < 75) colorName = "Yellow";
      else if (hue < 110) colorName = "Green";
      else if (hue < 160) colorName = "Cyan";
      else if (hue < 200) colorName = "Blue";
      else if (hue < 240) colorName = "Purple";
      else colorName = "White";

      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.ColorHSV(hue * 256, 255, 255));
      }
      break;
    }
  }
  strip.show();
  displaySubState(currentSubState, colorName);  // Now always updates progress bar after handling LED/servo
}
