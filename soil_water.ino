/*
 * Soil Moisture Monitoring & Automated Alert System
 * -------------------------------------------------
 * Component List:
 *  - Arduino Uno / Nano / Pro Mini
 *  - YL-69 Soil Moisture Sensor with LM393 Comparator Module
 *  - 0.96 inch I2C OLED Display (128x64 resolution, SSD1306 driver)
 *
 * Pin Connections:
 *  YL-69 Sensor Module:
 *    VCC -> 5V (or 3.3V)
 *    GND -> GND
 *    AO  -> Analog Pin A0
 *
 *  0.96" OLED Display (I2C):
 *    VCC -> 5V (or 3.3V)
 *    GND -> GND
 *    SCL -> A5 (Arduino Uno/Nano SCL)
 *    SDA -> A4 (Arduino Uno/Nano SDA)
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen Dimensions & I2C Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // Common I2C address for 0.96" SSD1306 OLED screens

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
const int SOIL_SENSOR_PIN = A0;

// Calibration Constants (Adjust these values based on actual sensor testing)
// YL-69 analog values decrease as soil moisture increases.
const int DRY_VALUE = 1023; // Analog reading in completely dry air / soil
const int WET_VALUE = 350;  // Analog reading when fully submerged in water

// Alert Threshold (Percentage)
const int WATER_ALERT_THRESHOLD = 30; // Triggers "NEEDS WATER" alert if moisture < 30%

// Number of samples to average for stable sensor readings
const int NUM_SAMPLES = 10;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  pinMode(SOIL_SENSOR_PIN, INPUT);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 OLED initialization failed!"));
    for (;;); // Stop execution if display fails
  }

  // Display initial splash screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 15);
  display.println(F("Soil Watering"));
  display.setCursor(25, 30);
  display.println(F("System"));
  display.setCursor(15, 48);
  display.println(F("Initializing..."));
  display.display();
  delay(2000);
}

void loop() {
  // Read and average analog sensor values
  long analogSum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    analogSum += analogRead(SOIL_SENSOR_PIN);
    delay(10);
  }
  int rawAnalog = analogSum / NUM_SAMPLES;

  // Map analog reading to moisture percentage (0% to 100%)
  int moisturePercent = map(rawAnalog, DRY_VALUE, WET_VALUE, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // Determine if watering is required
  bool needsWater = (moisturePercent < WATER_ALERT_THRESHOLD);

  // Serial Output for Debugging
  Serial.print(F("Raw Analog: "));
  Serial.print(rawAnalog);
  Serial.print(F(" | Moisture: "));
  Serial.print(moisturePercent);
  Serial.print(F("% | Status: "));
  if (needsWater) {
    Serial.println(F("NEEDS WATER"));
  } else {
    Serial.println(F("MOISTURE OK"));
  }

  // Update OLED Display
  display.clearDisplay();

  // Header Box
  display.drawRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 4);
  display.println(F("SOIL MOISTURE"));

  // Moisture Percentage Display
  display.setTextSize(2);
  display.setCursor(10, 22);
  display.print(moisturePercent);
  display.print(F("%"));

  // Progress Bar Outline & Fill
  int barWidth = map(moisturePercent, 0, 100, 0, 50);
  display.drawRect(68, 24, 52, 12, SSD1306_WHITE);
  display.fillRect(69, 25, barWidth, 10, SSD1306_WHITE);

  // Alert Banner
  if (needsWater) {
    // Highlighted Alert Box for "NEEDS WATER!"
    display.fillRect(0, 46, 128, 18, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Inverted text
    display.setTextSize(1);
    display.setCursor(14, 51);
    display.println(F("ALERT: NEEDS WATER!"));
  } else {
    // Normal Status Box for "MOISTURE OK"
    display.drawRect(0, 46, 128, 18, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(22, 51);
    display.println(F("STATUS: WATER OK"));
  }

  // Render to display screen
  display.display();

  // Update interval
  delay(1000);
}
