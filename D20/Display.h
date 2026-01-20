/*
 * Display.h - Display functions for GC9A01
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "DiceTypes.h"

// Display object (extern - defined in main file)
extern Adafruit_GC9A01A tft;

// Roll mode enum (needs to match main file)
enum RollMode { NORMAL, ADVANTAGE, DISADVANTAGE };

// Function declarations
void initDisplay();
void drawWelcomeScreen(DiceType diceType, int quantity = 1, RollMode mode = NORMAL, int lastRoll = 0);
void displayNumber(int number, int maxValue, DiceType diceType, int previousNumber);
void displayMultiDiceResult(int total, int rolls[], int quantity, int maxValue, DiceType diceType, int previousNumber);
void displayAdvDisResult(int result, int roll1, int roll2, RollMode mode, DiceType diceType, int previousNumber);
void animatedRoll(int finalNumber, int maxValue, DiceType diceType);

#endif
