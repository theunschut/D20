/*
 * GameState.h - Game state management and dice rolling logic
 */

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "DiceTypes.h"

// Game state variables (extern - defined in GameState.cpp)
extern DiceType currentDiceType;
extern int diceQuantity;
extern RollMode rollMode;
extern int currentNumber;
extern int previousNumber;

// Game state functions
void initGameState();
void changeDiceType();
void toggleRollMode();
void increaseDiceQuantity();
void decreaseDiceQuantity();
void rollDice();
void checkAutoReturn();  // Check if should return to welcome screen

#endif
