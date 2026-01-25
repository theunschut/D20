/*
 * Digital Multi-Dice for XIAO ESP32-C3 with GC9A01 Round Display
 *
 * Hardware:
 * - Seeed Studio XIAO ESP32-C3
 * - 1.28" Round TFT LCD (240x240, GC9A01 driver)
 * - 4 Buttons (Roll + Mode + Qty+/-)
 *
 * See Config.h for complete wiring details
 *
 * Required Libraries:
 * - Adafruit GFX Library
 * - Adafruit GC9A01A
 */

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>
#include "Config.h"
#include "DiceTypes.h"
#include "Display.h"
#include "GameState.h"
#include "ButtonHandler.h"
#include "BatteryMonitor.h"

// Display object - using hardware SPI
// Constructor with only CS, DC, RST enables hardware SPI on default pins
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Digital Multi-Dice ===");
  Serial.println("Initializing...");

  // Initialize all subsystems
  initButtons();
  initDisplay();
  initGameState();
  initBatteryMonitor();

  // Show welcome screen
  drawWelcomeScreen(currentDiceType);

  Serial.println("=== READY ===");
  Serial.println("Roll button: Roll dice");
  Serial.println("Mode button: Change dice type (long press for Adv/Dis)");
  Serial.println("Qty +/-: Change number of dice");
}

void loop() {
  updateButtons();
  updateBatteryReading();  // Throttled to every 5 seconds internally
  delay(10);
}
