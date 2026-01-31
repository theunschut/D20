# D20 Dice — Breadboard Pin Layout

**Board:** Seeed Studio XIAO ESP32-C3
**Date:** 2026-01-31

---

## XIAO ESP32-C3 Pin Assignments

| D Pin | GPIO | Function | Connected To | Notes |
|-------|------|----------|--------------|-------|
| D0 | 2 | Backlight PWM | GC9A01 BLK | Strapping pin — safe after boot. Was voltage-divider battery input. |
| D1 | 3 | Display Reset | GC9A01 RES | |
| D2 | 4 | Display DC | GC9A01 DC | Data/Command select |
| D3 | 5 | Display CS | GC9A01 CS | SPI chip select |
| D4 | 6 | **I2C SDA** | MCP23017 + MPU6050 + MAX17048 | Shared I2C data line |
| D5 | 7 | **I2C SCL** | MCP23017 + MPU6050 + MAX17048 | Shared I2C clock line |
| D6 | 21 | **SD Card CS** | SD Module CS | SPI chip select for SD |
| D7 | 20 | Unused | — | INTA not used — buttons use polling |
| D8 | 8 | SPI SCK | GC9A01 SCL + SD SCK | Hardware SPI clock — shared |
| D9 | 9 | **SPI MISO** | SD Module MISO | SD card only (display is write-only) |
| D10 | 10 | SPI MOSI | GC9A01 SDA + SD MOSI | Hardware SPI MOSI — shared |

**Bold** = changed from previous layout.

---

## I2C Bus — Three Devices, No Conflicts

All three share D4 (SDA) and D5 (SCL). Each has a unique address.

| Device | Address | How Address Is Set |
|--------|---------|-------------------|
| MCP23017 | 0x20 | A0 wired to GND; A1 and A2 floating (default LOW) |
| MPU6050 | 0x68 | AD0 wired to GND. Common clones (MPU-6886) return WHO_AM_I 0x70 — code uses raw I2C to support both |
| MAX17048 | 0x36 | Fixed (not configurable) |

Pull-up resistors: most breakout boards include 4.7 kΩ pull-ups on SDA/SCL. If using bare chips, add one set of 4.7 kΩ pull-ups to 3V3 on the shared bus.

---

## GC9A01 Display Wiring

| Display Pin | Wire To | Notes |
|-------------|---------|-------|
| VCC | 3V3 | |
| GND | GND | |
| SCL | D8 (GPIO8) | **Misleading label — this is SPI SCK** |
| SDA | D10 (GPIO10) | **Misleading label — this is SPI MOSI** |
| RES | D1 (GPIO3) | Reset |
| DC | D2 (GPIO4) | Data/Command |
| CS | D3 (GPIO5) | Chip Select |
| BLK | D0 (GPIO2) | **PWM backlight — wire to D0, NOT 3V3** |

---

## MCP23017 I/O Expander Wiring

### Power & I2C & Control

| MCP23017 Pin | Wire To | Notes |
|--------------|---------|-------|
| VCC (or VDD) | 3V3 | |
| GND (or VSS) | GND | |
| SDA | D4 (GPIO6) | I2C data — shared bus |
| SCL | D5 (GPIO7) | I2C clock — shared bus |
| A0 | GND | Address bit — sets 0x20 |
| A1 | — | Not connected (floating, defaults LOW) |
| A2 | — | Not connected (floating, defaults LOW) |
| RESET | 3V3 | Tie high to keep chip enabled |
| INTA | — | Not connected (polling used instead) |
| INTB | — | Not connected |

### Button Connections (Port A)

Internal pull-ups are enabled in software — **no external resistors needed**.
Each button connects between the MCP23017 pin and GND.

| MCP Pin | Button | Action |
|---------|--------|--------|
| GPA0 | Spare (4th) | Function TBD |
| GPA1 | Mode | Short press = next dice type; Long press = toggle Adv/Dis |
| GPA2 | Qty+ | Increase dice count |
| GPA3 | Qty- | Decrease dice count |
| GPA4–GPA7 | — | Not connected (spare) |
| GPB0–GPB7 | — | Not connected (spare) |

---

## MPU6050 Accelerometer Wiring

| MPU6050 Pin | Wire To | Notes |
|-------------|---------|-------|
| VCC | 3V3 | |
| GND | GND | |
| SDA | D4 (GPIO6) | I2C data — shared bus |
| SCL | D5 (GPIO7) | I2C clock — shared bus |
| AD0 | GND | Sets I2C address to 0x68 |
| INT | — | Not connected (shake detection uses polling) |
| XDA | — | Not connected (auxiliary I2C, unused) |
| XCL | — | Not connected |

---

## MAX17048 Fuel Gauge Wiring

| MAX17048 Pin | Wire To | Notes |
|--------------|---------|-------|
| VCC | 3V3 | |
| GND | GND | |
| SDA | D4 (GPIO6) | I2C data — shared bus |
| SCL | D5 (GPIO7) | I2C clock — shared bus |
| CELL | Battery + | **Required** — chip will not respond on I2C without a battery connected |
| ALRT | — | Not connected (low-battery alert, optional future use) |
| QSTRT | — | Not connected |

---

## SD Card Module Wiring

| SD Module Pin | Wire To | Notes |
|---------------|---------|-------|
| VCC (3V3) | 3V3 | Some modules have a 5V input with onboard regulator — check yours |
| GND | GND | |
| MISO | D9 (GPIO9) | SD card only — display doesn't use MISO |
| MOSI | D10 (GPIO10) | Shared with display |
| SCK | D8 (GPIO8) | Shared with display |
| CS | D6 (GPIO21) | Separate chip select for SD |

**SPI sharing note:** The display and SD card share SCK and MOSI. Each has its own CS pin. Only one device is active at a time — the libraries handle CS automatically. Never try to read SD and write display simultaneously.

---

## Shake-to-Roll Tuning

After wiring, these values in `Config.h` control shake sensitivity:

| Constant | Default | Effect |
|----------|---------|--------|
| `SHAKE_THRESHOLD` | 18.0 m/s² | Acceleration magnitude needed to start a shake. At rest ≈ 9.81. Lower = more sensitive. |
| `SHAKE_DURATION` | 200 ms | How long the shake must be sustained before triggering. Brief dips below `SHAKE_THRESHOLD` are allowed within a 150 ms grace window — shake is only abandoned if magnitude stays below threshold for the full 150 ms. |
| `DEBOUNCE_DELAY` | 1000 ms | Cooldown after a roll before the next one is allowed. |
| `REST_THRESHOLD` | 12.0 m/s² | Magnitude below which the device counts as "at rest". Must stay below this for `REST_TIME` before the next shake is armed. |
| `REST_TIME` | 500 ms | Device must be still (below `REST_THRESHOLD`) for this long before it's ready again. |

---

## SD Card Animation Format

Animation frames are stored as raw RGB565 binary files:

- **Directory:** `/roll/` (root of SD card)
- **Files:** `001.bin`, `002.bin`, `003.bin`, …
- **Each frame:** 240 × 240 pixels × 2 bytes = **115,200 bytes**
- **Byte order:** Little-endian (LSB first)
- **Playback:** 20 FPS default (set `ANIM_DEFAULT_FPS` in Config.h)

If no `/roll/` directory or no `.bin` files are found, the dice falls back to the built-in software animation automatically.

---

## Required Arduino Libraries

| Library | Purpose | Install via |
|---------|---------|-------------|
| Adafruit GFX Library | Graphics primitives | Library Manager |
| Adafruit GC9A01A | Round display driver | Library Manager |
| Adafruit MCP23X17 | I2C GPIO expander | Library Manager |
| SparkFun MAX1704x Fuel Gauge Arduino Library | Battery fuel gauge | Library Manager |
| SD | SD card file I/O | Built-in |
| Wire | I2C communication | Built-in |
| SPI | SPI communication | Built-in |
