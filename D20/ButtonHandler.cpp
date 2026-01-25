/*
 * ButtonHandler.cpp - Button input handling with debouncing and long-press detection
 * Mode, Qty+, and Qty- use interrupts for instant response
 * Roll button/tilt sensor uses main loop for complex timing logic
 */

#include "ButtonHandler.h"
#include "GameState.h"
#include "Config.h"
#include <Arduino.h>

// Roll button/tilt sensor state (handled in main loop)
static bool lastRollState = HIGH;
static unsigned long lastRollTime = 0;
static bool rollWaitingForRest = false;
static unsigned long rollRestStartTime = 0;
static bool rollSensorStable = false;

// Interrupt-driven button state (volatile for ISR access)
static volatile unsigned long lastModeTime = 0;
static volatile unsigned long lastQtyPlusTime = 0;
static volatile unsigned long lastQtyMinusTime = 0;
static volatile unsigned long modeButtonPressTime = 0;
static volatile bool modePendingAction = false;

// Constants for debouncing (defined here for ISR access)
static const unsigned long BUTTON_DEBOUNCE = 300;  // Debounce for interrupt buttons

// ============================================================================
// Interrupt Service Routines (ISRs) - must be fast and in IRAM
// ============================================================================

void IRAM_ATTR modeButtonISR() {
  unsigned long now = millis();
  if (now - lastModeTime > BUTTON_DEBOUNCE) {
    modeButtonPressTime = now;
    modePendingAction = true;
    lastModeTime = now;
  }
}

void IRAM_ATTR qtyPlusButtonISR() {
  unsigned long now = millis();
  if (now - lastQtyPlusTime > BUTTON_DEBOUNCE) {
    increaseDiceQuantity();
    lastQtyPlusTime = now;
  }
}

void IRAM_ATTR qtyMinusButtonISR() {
  unsigned long now = millis();
  if (now - lastQtyMinusTime > BUTTON_DEBOUNCE) {
    decreaseDiceQuantity();
    lastQtyMinusTime = now;
  }
}

// ============================================================================
// Initialization
// ============================================================================

void initButtons() {
  // Setup roll button/tilt sensor (handled in main loop)
  pinMode(ROLL_BUTTON_PIN, INPUT_PULLUP);

  // Setup interrupt-driven buttons
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(QTY_PLUS_PIN, INPUT_PULLUP);
  pinMode(QTY_MINUS_PIN, INPUT_PULLUP);

  // Attach interrupts (FALLING = button pressed, going from HIGH to LOW)
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON_PIN), modeButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(QTY_PLUS_PIN), qtyPlusButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(QTY_MINUS_PIN), qtyMinusButtonISR, FALLING);

  // Read initial state for roll button/tilt sensor
  delay(100);  // Let pins stabilize
  lastRollState = digitalRead(ROLL_BUTTON_PIN);

  // If roll sensor is already tilted at power-on, set waiting flag
  if (lastRollState == LOW) {
    rollWaitingForRest = true;
    rollRestStartTime = 0;
    rollSensorStable = false;
    Serial.println("⚠️ Tilt sensor is tilted at power-on - waiting for stable rest position");
  } else {
    // Sensor is at rest at power-on - immediately ready
    rollWaitingForRest = false;
    rollSensorStable = true;
    Serial.println("✓ Tilt sensor at rest - ready");
  }

  Serial.println("Buttons initialized (Mode/Qty on interrupts)");
  Serial.print("Roll/Tilt sensor: ");
  Serial.println(lastRollState == HIGH ? "UP" : "DOWN");
}

// ============================================================================
// Main Update Loop
// ============================================================================

void updateButtons() {
  unsigned long currentTime = millis();
  bool rollState = digitalRead(ROLL_BUTTON_PIN);

  // ========== Roll Button / Tilt Sensor (with stable rest requirement) ==========
  // The tilt sensor must be STABLE at rest (HIGH) for TILT_REST_TIME before next roll
  // This prevents brief bounces from clearing the "waiting for rest" flag

  if (rollWaitingForRest) {
    // Currently in cooldown - waiting for stable rest
    if (rollState == HIGH) {
      // Sensor is at rest - check if it's been stable
      if (rollRestStartTime == 0) {
        // Just went to rest - start timer
        rollRestStartTime = currentTime;
        rollSensorStable = false;
      } else if (!rollSensorStable && (currentTime - rollRestStartTime >= TILT_REST_TIME)) {
        // Been stable at rest for required time - ready for next roll
        rollSensorStable = true;
        rollWaitingForRest = false;
        Serial.println("✓ Tilt sensor ready");
      }
    } else {
      // Sensor tilted again before stable - reset timer
      rollRestStartTime = 0;
      rollSensorStable = false;
    }
  }

  if (rollState == LOW && lastRollState == HIGH && !rollWaitingForRest) {
    // Tilt detected AND sensor is ready - check debounce and trigger
    if (currentTime - lastRollTime > DEBOUNCE_DELAY) {
      Serial.println("🎲 Shake detected - rolling!");
      rollDice();
      lastRollTime = currentTime;
      rollWaitingForRest = true;  // Enter cooldown - must return to stable rest
      rollRestStartTime = 0;
      rollSensorStable = false;
    }
  }

  lastRollState = rollState;

  // ========== Mode Button (interrupt-driven with long-press detection) ==========
  // ISR sets modePendingAction flag, we handle long-press here
  if (modePendingAction) {
    bool modeState = digitalRead(MODE_BUTTON_PIN);

    // Wait to see if it's held (long press) or released (short press)
    while (modeState == LOW && (millis() - modeButtonPressTime < LONG_PRESS_DELAY)) {
      modeState = digitalRead(MODE_BUTTON_PIN);
      delay(10);
    }

    if (millis() - modeButtonPressTime >= LONG_PRESS_DELAY) {
      // Long press detected
      toggleRollMode();
    } else {
      // Short press (button was released)
      changeDiceType();
    }

    modePendingAction = false;
  }

  // Qty+/- buttons are fully handled by interrupts, no polling needed!
}
