/*
 * Display.h - Display functions for GC9A01
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include "DiceTypes.h"
#include "DiceBitmaps.h"

// Thin subclass exposing setAddrWindow publicly so SDAnimations can stream
// pixel data via writePixels() without the per-pixel overhead of drawRGBBitmap.
class D20Display : public Adafruit_GC9A01A {
public:
  using Adafruit_GC9A01A::Adafruit_GC9A01A;

  void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    setAddrWindow(x0, y0, x1, y1);
  }
};

extern D20Display tft;

// Function declarations
void initDisplay();
void drawWelcomeScreen(DiceType diceType, int quantity = 1, RollMode mode = NORMAL, int lastRoll = 0);
void displayMultiDiceResult(int total, int rolls[], int quantity, int maxValue, DiceType diceType, int previousNumber);
void displayAdvDisResult(int result, int roll1, int roll2, RollMode mode, DiceType diceType, int previousNumber);
void animatedRoll(int finalNumber, int maxValue, DiceType diceType);
void drawBatteryIndicator();

// Backlight control
void setBacklightBrightness(uint8_t level);
void resetActivityTimer();   // Call on any user activity to reset dim timer
void updateBacklight();      // Call every loop — handles auto-dim transitions

#endif
