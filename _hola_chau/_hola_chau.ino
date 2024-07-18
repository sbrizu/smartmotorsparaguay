#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonPin = 5; 
int buttonState = 0;
int lastButtonState = LOW; 
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int pressCount = 0;
unsigned long lastPressTime = 0;
unsigned long doublePressDelay = 500;

void setup() {
  pinMode(buttonPin, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); // Address 0x3C for 128x32

  display.display();
  delay(2000);

  display.clearDisplay();      
  display.setTextColor(WHITE); 
  display.setTextSize(1);

  // Center "Bienvenidos Paraguay"
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds("Bienvenidos Paraguay", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 0);
  display.println("Bienvenidos Paraguay");

  // Center "Comencemos!" and draw rectangle around it
  display.getTextBounds("Comencemos!", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 16);
  display.println("Comencemos!");
  display.drawRect((SCREEN_WIDTH - w) / 2 - 2, 16 + y1 - 2, w + 4, h + 4, SSD1306_WHITE);

  display.display();            
  delay(2000);
  display.clearDisplay();
}

void loop() {
  int reading = digitalRead(buttonPin);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        if ((millis() - lastPressTime) < doublePressDelay) {
          pressCount++;
        } else {
          pressCount = 1;
        }
        lastPressTime = millis();

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);

        int16_t x1, y1;
        uint16_t w, h;

        if (pressCount == 1) {
          display.getTextBounds("Hola", 0, 0, &x1, &y1, &w, &h);
          display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
          display.println("Hola");
        } else if (pressCount == 2) {
          display.getTextBounds("Chau", 0, 0, &x1, &y1, &w, &h);
          display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
          display.println("Chau");
          display.drawRect((SCREEN_WIDTH - w) / 2 - 2, (SCREEN_HEIGHT - h) / 2 + y1 - 2, w + 4, h + 4, SSD1306_WHITE);
          pressCount = 0; // Reset the count after double press
        }

        display.display();
      }
    }
  }

  lastButtonState = reading;
}
