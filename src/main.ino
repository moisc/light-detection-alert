/*
 * Light Detection Alert System for Heltec LoRa32 v2
 *
 * Features:
 * - Reads analog light sensor (currently potentiometer, can swap to photoresistor)
 * - Sends Discord webhook notifications when threshold is crossed
 * - OLED display shows current readings and status
 * - Serial commands for calibration and threshold adjustment
 * - Debouncing to prevent notification spam
 *
 * Hardware:
 * - Heltec LoRa32 v2
 * - Potentiometer/Photoresistor on GPIO 36 (ADC1_0)
 * - Built-in OLED (128x64, SSD1306)
 *
 * Serial Commands:
 * - 'c' or 'C': Enter calibration mode
 * - 't' or 'T': Set threshold manually
 * - 's' or 'S': Show current status
 */

#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"

// Pin definitions
#define SENSOR_PIN 36        // ADC1_0 for potentiometer/photoresistor
#define OLED_SDA 4          // Heltec LoRa32 v2 OLED SDA
#define OLED_SCL 15         // Heltec LoRa32 v2 OLED SCL
#define OLED_RST 16         // Heltec LoRa32 v2 OLED Reset

// OLED Display setup
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA, OLED_RST);

// Sensor and threshold settings
int sensorValue = 0;
int threshold = 2000;        // Default threshold (0-4095)
bool alertActive = false;
unsigned long lastAlertTime = 0;
const unsigned long DEBOUNCE_TIME = 60000;  // 60 seconds between alerts

// WiFi connection
bool wifiConnected = false;

// Calibration mode
bool calibrationMode = false;
int calibrationReadings[10];
int calibrationIndex = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=================================");
  Serial.println("Light Detection Alert System");
  Serial.println("Heltec LoRa32 v2");
  Serial.println("=================================\n");

  // Initialize OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 15, "Light Detector");
  u8g2.drawStr(0, 30, "Initializing...");
  u8g2.sendBuffer();

  // Setup ADC for sensor reading
  analogReadResolution(12);  // 12-bit resolution (0-4095)
  analogSetAttenuation(ADC_11db);  // Full range

  // Connect to WiFi
  connectWiFi();

  // Display ready message
  displayStatus();

  Serial.println("\nCommands:");
  Serial.println("  c/C - Calibrate threshold");
  Serial.println("  t/T - Set threshold manually");
  Serial.println("  s/S - Show status\n");
}

void loop() {
  // Read sensor value
  sensorValue = analogRead(SENSOR_PIN);

  // Check for serial commands
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    handleSerialCommand(cmd);
  }

  // Handle calibration mode
  if (calibrationMode) {
    handleCalibration();
    return;
  }

  // Check threshold crossing
  if (sensorValue < threshold && !alertActive) {
    // Light level dropped below threshold (darker)
    if (millis() - lastAlertTime > DEBOUNCE_TIME) {
      triggerAlert();
      alertActive = true;
      lastAlertTime = millis();
    }
  } else if (sensorValue >= threshold) {
    // Light level back above threshold (brighter)
    alertActive = false;
  }

  // Update display
  displayStatus();

  delay(500);  // Update every 500ms
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    Serial.println("\nWiFi connection failed!");
    Serial.println("Notifications will not work without WiFi.");
  }
}

void triggerAlert() {
  Serial.println("\n!!! ALERT TRIGGERED !!!");
  Serial.print("Sensor value: ");
  Serial.print(sensorValue);
  Serial.print(" (Threshold: ");
  Serial.print(threshold);
  Serial.println(")");

  if (wifiConnected && WiFi.status() == WL_CONNECTED) {
    sendDiscordNotification();
  } else {
    Serial.println("Cannot send notification - WiFi not connected");
  }
}

void sendDiscordNotification() {
  HTTPClient http;

  Serial.println("Sending Discord notification...");

  http.begin(DISCORD_WEBHOOK_URL);
  http.addHeader("Content-Type", "application/json");

  // Create Discord message
  String message = "{\"content\":\"🚨 **Light Alert Triggered!**\\n";
  message += "Sensor reading: **" + String(sensorValue) + "** / 4095\\n";
  message += "Threshold: **" + String(threshold) + "**\\n";
  message += "Status: Light level dropped below threshold (darker)\"}";

  int httpResponseCode = http.POST(message);

  if (httpResponseCode > 0) {
    Serial.print("Notification sent! Response: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending notification: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void displayStatus() {
  u8g2.clearBuffer();

  // Title
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "Light Detector");

  // WiFi status
  u8g2.setFont(u8g2_font_6x10_tr);
  if (wifiConnected && WiFi.status() == WL_CONNECTED) {
    u8g2.drawStr(100, 10, "WiFi");
  } else {
    u8g2.drawStr(95, 10, "NoWiFi");
  }

  // Draw separator line
  u8g2.drawLine(0, 12, 128, 12);

  // Sensor value
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 25, "Reading:");
  char valueStr[10];
  sprintf(valueStr, "%d", sensorValue);
  u8g2.drawStr(60, 25, valueStr);

  // Threshold
  u8g2.drawStr(0, 38, "Threshold:");
  char threshStr[10];
  sprintf(threshStr, "%d", threshold);
  u8g2.drawStr(60, 38, threshStr);

  // Status indicator
  u8g2.drawStr(0, 51, "Status:");
  if (alertActive) {
    u8g2.drawStr(60, 51, "ALERT!");
  } else if (sensorValue < threshold) {
    u8g2.drawStr(60, 51, "Below");
  } else {
    u8g2.drawStr(60, 51, "Normal");
  }

  // Visual bar graph
  int barWidth = map(sensorValue, 0, 4095, 0, 128);
  u8g2.drawFrame(0, 55, 128, 8);
  u8g2.drawBox(0, 55, barWidth, 8);

  u8g2.sendBuffer();
}

void handleSerialCommand(char cmd) {
  switch (cmd) {
    case 'c':
    case 'C':
      startCalibration();
      break;

    case 't':
    case 'T':
      setThresholdManual();
      break;

    case 's':
    case 'S':
      printStatus();
      break;

    case '\n':
    case '\r':
      // Ignore newlines
      break;

    default:
      Serial.println("Unknown command. Use c/C, t/T, or s/S");
  }
}

void startCalibration() {
  calibrationMode = true;
  calibrationIndex = 0;
  Serial.println("\n=== CALIBRATION MODE ===");
  Serial.println("Adjust sensor to desired threshold level");
  Serial.println("Taking 10 readings over 5 seconds...");
}

void handleCalibration() {
  if (calibrationIndex < 10) {
    calibrationReadings[calibrationIndex] = analogRead(SENSOR_PIN);

    Serial.print("Reading ");
    Serial.print(calibrationIndex + 1);
    Serial.print("/10: ");
    Serial.println(calibrationReadings[calibrationIndex]);

    // Update display
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 15, "CALIBRATING...");
    char msg[20];
    sprintf(msg, "Reading %d/10", calibrationIndex + 1);
    u8g2.drawStr(0, 30, msg);
    sprintf(msg, "Value: %d", calibrationReadings[calibrationIndex]);
    u8g2.drawStr(0, 45, msg);
    u8g2.sendBuffer();

    calibrationIndex++;
    delay(500);
  } else {
    // Calculate average
    long sum = 0;
    for (int i = 0; i < 10; i++) {
      sum += calibrationReadings[i];
    }
    threshold = sum / 10;

    Serial.println("\nCalibration complete!");
    Serial.print("New threshold set to: ");
    Serial.println(threshold);

    calibrationMode = false;
    alertActive = false;  // Reset alert state
  }
}

void setThresholdManual() {
  Serial.println("\nEnter new threshold value (0-4095):");

  // Wait for input
  while (!Serial.available()) {
    delay(100);
  }

  int newThreshold = Serial.parseInt();

  if (newThreshold >= 0 && newThreshold <= 4095) {
    threshold = newThreshold;
    Serial.print("Threshold updated to: ");
    Serial.println(threshold);
    alertActive = false;  // Reset alert state
  } else {
    Serial.println("Invalid threshold. Must be 0-4095");
  }

  // Clear serial buffer
  while (Serial.available()) {
    Serial.read();
  }
}

void printStatus() {
  Serial.println("\n=== CURRENT STATUS ===");
  Serial.print("Sensor Reading: ");
  Serial.print(sensorValue);
  Serial.println(" / 4095");
  Serial.print("Threshold: ");
  Serial.println(threshold);
  Serial.print("Alert Active: ");
  Serial.println(alertActive ? "YES" : "NO");
  Serial.print("WiFi Connected: ");
  Serial.println((wifiConnected && WiFi.status() == WL_CONNECTED) ? "YES" : "NO");
  if (wifiConnected && WiFi.status() == WL_CONNECTED) {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
  Serial.print("Time since last alert: ");
  Serial.print((millis() - lastAlertTime) / 1000);
  Serial.println(" seconds");
  Serial.println("====================\n");
}

/*
 * NOTE FOR PHOTORESISTOR SWAP:
 *
 * When swapping the potentiometer for a photoresistor, connect as follows:
 *
 * Voltage Divider Circuit:
 * 3.3V --- [10kΩ Resistor] --- GPIO36 --- [Photoresistor] --- GND
 *
 * This creates a voltage divider where:
 * - More light = lower resistance = higher voltage = higher ADC reading
 * - Less light = higher resistance = lower voltage = lower ADC reading
 *
 * You may need to adjust the threshold after swapping components.
 * Use the calibration mode (send 'c' via serial) to set a new threshold.
 *
 * Alternative configuration (for opposite behavior):
 * 3.3V --- [Photoresistor] --- GPIO36 --- [10kΩ Resistor] --- GND
 * - More light = lower resistance = lower voltage = lower ADC reading
 * - Less light = higher resistance = higher voltage = higher ADC reading
 */
