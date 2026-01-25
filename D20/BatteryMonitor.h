/*
 * BatteryMonitor.h - Battery voltage monitoring and percentage calculation
 *
 * Hardware Requirements:
 * - Voltage divider: BAT+ → 200kΩ → A0 → 200kΩ → GND
 * - This divides battery voltage by 2 (safe for ESP32 ADC max 3.3V)
 *
 * LiPo Voltage Levels:
 * - 4.2V = 100% (fully charged)
 * - 3.7V = 50%  (nominal)
 * - 3.0V = 0%   (empty - don't discharge below this!)
 */

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>

// Initialize battery monitoring
void initBatteryMonitor();

// Update battery reading (call periodically, e.g., every 5 seconds)
void updateBatteryReading();

// Get current battery voltage in volts (e.g., 3.85)
float getBatteryVoltage();

// Get battery percentage 0-100%
int getBatteryPercentage();

// Check if battery is low (below 20%)
bool isBatteryLow();

// Check if running on battery (vs USB power)
bool isOnBattery();

#endif
