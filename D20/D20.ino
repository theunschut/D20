/*
 * Digital Multi-Dice for Arduino Nano ESP32 with GC9A01 Round Display
 *
 * Hardware:
 * - Arduino Nano ESP32
 * - 1.28" Round TFT LCD (240x240, GC9A01 driver)
 * - 2 Buttons (Roll + Change Dice Type)
 *
 * Wiring (based on confirmed working configuration):
 * - Display: VCC→3.3V, GND→GND, SCL→D13, SDA→D11, RES→D7, DC→D8, CS→D9, BLK→3.3V
 * - Roll Button: D2 and GND
 * - Mode Button: D3 and GND
 *
 * Required Libraries:
 * - Adafruit GFX Library
 * - Adafruit GC9A01A
 */

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>
#include "DiceTypes.h"
#include "Display.h"

// Pin definitions for Arduino Nano ESP32 (confirmed working from forum)
#define TFT_CS    9   // D9
#define TFT_DC    8   // D8
#define TFT_RST   7   // D7
#define TFT_MOSI  11  // D11 - SDA (hardware SPI MOSI)
#define TFT_SCLK  13  // D13 - SCL (hardware SPI SCK)

#define ROLL_BUTTON_PIN  2   // D2 - Roll dice (will be tilt sensor later)
#define MODE_BUTTON_PIN  3   // D3 - Change dice type (long press for advantage/disadvantage)
#define QTY_PLUS_PIN     4   // D4 - Increase dice quantity
#define QTY_MINUS_PIN    5   // D5 - Decrease dice quantity

// Display object - using HARDWARE SPI for maximum speed
// Constructor with only CS, DC, RST enables hardware SPI
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

// Game state
DiceType currentDiceType = DICE_D20;  // Start with D20
int diceQuantity = 1;                 // Number of dice to roll (1-4)
RollMode rollMode = NORMAL;

// Debouncing variables
unsigned long lastRollTime = 0;
unsigned long lastModeTime = 0;
unsigned long lastQtyPlusTime = 0;
unsigned long lastQtyMinusTime = 0;
const unsigned long debounceDelay = 300;
const unsigned long longPressDelay = 1000;  // 1 second for long press

bool lastRollState = HIGH;
bool lastModeState = HIGH;
bool lastQtyPlusState = HIGH;
bool lastQtyMinusState = HIGH;

unsigned long modeButtonPressStart = 0;
bool modeButtonLongPressed = false;

// Roll history
int currentNumber = 0;
int previousNumber = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Digital Multi-Dice ===");
  Serial.println("Initializing...");

  // Initialize buttons
  pinMode(ROLL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(QTY_PLUS_PIN, INPUT_PULLUP);
  pinMode(QTY_MINUS_PIN, INPUT_PULLUP);
  Serial.println("Buttons OK");

  // Initialize display
  initDisplay();

  // Welcome screen
  drawWelcomeScreen(currentDiceType);

  randomSeed(analogRead(A0));

  Serial.println("=== READY ===");
  Serial.println("Roll button: Roll dice");
  Serial.println("Mode button: Change dice type (long press for Adv/Dis)");
  Serial.println("Qty +/-: Change number of dice");
}

void loop() {
  bool rollState = digitalRead(ROLL_BUTTON_PIN);
  bool modeState = digitalRead(MODE_BUTTON_PIN);
  bool qtyPlusState = digitalRead(QTY_PLUS_PIN);
  bool qtyMinusState = digitalRead(QTY_MINUS_PIN);
  unsigned long currentTime = millis();

  // Handle roll button
  if (rollState == LOW && lastRollState == HIGH) {
    if (currentTime - lastRollTime > debounceDelay) {
      rollDice();
      lastRollTime = currentTime;
    }
  }

  // Handle mode button - long press detection
  if (modeState == LOW && lastModeState == HIGH) {
    // Button just pressed
    modeButtonPressStart = currentTime;
    modeButtonLongPressed = false;
  }

  if (modeState == LOW && !modeButtonLongPressed) {
    // Button held down - check for long press
    if (currentTime - modeButtonPressStart >= longPressDelay) {
      toggleRollMode();
      modeButtonLongPressed = true;
    }
  }

  if (modeState == HIGH && lastModeState == LOW) {
    // Button released - check if it was a short press
    if (!modeButtonLongPressed && (currentTime - modeButtonPressStart < longPressDelay)) {
      if (currentTime - lastModeTime > debounceDelay) {
        changeDiceType();
        lastModeTime = currentTime;
      }
    }
  }

  // Handle quantity + button
  if (qtyPlusState == LOW && lastQtyPlusState == HIGH) {
    if (currentTime - lastQtyPlusTime > debounceDelay) {
      increaseDiceQuantity();
      lastQtyPlusTime = currentTime;
    }
  }

  // Handle quantity - button
  if (qtyMinusState == LOW && lastQtyMinusState == HIGH) {
    if (currentTime - lastQtyMinusTime > debounceDelay) {
      decreaseDiceQuantity();
      lastQtyMinusTime = currentTime;
    }
  }

  lastRollState = rollState;
  lastModeState = modeState;
  lastQtyPlusState = qtyPlusState;
  lastQtyMinusState = qtyMinusState;
  delay(10);
}

void changeDiceType() {
  // Cycle to next dice type
  currentDiceType = (DiceType)((currentDiceType + 1) % DICE_TYPE_COUNT);

  String diceName = getDiceName(currentDiceType);
  Serial.print("Changed to: ");
  Serial.println(diceName);

  // Reset quantity when changing dice type (keep currentNumber visible)
  previousNumber = 0;
  diceQuantity = 1;

  // Show new dice type
  drawWelcomeScreen(currentDiceType, diceQuantity, rollMode, currentNumber);
}

void toggleRollMode() {
  // Cycle through NORMAL -> ADVANTAGE -> DISADVANTAGE -> NORMAL
  if (rollMode == NORMAL) {
    rollMode = ADVANTAGE;
    Serial.println("Mode: ADVANTAGE");
  } else if (rollMode == ADVANTAGE) {
    rollMode = DISADVANTAGE;
    Serial.println("Mode: DISADVANTAGE");
  } else {
    rollMode = NORMAL;
    Serial.println("Mode: NORMAL");
  }

  // Reset quantity when changing roll mode
  diceQuantity = 1;

  // Update display
  drawWelcomeScreen(currentDiceType, diceQuantity, rollMode, currentNumber);
}

void increaseDiceQuantity() {
  if (diceQuantity < 4) {
    diceQuantity++;
    Serial.print("Quantity: ");
    Serial.println(diceQuantity);
    drawWelcomeScreen(currentDiceType, diceQuantity, rollMode, currentNumber);
  }
}

void decreaseDiceQuantity() {
  if (diceQuantity > 1) {
    diceQuantity--;
    Serial.print("Quantity: ");
    Serial.println(diceQuantity);
    drawWelcomeScreen(currentDiceType, diceQuantity, rollMode, currentNumber);
  }
}

void rollDice() {
  int maxValue = getDiceMax(currentDiceType);
  String diceName = getDiceName(currentDiceType);

  Serial.print("Rolling ");
  Serial.print(diceQuantity);
  Serial.print(diceName);
  Serial.print(" (");
  Serial.print(rollMode == ADVANTAGE ? "ADV" : rollMode == DISADVANTAGE ? "DIS" : "NORMAL");
  Serial.println(")...");

  previousNumber = currentNumber;
  int total = 0;
  int rolls[4] = {0};  // Store up to 4 rolls

  if (rollMode == ADVANTAGE || rollMode == DISADVANTAGE) {
    // Roll twice, keep higher (advantage) or lower (disadvantage)
    int roll1 = random(1, maxValue + 1);
    int roll2 = random(1, maxValue + 1);

    if (rollMode == ADVANTAGE) {
      currentNumber = max(roll1, roll2);
    } else {
      currentNumber = min(roll1, roll2);
    }

    rolls[0] = roll1;
    rolls[1] = roll2;

    Serial.print("Rolls: ");
    Serial.print(roll1);
    Serial.print(" / ");
    Serial.print(roll2);
    Serial.print(" → ");
    Serial.println(currentNumber);

    // Animated rolling effect
    animatedRoll(currentNumber, maxValue, currentDiceType);

    // Display result with both rolls
    displayAdvDisResult(currentNumber, roll1, roll2, rollMode, currentDiceType, previousNumber);

  } else {
    // Normal roll - multiple dice
    for (int i = 0; i < diceQuantity; i++) {
      rolls[i] = random(1, maxValue + 1);
      total += rolls[i];
    }

    currentNumber = total;

    Serial.print("Rolls: ");
    for (int i = 0; i < diceQuantity; i++) {
      Serial.print(rolls[i]);
      if (i < diceQuantity - 1) Serial.print(" + ");
    }
    Serial.print(" = ");
    Serial.println(total);

    // Animated rolling effect
    animatedRoll(currentNumber, maxValue, currentDiceType);

    // Display final result
    displayMultiDiceResult(currentNumber, rolls, diceQuantity, maxValue, currentDiceType, previousNumber);
  }
}
