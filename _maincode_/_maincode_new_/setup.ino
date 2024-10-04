void setup() {
  Serial.begin(9600);
  Wire.begin(); // Start the I2C communication

  myservo.attach(servoPin);
  myservo2.attach(servoPin2);
  pinMode(buttonPin, INPUT);
  pinMode(upbuttonPin, INPUT);
  pinMode(downbuttonPin, INPUT);

  // display set up
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(F("Bienvenidos"), 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 0);
  display.println(F("Bienvenidos"));

  display.setTextSize(2);
  display.getTextBounds(F("COMENZAR"), 0, 0, &x1, &y1, &w, &h);
  int16_t box_x = (SCREEN_WIDTH - w) / 2 - 4;
  int16_t box_y = 16 - 4;
  uint16_t box_w = w + 8;
  uint16_t box_h = h + 8;
  display.drawRect(box_x, box_y, box_w, box_h, SSD1306_WHITE);
  display.setCursor((SCREEN_WIDTH - w) / 2, 16);
  display.println(F("COMENZAR"));

  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println(F("Smart Motors Paraguay"));

  display.display();
  delay(2000);
  display.clearDisplay();

  checkMemory(); // Check free memory at the end of setup
}
