/*
 * ButtonHandler.cpp - Button input via MCP23017 I2C GPIO expander
 *
 * All 4 buttons are on MCP23017 Port A (GPA0-GPA3) with internal pull-ups.
 * Buttons pull LOW when pressed.
 * Port A is polled each loop iteration; transitions are detected in software.
 * MCP23017 INTA pin (GPIO20) is configured as INPUT_PULLUP for future
 * interrupt-based sleep/wake but is not used for button detection here.
 */

#include "ButtonHandler.h"
#include "GameState.h"
#include "Config.h"
#include "Display.h"
#include <Adafruit_MCP23X17.h>
#include <Arduino.h>

static Adafruit_MCP23X17 mcp;
static bool mcpInitialized = false;

// Last known port state (all HIGH = all released at init)
static uint8_t lastPortA = 0xFF;

// Debounce: ignore repeated presses within this window
static unsigned long lastPressTime[4] = {0, 0, 0, 0};
static const unsigned long BUTTON_DEBOUNCE_MS = 100;

// Mode button long-press state machine
static bool modePending   = false;
static unsigned long modePressTime = 0;
static bool modeLongDone  = false;  // true once long-press action has fired

void initButtons() {
  // Configure MCP23017 interrupt pin on ESP32 (future use for sleep/wake)
  pinMode(MCP_INT_PIN, INPUT_PULLUP);

  // Initialize MCP23017 on I2C (default address 0x20)
  if (!mcp.begin_I2C()) {
    Serial.println("ERROR: MCP23017 not found on I2C!");
    return;
  }

  // Configure button pins as inputs with internal pull-ups
  mcp.pinMode(MCP_BTN_SPARE,     INPUT_PULLUP);
  mcp.pinMode(MCP_BTN_MODE,      INPUT_PULLUP);
  mcp.pinMode(MCP_BTN_QTY_PLUS,  INPUT_PULLUP);
  mcp.pinMode(MCP_BTN_QTY_MINUS, INPUT_PULLUP);

  mcpInitialized = true;

  // Read initial state so we don't fire spurious transitions on first loop
  lastPortA = mcp.readGPIO(0);

  Serial.println("MCP23017 buttons initialized");
}

void updateButtons() {
  if (!mcpInitialized) return;

  uint8_t portA = mcp.readGPIO(0);  // Read all 8 pins of Port A in one I2C transaction
  unsigned long now = millis();

  // Detect HIGH→LOW (press) transitions for each button
  for (int i = 0; i < 4; i++) {
    bool currentlyPressed = !((portA >> i) & 1);   // LOW = pressed
    bool wasPressed        = !((lastPortA >> i) & 1);

    if (currentlyPressed && !wasPressed) {
      // Rising edge of press — apply debounce
      if (now - lastPressTime[i] < BUTTON_DEBOUNCE_MS) continue;
      lastPressTime[i] = now;

      resetActivityTimer();  // Any button press resets the backlight dim timer

      switch (i) {
        case MCP_BTN_SPARE:
          Serial.println("Spare button pressed");
          break;

        case MCP_BTN_MODE:
          // Start long-press detection
          modePending   = true;
          modePressTime = now;
          modeLongDone  = false;
          break;

        case MCP_BTN_QTY_PLUS:
          increaseDiceQuantity();
          break;

        case MCP_BTN_QTY_MINUS:
          decreaseDiceQuantity();
          break;
      }
    }
  }

  // --- Mode button long-press state machine ---
  if (modePending) {
    bool modeStillHeld = !((portA >> MCP_BTN_MODE) & 1);

    if (!modeStillHeld) {
      // Button released
      if (!modeLongDone) {
        changeDiceType();   // Short press
      }
      modePending  = false;
      modeLongDone = false;
    } else if (!modeLongDone && (now - modePressTime >= LONG_PRESS_DELAY)) {
      // Held past threshold while still down — long press
      toggleRollMode();
      modeLongDone = true;  // Prevent re-firing while still held
    }
  }

  lastPortA = portA;
}
