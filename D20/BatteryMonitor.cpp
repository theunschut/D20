/*
 * BatteryMonitor.cpp - Battery voltage monitoring and percentage calculation
 */

#include "BatteryMonitor.h"
#include "Config.h"

// Battery voltage thresholds (LiPo chemistry)
#define BATTERY_MAX_VOLTAGE   4.20f  // 100% - fully charged
#define BATTERY_NOM_VOLTAGE   3.70f  // ~50% - nominal voltage
#define BATTERY_MIN_VOLTAGE   3.00f  // 0% - empty (don't go below!)
#define BATTERY_LOW_THRESHOLD 3.30f  // 20% - low battery warning

// Voltage divider ratio (2x 200kΩ resistors = divide by 2)
#define VOLTAGE_DIVIDER_RATIO 2.0f

// Number of samples to average for stable reading
#define BATTERY_SAMPLES 10

// Update interval (milliseconds)
#define BATTERY_UPDATE_INTERVAL 5000  // 5 seconds

// State variables
static float currentVoltage = 0.0f;
static int currentPercentage = 0;
static unsigned long lastUpdateTime = 0;
static bool batteryPresent = false;
static bool prevBatteryPresent = false;  // Track state changes

void initBatteryMonitor() {
  pinMode(BATTERY_PIN, INPUT);
  Serial.println("Battery monitor initialized");

  // Take initial reading
  updateBatteryReading();
}

void updateBatteryReading() {
  unsigned long currentTime = millis();

  // Throttle updates to save power
  if (currentTime - lastUpdateTime < BATTERY_UPDATE_INTERVAL) {
    return;
  }

  lastUpdateTime = currentTime;

  // Average multiple readings for stability
  long sum = 0;
  for (int i = 0; i < BATTERY_SAMPLES; i++) {
    sum += analogReadMilliVolts(BATTERY_PIN);
    delay(10);
  }

  float avgMillivolts = sum / (float)BATTERY_SAMPLES;

  // Convert to actual battery voltage (compensate for voltage divider)
  currentVoltage = (avgMillivolts / 1000.0f) * VOLTAGE_DIVIDER_RATIO;

  // Detect if battery is connected
  // Use 3.5V threshold to avoid false positives from floating pin noise
  // Real LiPo battery will always be >= 3.0V (empty) to 4.2V (full)
  if (currentVoltage < 3.5f) {
    batteryPresent = false;
    currentPercentage = 100;  // Show full when on USB
    currentVoltage = 0.0f;
  } else {
    batteryPresent = true;

    // Convert voltage to percentage (linear approximation)
    // More accurate would use LiPo discharge curve, but linear is good enough
    if (currentVoltage >= BATTERY_MAX_VOLTAGE) {
      currentPercentage = 100;
    } else if (currentVoltage <= BATTERY_MIN_VOLTAGE) {
      currentPercentage = 0;
    } else {
      float range = BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE;
      float voltage_above_min = currentVoltage - BATTERY_MIN_VOLTAGE;
      currentPercentage = (int)((voltage_above_min / range) * 100.0f);

      // Clamp to 0-100
      if (currentPercentage < 0) currentPercentage = 0;
      if (currentPercentage > 100) currentPercentage = 100;
    }
  }

  // Debug output (print USB power once, battery continuously)
  if (batteryPresent) {
    // Always print battery status
    Serial.print("Battery: ");
    Serial.print(currentVoltage, 2);
    Serial.print("V (");
    Serial.print(currentPercentage);
    Serial.println("%)");

    if (isBatteryLow()) {
      Serial.println("⚠️ LOW BATTERY WARNING!");
    }
  } else {
    // Only print USB power when state changes
    if (prevBatteryPresent) {
      Serial.println("Battery: USB Power");
    }
  }

  prevBatteryPresent = batteryPresent;
}

float getBatteryVoltage() {
  return currentVoltage;
}

int getBatteryPercentage() {
  return currentPercentage;
}

bool isBatteryLow() {
  return batteryPresent && (currentVoltage < BATTERY_LOW_THRESHOLD);
}

bool isOnBattery() {
  return batteryPresent;
}
