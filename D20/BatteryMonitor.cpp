/*
 * BatteryMonitor.cpp - MAX17048 fuel gauge via I2C
 */

#include "BatteryMonitor.h"
#include "Config.h"
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>

static SFE_MAX1704X fuelGauge;
static bool gaugeInitialized = false;

static float         currentVoltage    = 0.0f;
static int           currentPercentage = 100;
static float         currentRate       = 0.0f;  // %/hour (positive = charging)
static unsigned long lastUpdateTime    = 0;

void initBatteryMonitor() {
  if (!fuelGauge.begin(Wire)) {
    Serial.println("ERROR: MAX17048 not found on I2C!");
    return;
  }

  gaugeInitialized = true;
  Serial.println("MAX17048 battery monitor initialized");

  // Take an immediate first reading
  lastUpdateTime = 0;
  updateBatteryReading();
}

void updateBatteryReading() {
  if (!gaugeInitialized) return;

  unsigned long now = millis();
  if (now - lastUpdateTime < BATTERY_UPDATE_INTERVAL) return;
  lastUpdateTime = now;

  currentVoltage    = fuelGauge.getVoltage();
  currentPercentage = (int)fuelGauge.getSOC();
  currentRate       = fuelGauge.getChangeRate();

  // Clamp percentage
  if (currentPercentage > 100) currentPercentage = 100;
  if (currentPercentage < 0)   currentPercentage = 0;

  Serial.print("Battery: ");
  Serial.print(currentVoltage, 2);
  Serial.print("V (");
  Serial.print(currentPercentage);
  Serial.print("%) rate=");
  Serial.print(currentRate, 1);
  Serial.println("%/hr");

  if (isBatteryLow()) {
    Serial.println("WARNING: LOW BATTERY!");
  }
}

float getBatteryVoltage()   { return currentVoltage; }
int   getBatteryPercentage() { return currentPercentage; }

bool isBatteryLow() {
  return gaugeInitialized && (currentPercentage <= BATTERY_LOW_THRESHOLD);
}

bool isOnBattery() {
  return gaugeInitialized;
}

bool isCharging() {
  return gaugeInitialized && (currentRate > 0.0f);
}
