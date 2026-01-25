/*
 * ButtonHandler.cpp - Button input handling with debouncing and long-press detection
 */

#include "ButtonHandler.h"
#include "GameState.h"
#include "Config.h"
#include <Arduino.h>

// Button state tracking
static bool lastRollState = HIGH;
static bool lastModeState = HIGH;
static bool lastQtyPlusState = HIGH;
static bool lastQtyMinusState = HIGH;

// Debouncing timing
static unsigned long lastRollTime = 0;
static unsigned long lastModeTime = 0;
static unsigned long lastQtyPlusTime = 0;
static unsigned long lastQtyMinusTime = 0;

// Long press detection
static unsigned long modeButtonPressStart = 0;
static bool modeButtonLongPressed = false;

void initButtons() {
  pinMode(ROLL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(QTY_PLUS_PIN, INPUT_PULLUP);
  pinMode(QTY_MINUS_PIN, INPUT_PULLUP);

  Serial.println("Buttons initialized");
}

void updateButtons() {
  // Read current button states
  bool rollState = digitalRead(ROLL_BUTTON_PIN);
  bool modeState = digitalRead(MODE_BUTTON_PIN);
  bool qtyPlusState = digitalRead(QTY_PLUS_PIN);
  bool qtyMinusState = digitalRead(QTY_MINUS_PIN);
  unsigned long currentTime = millis();

  // ========== Roll Button ==========
  if (rollState == LOW && lastRollState == HIGH) {
    if (currentTime - lastRollTime > DEBOUNCE_DELAY) {
      rollDice();
      lastRollTime = currentTime;
    }
  }

  // ========== Mode Button (with long-press detection) ==========
  if (modeState == LOW && lastModeState == HIGH) {
    // Button just pressed
    modeButtonPressStart = currentTime;
    modeButtonLongPressed = false;
  }

  if (modeState == LOW && !modeButtonLongPressed) {
    // Button held down - check for long press
    if (currentTime - modeButtonPressStart >= LONG_PRESS_DELAY) {
      toggleRollMode();
      modeButtonLongPressed = true;
    }
  }

  if (modeState == HIGH && lastModeState == LOW) {
    // Button released - check if it was a short press
    if (!modeButtonLongPressed && (currentTime - modeButtonPressStart < LONG_PRESS_DELAY)) {
      if (currentTime - lastModeTime > DEBOUNCE_DELAY) {
        changeDiceType();
        lastModeTime = currentTime;
      }
    }
  }

  // ========== Quantity Plus Button ==========
  if (qtyPlusState == LOW && lastQtyPlusState == HIGH) {
    if (currentTime - lastQtyPlusTime > DEBOUNCE_DELAY) {
      increaseDiceQuantity();
      lastQtyPlusTime = currentTime;
    }
  }

  // ========== Quantity Minus Button ==========
  if (qtyMinusState == LOW && lastQtyMinusState == HIGH) {
    if (currentTime - lastQtyMinusTime > DEBOUNCE_DELAY) {
      decreaseDiceQuantity();
      lastQtyMinusTime = currentTime;
    }
  }

  // Update last states
  lastRollState = rollState;
  lastModeState = modeState;
  lastQtyPlusState = qtyPlusState;
  lastQtyMinusState = qtyMinusState;
}
