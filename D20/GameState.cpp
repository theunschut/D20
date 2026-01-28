/*
 * GameState.cpp - Game state management and dice rolling logic
 */

#include "GameState.h"
#include "Display.h"
#include "Config.h"
#include <Arduino.h>

// Game state variables
DiceType currentDiceType = DICE_D20;  // Start with D20
int diceQuantity = 1;                 // Number of dice to roll (1-4)
RollMode rollMode = NORMAL;
int currentNumber = 0;
int previousNumber = 0;

// Auto-return timer
static unsigned long lastRollTime = 0;
static bool resultDisplayed = false;
#define AUTO_RETURN_DELAY 5000  // 5 seconds

void initGameState() {
  currentDiceType = DICE_D20;
  diceQuantity = 1;
  rollMode = NORMAL;
  currentNumber = 0;
  previousNumber = 0;

  // Seed random number generator
  randomSeed(analogRead(A0));
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

  // Cancel auto-return (user is making changes)
  resultDisplayed = false;

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

  // Cancel auto-return (user is making changes)
  resultDisplayed = false;

  // Update display
  drawWelcomeScreen(currentDiceType, diceQuantity, rollMode, currentNumber);
}

void increaseDiceQuantity() {
  if (diceQuantity < MAX_DICE_QUANTITY) {
    diceQuantity++;
    Serial.print("Quantity: ");
    Serial.println(diceQuantity);

    // Cancel auto-return (user is making changes)
    resultDisplayed = false;

    drawWelcomeScreen(currentDiceType, diceQuantity, rollMode, currentNumber);
  }
}

void decreaseDiceQuantity() {
  if (diceQuantity > 1) {
    diceQuantity--;
    Serial.print("Quantity: ");
    Serial.println(diceQuantity);

    // Cancel auto-return (user is making changes)
    resultDisplayed = false;

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

  // Start auto-return timer
  lastRollTime = millis();
  resultDisplayed = true;
}

void checkAutoReturn() {
  // If result is displayed and 5 seconds have passed, return to welcome screen
  if (resultDisplayed && (millis() - lastRollTime >= AUTO_RETURN_DELAY)) {
    resultDisplayed = false;
    drawWelcomeScreen(currentDiceType, diceQuantity, rollMode, currentNumber);
    Serial.println("Auto-returned to welcome screen");
  }
}
