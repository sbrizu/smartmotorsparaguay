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


  checkMemory(); // Check free memory at the end of setup
}
