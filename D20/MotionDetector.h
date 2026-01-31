/*
 * MotionDetector.h - MPU6050 shake detection for dice rolling
 *
 * Replaces the tilt sensor with 6-axis IMU shake detection.
 * State machine: WAITING_REST → READY → SHAKING → COOLDOWN → WAITING_REST
 *
 * Tuning constants in Config.h:
 *   SHAKE_THRESHOLD  - acceleration magnitude (m/s²) to start a shake
 *   SHAKE_DURATION   - how long the shake must be sustained (ms)
 *   DEBOUNCE_DELAY   - cooldown after a roll before the next one (ms)
 *   REST_THRESHOLD   - magnitude below which the device is "at rest"
 *   REST_TIME        - how long at rest before ready for the next shake (ms)
 */

#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

// Initialize MPU6050 — returns false if device not found
bool initMotionDetector();

// Poll accelerometer and trigger rollDice() on shake — call every loop iteration
void updateMotionDetector();

// True when the detector is in READY state and waiting for a shake
bool isMotionReady();

#endif
