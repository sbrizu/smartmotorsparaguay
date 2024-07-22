#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonPin = 5;
bool startProgressBar = false;
int sensorPin = A1;
int sensorValue = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds("Bienvenidos", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 0);
  display.println("Bienvenidos");

  display.setTextSize(2);
  display.getTextBounds("COMENZAR", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 16);
  display.println("COMENZAR");
  display.drawRect((SCREEN_WIDTH - w) / 2 - 2, 16 + y1 - 2, w + 4, h + 4, SSD1306_WHITE);

  display.setTextSize(1);
  display.getTextBounds("Smart Motors Paraguay", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 50);
  display.println("Smart Motors Paraguay");
  display.display();
  delay(2000);
  display.clearDisplay();
}

int align2(int q) {  // Space in 100s and 10s
  if (q < 100) {
    display.print(" ");
  }
  if (q < 10) {
    display.print(" ");    
  }
}

int align3(int q) {  // Space in 1000s
  if (q < 1000) {
    display.print(" ");
  }
  align2(q);
}

void progressbar() {
  sensorValue = analogRead(sensorPin); 
  int perCent = float(sensorValue * 100.0 / 1018);

  display.clearDisplay();
  display.drawLine(0, 47, 0, 62, SSD1306_WHITE);
  display.fillRect(1, 50, perCent, 10, SSD1306_WHITE);
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(102, 50);
  align2(perCent);    
  display.print(perCent);
  display.println("%");
  display.fillRect(1, 11, 68, 33, SSD1306_BLACK); // Clear box
  display.setCursor(5, 30);
  display.setTextSize(1); 
  display.println("Intensidad de la luz");
  display.setTextSize(1);
  display.display();
  delay(100);
}

void loop() {
  if (digitalRead(buttonPin) == HIGH) { // Assuming button is active LOW
    startProgressBar = true;
  }

  if (startProgressBar) {
    progressbar();
  }
}
