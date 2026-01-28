/*
 * DiceTypes.cpp - Dice type utility functions
 */

#include "DiceTypes.h"

String getDiceName(DiceType type) {
  switch(type) {
    case DICE_D4:   return "D4";
    case DICE_D6:   return "D6";
    case DICE_D8:   return "D8";
    case DICE_D10:  return "D10";
    case DICE_D12:  return "D12";
    case DICE_D20:  return "D20";
    default:        return "D20";
  }
}

int getDiceMax(DiceType type) {
  switch(type) {
    case DICE_D4:   return 4;
    case DICE_D6:   return 6;
    case DICE_D8:   return 8;
    case DICE_D10:  return 10;
    case DICE_D12:  return 12;
    case DICE_D20:  return 20;
    default:        return 20;
  }
}

uint16_t getDiceColor(DiceType type) {
  switch(type) {
    case DICE_D4:   return COLOR_D4;
    case DICE_D6:   return COLOR_D6;
    case DICE_D8:   return COLOR_D8;
    case DICE_D10:  return COLOR_D10;
    case DICE_D12:  return COLOR_D12;
    case DICE_D20:  return COLOR_D20;
    default:        return COLOR_D20;
  }
}
