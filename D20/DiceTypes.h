/*
 * DiceTypes.h - Dice type definitions and utilities
 */

#ifndef DICE_TYPES_H
#define DICE_TYPES_H

#include <Arduino.h>

// Dice types
enum DiceType {
  DICE_D4 = 0,
  DICE_D6,
  DICE_D8,
  DICE_D10,
  DICE_D12,
  DICE_D20,
  DICE_TYPE_COUNT
};

// Roll modes for advantage/disadvantage
enum RollMode {
  NORMAL,
  ADVANTAGE,
  DISADVANTAGE
};

// Color definitions for each dice type
#define COLOR_D4      0x07FF  // Cyan
#define COLOR_D6      0xFFE0  // Yellow
#define COLOR_D8      0xF81F  // Magenta
#define COLOR_D10     0xFD20  // Orange
#define COLOR_D12     0x07E0  // Green
#define COLOR_D20     0x07E0  // Green

// General colors
#define COLOR_BG           0x0000  // Black
#define COLOR_TEXT         0xFFFF  // White
#define COLOR_CRIT         0xFFE0  // Yellow (for max roll)
#define COLOR_FAIL         0xF800  // Red (for roll of 1)
#define COLOR_HISTORY      0x4208  // Gray (for history display)
#define COLOR_ADVANTAGE    0x07E0  // Green (for ADV mode)
#define COLOR_DISADVANTAGE 0xF800  // Red (for DIS mode)

// Function declarations
String getDiceName(DiceType type);
int getDiceMax(DiceType type);
uint16_t getDiceColor(DiceType type);

#endif
