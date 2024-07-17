#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

void setup() {

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  display.display();
  delay(2000);

  display.clearDisplay();      //this line to clear previous logo
  display.setTextColor(WHITE); //without this no display
  display.print("Hola");//your TEXT here
  display.display();            //to shows or update your TEXT
}

void loop() {

 }