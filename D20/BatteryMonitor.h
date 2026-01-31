/*
 * BatteryMonitor.h - Battery monitoring via MAX17048 fuel gauge (I2C)
 *
 * The MAX17048 provides accurate state-of-charge without an ADC or
 * voltage divider.  It shares the I2C bus (D4/D5) with MCP23017 and MPU6050.
 * Fixed address: 0x36.
 *
 * Required library: SparkFun_MAX1704x_Fuel_Gauge
 */

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>

// Initialize MAX17048 — call after Wire.begin()
void initBatteryMonitor();

// Poll the fuel gauge (throttled internally to BATTERY_UPDATE_INTERVAL)
void updateBatteryReading();

// Cell voltage in volts (3.0–4.2 for LiPo)
float getBatteryVoltage();

// State of charge 0–100%
int getBatteryPercentage();

// True when SOC is at or below BATTERY_LOW_THRESHOLD
bool isBatteryLow();

// True when MAX17048 initialized successfully (battery is present)
bool isOnBattery();

// True when the fuel gauge reports a positive charge rate (%/hour)
bool isCharging();

#endif
