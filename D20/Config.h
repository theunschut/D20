/*
 * Config.h - Hardware configuration and constants for XIAO ESP32-C3
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// HARDWARE CONFIGURATION - XIAO ESP32-C3
// ============================================================================

// Display pins (IMPORTANT: Use GPIO numbers, not D-numbers!)
#define TFT_CS    5   // D3 = GPIO5 - Chip Select
#define TFT_DC    4   // D2 = GPIO4 - Data/Command
#define TFT_RST   3   // D1 = GPIO3 - Reset
// Hardware SPI auto-uses: GPIO8 (D8/SCK), GPIO10 (D10/MOSI)

// Button pins (IMPORTANT: Use GPIO numbers, not D-numbers!)
#define ROLL_BUTTON_PIN   20  // D7 = GPIO20 - Roll dice (or SW-520D tilt sensor)
#define MODE_BUTTON_PIN   21  // D6 = GPIO21 - Change dice type (long press for Adv/Dis)
#define QTY_PLUS_PIN      6   // D4 = GPIO6  - Increase dice quantity
#define QTY_MINUS_PIN     7   // D5 = GPIO7  - Decrease dice quantity

// Note: ROLL_BUTTON_PIN can be replaced with SW-520D tilt sensor (shake to roll)
// Just connect SW-520D to D7+GND instead of button - works the same way!

// Battery monitoring (IMPORTANT: Use GPIO numbers, not D-numbers!)
#define BATTERY_PIN       2   // D0 = GPIO2 (A0) - Battery voltage via voltage divider

// Display configuration
#define SPI_SPEED         40000000  // Hardware SPI @ 40MHz

// ============================================================================
// GAME CONSTANTS
// ============================================================================

#define MAX_DICE_QUANTITY 4  // Maximum number of dice to roll at once

// ============================================================================
// TIMING CONSTANTS
// ============================================================================

#define DEBOUNCE_DELAY       1000  // Roll button/tilt sensor cooldown period (ms)
                                    // Time after a roll before next roll allowed
                                    // Adjust for tilt sensor sensitivity:
                                    // - 800ms = more sensitive
                                    // - 1000ms = normal (recommended)
                                    // - 1500ms = less sensitive
                                    // - 2000ms = very deliberate
#define TILT_REST_TIME       300   // Tilt sensor must be stable/flat for this long (ms)
                                    // before being ready for next roll
#define LONG_PRESS_DELAY     1000  // Long press threshold in milliseconds

// ============================================================================
// WIRING REFERENCE
// ============================================================================
/*
 * XIAO ESP32-C3 Wiring:
 *
 * Display (GC9A01 1.28" Round):
 *   VCC → 3V3
 *   GND → GND
 *   SCL → D8  (GPIO8 - Hardware SPI Clock)
 *   SDA → D10 (GPIO10 - Hardware SPI MOSI)
 *   RES → D1  (GPIO3)
 *   DC  → D2  (GPIO4)
 *   CS  → D3  (GPIO5)
 *   BLK → 3V3
 *
 * Buttons:
 *   Roll    → D7 + GND (GPIO20) - OR SW-520D tilt sensor for shake-to-roll!
 *   Mode    → D6 + GND (GPIO21)
 *   Qty +   → D4 + GND (GPIO6)
 *   Qty -   → D5 + GND (GPIO7)
 *
 * Tilt Sensor (optional - replaces roll button):
 *   SW-520D Pin 1 → D7 (GPIO20) - same as roll button!
 *   SW-520D Pin 2 → GND
 *   No polarity - pins are interchangeable
 *   Works with INPUT_PULLUP (same as buttons)
 *
 *   How it works:
 *   1. Shake device → sensor tilts → triggers roll
 *   2. Enter cooldown (DEBOUNCE_DELAY = 1000ms default)
 *   3. Sensor must be stable at rest for TILT_REST_TIME (300ms)
 *   4. Once stable → ready for next shake
 *
 *   This prevents:
 *   - Button press vibrations triggering rolls
 *   - Tiny bumps/movements causing rolls
 *   - Continuous rolling from unstable sensor
 *
 *   Sensitivity adjustments (in "TIMING CONSTANTS" section):
 *   - DEBOUNCE_DELAY: Cooldown after roll (800-2000ms)
 *   - TILT_REST_TIME: How long sensor must be flat (200-500ms)
 *   - Increase both for less sensitivity, decrease for more
 *
 * Battery Monitor (Voltage Divider):
 *   BAT+ → 200kΩ resistor → A0 (D0/GPIO2) → 200kΩ resistor → GND
 *   This divides battery voltage by 2 (4.2V → 2.1V safe for ADC)
 *
 * Battery Power (on back of XIAO):
 *   LiPo (+) → BAT+ pad
 *   LiPo (-) → BAT- pad (or GND pin)
 *   OR use TP4056 charger module:
 *     LiPo → TP4056 B+/B-
 *     TP4056 OUT+ → XIAO BAT+
 *     TP4056 OUT- → XIAO GND
 *
 * Notes:
 *   - D0 (GPIO2) used for battery monitoring (analog input)
 *   - D9 (GPIO9) is hardware SPI MISO - kept free
 */

#endif
