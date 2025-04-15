void drawScaledBitmap(const unsigned char* bitmap, int originalWidth, int originalHeight, int newWidth, int newHeight, int x, int y) {
  for (int j = 0; j < newHeight; j++) {
    for (int i = 0; i < newWidth; i++) {
      // Calculate the corresponding pixel in the original bitmap
      int origX = i * originalWidth / newWidth;
      int origY = j * originalHeight / newHeight;
      
      // Get the color of the pixel from the original bitmap
      bool color = pgm_read_byte(bitmap + (origY * (originalWidth / 8) + origX / 8)) & (1 << (7 - origX % 8));
      
      // Draw the pixel on the display
      if (color) {
        display.drawPixel(x + i, y + j, WHITE);
      }
    }
  }
}

void displaySensorOptions(){
  drawScaledBitmap(lightSymbol, 128, 64, 55, 25, 2, 5);  // Scale lightSymbol to half size
  drawScaledBitmap(distanceSymbol, 128, 64, 64, 32, 0, 32);  // Scale distanceSymbol to half size
   // Set text size and color
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Print "Light Sensor" next to the light symbol
  display.setCursor(38, 15);
  display.println(F("Sensor 1"));
  
  // Print "Distance Sensor" next to the distance symbol
  display.setCursor(38, 40);
  display.println(F("Sensor 2"));
  
  // Display everything on the screen
  display.display();
}



void displayMotorPositions(int pos1, int pos2, bool playbackMode) {
  display.setTextSize(1);
  display.setTextColor(playbackMode ? SSD1306_BLACK : SSD1306_WHITE); // Change text color based on mode

  // Draw first box
  display.drawRect(10, 15, 50, 15, playbackMode ? SSD1306_BLACK : SSD1306_WHITE);
  display.setCursor(15, 18); // Adjusted position
  display.print(F("M1:"));
  display.print(pos1);

  // Draw second box
  display.drawRect(70, 15, 50, 15, playbackMode ? SSD1306_BLACK : SSD1306_WHITE);
  display.setCursor(75, 18); // Adjusted position
  display.print(F("M2:"));
  display.print(pos2);
}


void displaySensorValue(int sensorValue, const __FlashStringHelper* label, int motorPos1, int motorPos2, bool playbackMode) {
  int percentage = map(sensorValue, 0, 100, 0, 100);

  // Clear specific areas instead of the whole display
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, playbackMode ? SSD1306_WHITE : SSD1306_BLACK);

  // Display the motor positions
  displayMotorPositions(motorPos1, motorPos2, playbackMode);

  // Draw the progress bar
  display.drawLine(0, 47, 0, 62, playbackMode ? SSD1306_BLACK : SSD1306_WHITE);
  display.fillRect(1, 50, map(percentage, 0, 100, 0, SCREEN_WIDTH - 2), 10, playbackMode ? SSD1306_BLACK : SSD1306_WHITE);

  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(playbackMode ? SSD1306_BLACK : SSD1306_WHITE); // Change text color based on mode
  display.setCursor(102, 50);
  display.print(percentage);
  display.println(F("%"));

  // Adjusted position of the label
  display.setCursor(5, 35); // Lowered text position
  display.println(label);
  display.display();
}