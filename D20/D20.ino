/*
 * Digital Multi-Dice for XIAO ESP32-C3 with GC9A01 Round Display
 *
 * Hardware:
 * - Seeed Studio XIAO ESP32-C3
 * - 1.28" Round TFT LCD (240x240, GC9A01 driver)
 * - MCP23017 I2C GPIO Expander (4 buttons)
 * - MPU6050 I2C Accelerometer (shake-to-roll)
 * - MAX17048 I2C Fuel Gauge (battery)
 * - MicroSD Card (roll animations)
 *
 * See Config.h for pin assignments and wiring reference.
 *
 * Required Libraries:
 * - Adafruit GFX Library
 * - Adafruit GC9A01A
 * - Adafruit MCP23X17
 * - Adafruit MPU6050
 * - SparkFun MAX1704x Fuel Gauge
 * - SD (built-in)
 */

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <Wire.h>
#include <SPI.h>
#include "Config.h"
#include "DiceTypes.h"
#include "Display.h"
#include "GameState.h"
#include "ButtonHandler.h"
#include "BatteryMonitor.h"
#include "MotionDetector.h"
#include "SDAnimations.h"

// Display — D20Display subclass exposes setWindow() for SD frame streaming
D20Display tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Digital Multi-Dice ===");
  Serial.println("Initializing...");

  // I2C bus must be started before any I2C device
  Wire.begin(I2C_SDA, I2C_SCL);

  // Display first — also starts SPI (needed before SD init)
  initDisplay();

  // I2C peripherals
  initButtons();            // MCP23017
  initMotionDetector();     // MPU6050
  initBatteryMonitor();     // MAX17048

  // SPI peripheral (SD card — SPI already running from display init)
  initSDAnimations();

  // Game logic
  initGameState();

  // Show welcome screen
  drawWelcomeScreen(currentDiceType);
  resetActivityTimer();

  Serial.println("=== READY ===");
  Serial.println("Shake to roll | Mode: dice type (short) / adv-dis (long)");
  Serial.println("Qty +/-: number of dice");
}

void loop() {
  updateButtons();
  updateMotionDetector();
  updateBatteryReading();   // Throttled internally
  updateBacklight();        // Auto-dim
  checkAutoReturn();
  delay(10);
}
