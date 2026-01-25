/*
 * ButtonHandler.h - Button input handling with debouncing and long-press detection
 */

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

// Initialize button pins
void initButtons();

// Main button update function - call this in loop()
void updateButtons();

#endif
