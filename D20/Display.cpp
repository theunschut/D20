/*
 * Display.cpp - Display functions for GC9A01
 */

#include "Display.h"
#include "Config.h"
#include "BatteryMonitor.h"

void initDisplay() {
  Serial.println("Starting display...");

  // Initialize with hardware SPI
  tft.begin(SPI_SPEED);

  // Delay to ensure display is fully initialized
  delay(100);

  // Clear screen
  tft.fillScreen(COLOR_BG);

  Serial.println("Display initialized with HARDWARE SPI @ 40MHz");
  Serial.println("Display init SUCCESS");
}

void drawWelcomeScreen(DiceType diceType, int quantity, RollMode mode, int lastRoll) {
  // Don't clear screen if coming from boot (already cleared in initDisplay)
  // Only clear when changing dice type
  static bool firstBoot = true;
  if (!firstBoot) {
    tft.fillScreen(COLOR_BG);
  }
  firstBoot = false;

  // Get dice info
  String diceName = getDiceName(diceType);
  uint16_t diceColor = getDiceColor(diceType);

  // Calculate text bounds for both parts to center them together
  int16_t x1, y1;
  uint16_t w, h, qtyW, qtyH, diceW, diceH;

  String qtyStr = String(quantity);
  tft.setTextSize(4);  // Smaller size for quantity
  tft.getTextBounds(qtyStr.c_str(), 0, 0, &x1, &y1, &qtyW, &qtyH);

  tft.setTextSize(5);  // Larger size for dice name
  tft.getTextBounds(diceName.c_str(), 0, 0, &x1, &y1, &diceW, &diceH);

  // Total width and use larger height
  uint16_t totalW = qtyW + diceW;
  uint16_t maxH = max(qtyH, diceH);

  int startX = (240 - totalW) / 2;
  int startY = (240 - maxH) / 2 - 30;

  tft.setTextColor(diceColor);

  // Draw quantity (smaller)
  tft.setTextSize(4);
  tft.setCursor(startX, startY + (diceH - qtyH));  // Align baseline with dice name
  tft.print(qtyStr);

  // Draw dice name (larger)
  tft.setTextSize(5);
  tft.setCursor(startX + qtyW, startY);
  tft.print(diceName);

  // Draw mode indicator (ADV/DIS)
  if (mode != NORMAL) {
    tft.setTextSize(2);
    tft.setTextColor(mode == ADVANTAGE ? COLOR_ADVANTAGE : COLOR_DISADVANTAGE);
    String modeText = mode == ADVANTAGE ? "ADV" : "DIS";
    tft.getTextBounds(modeText.c_str(), 0, 0, &x1, &y1, &w, &h);
    tft.setCursor((240 - w) / 2, (240 - h) / 2);
    tft.println(modeText);
  }

  // Draw last roll if it exists
  if (lastRoll > 0) {
    tft.setTextColor(COLOR_TEXT);
    String lastText = String(lastRoll);
    tft.setTextSize(3);
    tft.getTextBounds(lastText.c_str(), 0, 0, &x1, &y1, &w, &h);
    tft.setCursor((240 - w) / 2, (240 - h) / 2 + 50);
    tft.println(lastText);
  }

  // Draw circle border
  tft.drawCircle(120, 120, 115, diceColor);
  tft.drawCircle(120, 120, 113, diceColor);

  // Draw battery indicator
  drawBatteryIndicator();

  Serial.print("Welcome screen drawn - ");
  Serial.print(quantity);
  Serial.println(diceName);
}

void animatedRoll(int finalNumber, int maxValue, DiceType diceType) {
  // Clear screen once at start
  tft.fillScreen(COLOR_BG);

  // Show random numbers for animation effect - FAST!
  for (int i = 0; i < 15; i++) {
    int fakeRoll = random(1, maxValue + 1);

    // Clear just the center area where numbers appear (faster than full screen)
    tft.fillRect(40, 80, 160, 80, COLOR_BG);

    // Display the fake number
    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(10);

    String numStr = String(fakeRoll);
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(numStr.c_str(), 0, 0, &x1, &y1, &w, &h);

    int x = (240 - w) / 2;
    int y = (240 - h) / 2;

    tft.setCursor(x, y);
    tft.print(numStr);

    delay(40);
  }

  // Clear the number area one more time before showing final result
  tft.fillRect(40, 80, 160, 80, COLOR_BG);
}

void displayMultiDiceResult(int total, int rolls[], int quantity, int maxValue, DiceType diceType, int previousNumber) {
  tft.fillScreen(COLOR_BG);

  // Display the total (always white for multi-dice)
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(10);

  String numStr = String(total);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(numStr.c_str(), 0, 0, &x1, &y1, &w, &h);

  int x = (240 - w) / 2;
  int y = (240 - h) / 2;

  tft.setCursor(x, y);
  tft.print(numStr);

  // Draw circle border
  uint16_t diceColor = getDiceColor(diceType);
  tft.drawCircle(120, 120, 115, diceColor);
  tft.drawCircle(120, 120, 113, diceColor);

  // Draw dice type with quantity at top (quantity smaller)
  String qtyStr = String(quantity);
  String diceName = getDiceName(diceType);

  uint16_t qtyW, qtyH, diceW, diceH;
  tft.setTextSize(1);
  tft.getTextBounds(qtyStr.c_str(), 0, 0, &x1, &y1, &qtyW, &qtyH);

  tft.setTextSize(2);
  tft.getTextBounds(diceName.c_str(), 0, 0, &x1, &y1, &diceW, &diceH);

  uint16_t totalW = qtyW + diceW;
  int startX = (240 - totalW) / 2;

  tft.setTextColor(diceColor);
  tft.setTextSize(1);
  tft.setCursor(startX, 20 + (diceH - qtyH));
  tft.print(qtyStr);

  tft.setTextSize(2);
  tft.setCursor(startX + qtyW, 20);
  tft.print(diceName);

  // Draw individual rolls breakdown at bottom with colors
  tft.setTextSize(1);
  int cursorX = 0;
  int cursorY = 210;

  // Calculate total width first to center the breakdown
  String fullBreakdown = "";
  for (int i = 0; i < quantity; i++) {
    fullBreakdown += String(rolls[i]);
    if (i < quantity - 1) fullBreakdown += "+";
  }
  tft.getTextBounds(fullBreakdown.c_str(), 0, 0, &x1, &y1, &w, &h);
  cursorX = (240 - w) / 2;

  // Draw each roll with appropriate color
  for (int i = 0; i < quantity; i++) {
    // Choose color based on individual roll
    uint16_t rollColor = COLOR_HISTORY;
    if (rolls[i] == maxValue && maxValue > 1) {
      rollColor = COLOR_CRIT;  // Yellow for max
    } else if (rolls[i] == 1 && maxValue > 1) {
      rollColor = COLOR_FAIL;  // Red for 1
    }

    tft.setTextColor(rollColor);
    tft.setCursor(cursorX, cursorY);
    String rollStr = String(rolls[i]);
    tft.print(rollStr);

    // Move cursor for next item
    tft.getTextBounds(rollStr.c_str(), 0, 0, &x1, &y1, &w, &h);
    cursorX += w;

    // Add separator if not last roll
    if (i < quantity - 1) {
      tft.setTextColor(COLOR_HISTORY);
      tft.setCursor(cursorX, cursorY);
      tft.print("+");
      tft.getTextBounds("+", 0, 0, &x1, &y1, &w, &h);
      cursorX += w;
    }
  }

  // Draw previous roll in corner (if exists)
  if (previousNumber > 0) {
    tft.setTextSize(1);
    tft.setTextColor(COLOR_HISTORY);
    tft.setCursor(10, 220);
    tft.print("Last: ");
    tft.print(previousNumber);
  }

  // Draw battery indicator
  drawBatteryIndicator();
}

void displayAdvDisResult(int result, int roll1, int roll2, RollMode mode, DiceType diceType, int previousNumber) {
  tft.fillScreen(COLOR_BG);

  int maxValue = getDiceMax(diceType);

  // Choose color based on result
  uint16_t textColor = COLOR_TEXT;
  if (result == maxValue && maxValue > 1) {
    textColor = COLOR_CRIT;  // Yellow for max
  } else if (result == 1 && maxValue > 1) {
    textColor = COLOR_FAIL;  // Red for 1
  }

  // Display the result number
  tft.setTextColor(textColor);
  tft.setTextSize(10);

  String numStr = String(result);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(numStr.c_str(), 0, 0, &x1, &y1, &w, &h);

  int x = (240 - w) / 2;
  int y = (240 - h) / 2;

  tft.setCursor(x, y);
  tft.print(numStr);

  // Draw circle border
  uint16_t diceColor = getDiceColor(diceType);
  tft.drawCircle(120, 120, 115, diceColor);
  tft.drawCircle(120, 120, 113, diceColor);

  // Draw mode and dice type at top
  tft.setTextSize(2);
  tft.setTextColor(mode == ADVANTAGE ? COLOR_ADVANTAGE : COLOR_DISADVANTAGE);
  String modeText = mode == ADVANTAGE ? "ADV " : "DIS ";
  modeText += getDiceName(diceType);
  tft.getTextBounds(modeText.c_str(), 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 20);
  tft.print(modeText);

  // Draw both rolls at bottom
  tft.setTextSize(1);
  tft.setTextColor(COLOR_HISTORY);
  String breakdown = String(roll1) + " / " + String(roll2);
  tft.getTextBounds(breakdown.c_str(), 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 210);
  tft.print(breakdown);

  // Draw previous roll in corner (if exists)
  if (previousNumber > 0) {
    tft.setTextSize(1);
    tft.setTextColor(COLOR_HISTORY);
    tft.setCursor(10, 220);
    tft.print("Last: ");
    tft.print(previousNumber);
  }

  // Draw battery indicator
  drawBatteryIndicator();
}

void drawBatteryIndicator() {
  // Battery icon position (top-right corner)
  int x = 200;
  int y = 10;
  int width = 28;
  int height = 14;
  int tipWidth = 3;

  // Get battery info
  int percentage = getBatteryPercentage();
  bool onBattery = isOnBattery();

  // Don't draw if on USB power
  if (!onBattery) {
    return;
  }

  // Choose color based on battery level
  uint16_t batteryColor;
  if (percentage <= 20) {
    batteryColor = 0xF800;  // Red - low battery
  } else if (percentage <= 50) {
    batteryColor = 0xFFE0;  // Yellow - medium
  } else {
    batteryColor = 0x07E0;  // Green - good
  }

  // Draw battery outline
  tft.drawRect(x, y, width, height, batteryColor);

  // Draw battery tip
  tft.fillRect(x + width, y + 4, tipWidth, height - 8, batteryColor);

  // Draw fill level (inside battery)
  int fillWidth = (width - 4) * percentage / 100;
  if (fillWidth > 0) {
    tft.fillRect(x + 2, y + 2, fillWidth, height - 4, batteryColor);
  }

  // Draw percentage text below battery
  tft.setTextSize(1);
  tft.setTextColor(batteryColor);
  tft.setCursor(x + 2, y + height + 2);
  tft.print(percentage);
  tft.print("%");

  // Low battery warning indicator (flashing)
  if (isBatteryLow() && (millis() / 500) % 2 == 0) {
    tft.setCursor(x - 5, y);
    tft.print("!");
  }
}

