/*
 * MotionDetector.cpp - Shake detection via raw I2C accelerometer reads
 *
 * Works with MPU6050 (WHO_AM_I 0x68) and MPU-6886 clones (WHO_AM_I 0x70).
 * Only the accelerometer is used; gyro and temp are ignored.
 *
 * Register map (same on both chips):
 *   0x6B        PWR_MGMT_1  – bit 7 = reset
 *   0x1A        CONFIG      – bits 2:0 = DLPF bandwidth
 *   0x1C        ACCEL_CONFIG – bits 4:3 = range (01 = ±4 g)
 *   0x3B–0x40   ACCEL_XYZH/L – 6 bytes, big-endian signed 16-bit
 *
 * ±4 g range → 8192 LSB/g → m/s² = raw / 8192.0 * 9.81
 */

#include "MotionDetector.h"
#include "Config.h"
#include "GameState.h"
#include "Display.h"
#include <Wire.h>
#include <math.h>

static const uint8_t MPU_ADDR         = 0x68;
static const uint8_t REG_PWR_MGMT_1   = 0x6B;
static const uint8_t REG_CONFIG       = 0x1A;
static const uint8_t REG_ACCEL_CONFIG = 0x1C;
static const uint8_t REG_ACCEL_XOUT_H = 0x3B;
static const float   ACCEL_SCALE      = 8192.0f;  // LSB/g for ±4 g

static bool mpuInitialized = false;

enum MotionState {
  MOTION_WAITING_REST,  // Initial: wait for device to settle before first roll
  MOTION_READY,         // Idle, watching for shake
  MOTION_SHAKING,       // Shake in progress, checking duration
  MOTION_COOLDOWN       // Post-roll cooldown
};

static MotionState   state           = MOTION_WAITING_REST;
static unsigned long shakeStartTime  = 0;
static unsigned long cooldownEnd     = 0;
static unsigned long restStartTime   = 0;
static unsigned long gracePeriodEnd  = 0;  // grace window when magnitude dips during shake

// --- Raw I2C helpers --------------------------------------------------------

static bool writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

static bool readRegs(uint8_t startReg, uint8_t count, uint8_t *buf) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(startReg);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom(MPU_ADDR, count);
  for (uint8_t i = 0; i < count; i++) {
    buf[i] = Wire.available() ? Wire.read() : 0;
  }
  return true;
}

// ----------------------------------------------------------------------------

bool initMotionDetector() {
  // Quick presence check
  Wire.beginTransmission(MPU_ADDR);
  if (Wire.endTransmission() != 0) {
    Serial.println("ERROR: MPU not found on I2C!");
    return false;
  }

  // Reset
  writeReg(REG_PWR_MGMT_1, 0x80);
  delay(100);
  writeReg(REG_PWR_MGMT_1, 0x00);  // wake up, use internal 8 MHz osc
  delay(10);

  // DLPF = 20 Hz cutoff (DLPF_CFG = 4)
  writeReg(REG_CONFIG, 0x04);

  // Accelerometer ±4 g (bits 4:3 = 01)
  writeReg(REG_ACCEL_CONFIG, 0x08);

  mpuInitialized = true;
  state          = MOTION_WAITING_REST;
  restStartTime  = 0;

  Serial.println("MPU accelerometer initialized (raw I2C)");
  return true;
}

void updateMotionDetector() {
  if (!mpuInitialized) return;

  uint8_t raw[6];
  if (!readRegs(REG_ACCEL_XOUT_H, 6, raw)) return;

  // Big-endian signed 16-bit → float m/s²
  float ax = (int16_t)((raw[0] << 8) | raw[1]) / ACCEL_SCALE * 9.81f;
  float ay = (int16_t)((raw[2] << 8) | raw[3]) / ACCEL_SCALE * 9.81f;
  float az = (int16_t)((raw[4] << 8) | raw[5]) / ACCEL_SCALE * 9.81f;
  float magnitude = sqrt(ax * ax + ay * ay + az * az);

  unsigned long now = millis();

  switch (state) {
    case MOTION_READY:
      if (magnitude > SHAKE_THRESHOLD) {
        shakeStartTime = now;
        gracePeriodEnd = 0;
        state = MOTION_SHAKING;
      }
      break;

    case MOTION_SHAKING:
      if (magnitude >= SHAKE_THRESHOLD) {
        gracePeriodEnd = 0;  // Back above threshold — cancel any grace period
      } else if (gracePeriodEnd == 0) {
        gracePeriodEnd = now + 150;  // Start 150 ms grace window
      } else if (now >= gracePeriodEnd) {
        // Grace expired without recovering — shake abandoned
        state = MOTION_READY;
        break;
      }
      if (now - shakeStartTime >= SHAKE_DURATION) {
        // Sustained shake — trigger roll
        Serial.println("Shake detected - rolling!");
        resetActivityTimer();
        rollDice();
        cooldownEnd = now + DEBOUNCE_DELAY;
        state = MOTION_COOLDOWN;
      }
      break;

    case MOTION_COOLDOWN:
      if (now >= cooldownEnd) {
        restStartTime = 0;
        state = MOTION_WAITING_REST;
        Serial.println("Cooldown done - waiting for rest...");
      }
      break;

    case MOTION_WAITING_REST:
      if (magnitude < REST_THRESHOLD) {
        if (restStartTime == 0) {
          restStartTime = now;
        } else if (now - restStartTime >= REST_TIME) {
          state = MOTION_READY;
          Serial.println("At rest - shake ready");
        }
      } else {
        restStartTime = 0;  // Movement detected — reset rest timer
      }
      break;
  }
}

bool isMotionReady() {
  return mpuInitialized && (state == MOTION_READY);
}
