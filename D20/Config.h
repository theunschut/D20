/*
 * Config.h - Hardware configuration and constants for XIAO ESP32-C3
 *
 * Hardware:
 * - XIAO ESP32-C3
 * - GC9A01 1.28" Round TFT (SPI)
 * - MCP23017 I2C GPIO Expander (buttons)
 * - MPU6050 I2C Accelerometer/Gyroscope (shake-to-roll)
 * - MAX17048 I2C LiPo Fuel Gauge (battery monitoring)
 * - MicroSD Card (SPI, animations)
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// SPI PINS (Hardware SPI - fixed on XIAO ESP32-C3)
// ============================================================================
// D8  = GPIO8  → SPI SCK  (auto-used)
// D9  = GPIO9  → SPI MISO (auto-used)
// D10 = GPIO10 → SPI MOSI (auto-used)

// Display pins
#define TFT_CS    5   // D3 = GPIO5 - Chip Select
#define TFT_DC    4   // D2 = GPIO4 - Data/Command
#define TFT_RST   3   // D1 = GPIO3 - Reset
#define TFT_BLK   2   // D0 = GPIO2 - Backlight PWM (strapping pin, safe after boot)

// SD Card
#define SD_CS_PIN 21  // D6 = GPIO21 - SD Card Chip Select

// ============================================================================
// I2C PINS (shared bus - MCP23017, MPU6050, MAX17048)
// ============================================================================
#define I2C_SDA   6   // D4 = GPIO6
#define I2C_SCL   7   // D5 = GPIO7

// ============================================================================
// MCP23017 CONFIGURATION
// ============================================================================
#define MCP_INT_PIN       20  // D7 = GPIO20 - INTA interrupt output (future: sleep/wake)

// Button pin mapping on MCP23017 Port A (bit numbers 0-7)
#define MCP_BTN_SPARE     0   // GPA0 - 4th button (spare, function TBD)
#define MCP_BTN_MODE      1   // GPA1 - Mode button (short=dice type, long=adv/dis)
#define MCP_BTN_QTY_PLUS  2   // GPA2 - Qty+ button
#define MCP_BTN_QTY_MINUS 3   // GPA3 - Qty- button

// ============================================================================
// MPU6050 SHAKE DETECTION
// ============================================================================
#define SHAKE_THRESHOLD   18.0   // m/s² total magnitude to trigger shake
                                  // At rest ≈ 9.81 (gravity). Peaks 16–23 on shake
#define SHAKE_DURATION    200    // ms - shake must be sustained (dips OK, full rest resets)
#define DEBOUNCE_DELAY    1000   // ms - cooldown after roll before next roll allowed
#define REST_THRESHOLD    12.0   // m/s² - below this counts as "at rest"
#define REST_TIME         500    // ms - must be at rest for this long before ready again

// ============================================================================
// BACKLIGHT / AUTO-DIM
// ============================================================================
#define BACKLIGHT_FULL    255
#define BACKLIGHT_DIM     128
#define BACKLIGHT_SLEEP   25
#define DIM_TIMEOUT       10000  // ms to full→dim
#define SLEEP_TIMEOUT     30000  // ms to dim→very dim

// ============================================================================
// SPI / GAME / BATTERY CONSTANTS
// ============================================================================
#define SPI_SPEED              40000000  // Hardware SPI @ 40MHz
#define MAX_DICE_QUANTITY      4
#define LONG_PRESS_DELAY       1000      // ms - long press threshold for Mode button
#define AUTO_RETURN_DELAY      5000      // ms - auto-return to welcome screen
#define BATTERY_UPDATE_INTERVAL 5000     // ms - how often to poll MAX17048
#define BATTERY_LOW_THRESHOLD  20        // % - low battery warning level

// ============================================================================
// SD CARD ANIMATION CONFIGURATION
// ============================================================================
// Frame format: raw RGB565 binary, 240x240 pixels = 115,200 bytes per frame
// Directory: /roll/
// Naming: 001.bin, 002.bin, 003.bin, ...
#define ANIM_FRAME_WIDTH  240
#define ANIM_FRAME_HEIGHT 240
#define ANIM_FRAME_SIZE   (ANIM_FRAME_WIDTH * ANIM_FRAME_HEIGHT * 2)  // bytes per frame
#define ANIM_DEFAULT_FPS  20     // frames per second
#define ANIM_DIR          "/roll"

// ============================================================================
// WIRING REFERENCE
// ============================================================================
/*
 * XIAO ESP32-C3 Pin Summary:
 *
 *   D0  (GPIO2)  → GC9A01 BLK    (PWM backlight)
 *   D1  (GPIO3)  → GC9A01 RES    (reset)
 *   D2  (GPIO4)  → GC9A01 DC     (data/command)
 *   D3  (GPIO5)  → GC9A01 CS     (SPI chip select)
 *   D4  (GPIO6)  → I2C SDA       (MCP23017 + MPU6050 + MAX17048)
 *   D5  (GPIO7)  → I2C SCL       (MCP23017 + MPU6050 + MAX17048)
 *   D6  (GPIO21) → SD Card CS    (SPI chip select)
 *   D7  (GPIO20) → MCP23017 INTA (interrupt - future sleep/wake)
 *   D8  (GPIO8)  → SPI SCK       (GC9A01 + SD card, shared)
 *   D9  (GPIO9)  → SPI MISO      (SD card only)
 *   D10 (GPIO10) → SPI MOSI      (GC9A01 + SD card, shared)
 *
 * I2C Bus (D4/D5):
 *   MCP23017  @ 0x20  (A0=A1=A2=GND)
 *   MPU6050   @ 0x68  (AD0=GND)
 *   MAX17048  @ 0x36  (fixed)
 *
 * MCP23017 Buttons (internal pull-ups, no external resistors):
 *   GPA0 → Spare button → GND
 *   GPA1 → Mode button  → GND
 *   GPA2 → Qty+ button  → GND
 *   GPA3 → Qty- button  → GND
 *   A0, A1, A2 → GND    (sets address 0x20)
 *   RESET → 3V3
 *   INTA  → D7 (GPIO20)
 *
 * MPU6050:
 *   VCC→3V3, GND→GND, SDA→D4, SCL→D5, AD0→GND
 *   INT, XDA, XCL → not connected
 *
 * MAX17048:
 *   VCC→3V3, GND→GND, SDA→D4, SCL→D5, CELL→Battery+
 *   ALRT, QSTRT → not connected
 *
 * SD Card Module:
 *   VCC→3V3, GND→GND, MISO→D9, MOSI→D10, SCK→D8, CS→D6
 *
 * Display (GC9A01):
 *   VCC→3V3, GND→GND, SCL→D8, SDA→D10, RES→D1, DC→D2, CS→D3
 *   BLK → D0 (PWM backlight - was 3V3)
 */

#endif
