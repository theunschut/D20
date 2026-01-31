# Digital D20 Dice - Hardware Specification & Build Guide

**Project:** Digital D20 Dice Roller for D&D
**Target Board:** Seeed Studio XIAO ESP32-C3
**Display:** 1.28" Round GC9A01 TFT (240x240 pixels)
**Last Updated:** January 26, 2026

---

## Project Overview

A battery-powered, motion-activated digital dice roller with the following features:
- Multiple dice types (D4, D6, D8, D10, D12, D20, D100)
- Roll 1-4 dice at once
- Advantage/Disadvantage mechanics (D&D 5e)
- **Shake-to-roll using MPU6050 accelerometer** (no dedicated roll button)
- 3 buttons for dice configuration (Mode, Qty+, Qty-)
- 1 spare button (function to be determined)
- SD card storage for roll animations
- Magnetic pogo pin charging (no USB port on device)
- Accurate battery monitoring with MAX17048
- Auto-dimming display for extended battery life
- Compact D20-shaped enclosure (~45-50mm diameter)

---

## Complete Component List

### Core Electronics
| Component | Specification | Quantity | Purpose | Est. Cost |
|-----------|---------------|----------|---------|-----------|
| XIAO ESP32-C3 | Seeed Studio, ESP32-C3 RISC-V | 1 | Main controller | ~€5 |
| GC9A01 Display | 1.28" round, 240x240, SPI | 1 | Visual output | ~€8 |
| MPU6050 | 6-axis accel/gyro, I2C | 1 | Motion detection (shake-to-roll) | ~€2 |
| MCP23017 | 16-bit I/O expander, I2C | 1 | Button inputs + extras | ~€2 |
| MAX17048 | LiPo fuel gauge, I2C | 1 | Accurate battery monitoring | ~€7 |
| SD Card Module | MicroSD, SPI interface | 1 | Store animation frames | ~€2 |

### Power System
| Component | Specification | Quantity | Purpose | Est. Cost |
|-----------|---------------|----------|---------|-----------|
| TP4056 Module | USB-C charging, protection | 1 | LiPo battery charger | ~€1 |
| LiPo Battery | 602030, 300-350mAh, 3.7V | 1 | Power source | ~€3 |
| Pogo Pins | 2-pin magnetic connector | 1 set | Charging interface | ~€3 |

### User Interface
| Component | Specification | Quantity | Purpose | Est. Cost |
|-----------|---------------|----------|---------|-----------|
| Tactile Buttons | 6x6mm push buttons | 4 | User input | ~€1 |
| MicroSD Card | 8-16GB, Class 10 | 1 | Animation storage | ~€3 |

### Miscellaneous
| Component | Specification | Quantity | Purpose | Est. Cost |
|-----------|---------------|----------|---------|-----------|
| Silicone Wire | 28 AWG, flexible | As needed | Connections | ~€2 |
| Pin Headers | 2.54mm pitch | As needed | Board connections | ~€1 |

**Total Estimated Cost:** ~€40

---

## Pin Assignment - XIAO ESP32-C3

### All 11 Physical Pins Used

| D Pin | GPIO | Function | Connection | Protocol | Notes |
|-------|------|----------|------------|----------|-------|
| D0 | GPIO2 | Display Backlight PWM | GC9A01 BLK | PWM | Brightness control for battery saving |
| D1 | GPIO3 | Display Reset | GC9A01 RES | Digital Out | Hardware reset |
| D2 | GPIO4 | Display Data/Command | GC9A01 DC | Digital Out | Register select |
| D3 | GPIO5 | Display Chip Select | GC9A01 CS | SPI CS | Active low |
| D4 | GPIO6 | I2C Data | SDA (shared) | I2C SDA | MPU6050 + MCP23017 + MAX17048 |
| D5 | GPIO7 | I2C Clock | SCL (shared) | I2C SCL | MPU6050 + MCP23017 + MAX17048 |
| D6 | GPIO21 | SD Card Chip Select | SD CS | SPI CS | Active low |
| D7 | GPIO20 | Interrupt Input | MCP23017 INT | Digital In | Optional: button press notification |
| D8 | GPIO8 | Hardware SPI Clock | SCLK | SPI Clock | Display + SD Card (shared) |
| D9 | GPIO9 | Hardware SPI MISO | MISO | SPI MISO | SD Card only (display doesn't need) |
| D10 | GPIO10 | Hardware SPI MOSI | MOSI | SPI MOSI | Display + SD Card (shared) |

**Important Notes:**
- Hardware SPI pins (D8, D9, D10) cannot be changed on XIAO ESP32-C3
- Hardware I2C pins (D4, D5) are default but can be reassigned if needed
- All pins have internal pull-up/pull-down capabilities
- GPIO2 (D0) has PWM capability for smooth backlight dimming

---

## I2C Device Addresses

The I2C bus (D4/D5) is shared by three devices with no address conflicts:

| Device | I2C Address | Configurable? | Notes |
|--------|-------------|---------------|-------|
| MCP23017 | 0x20 | Yes (A0-A2 pins) | Set A0=A1=A2=GND for 0x20 |
| MAX17048 | 0x36 | No | Fixed address |
| MPU6050 | 0x68 | Yes (AD0 pin) | AD0=GND for 0x68, AD0=VCC for 0x69 |

**Total I2C devices:** 3
**No conflicts:** ✅ All addresses are unique

---

## MCP23017 Pin Assignments

The MCP23017 provides 16 additional GPIO pins via I2C:

### Port A (GPA0-GPA7)
| Pin | Function | Connection | Pull-up | Notes |
|-----|----------|------------|---------|-------|
| GPA0 | Button 4 (TBD) | Button + GND | Internal | Future feature - function undefined |
| GPA1 | Mode Button | Button + GND | Internal | Dice type / Adv-Dis toggle |
| GPA2 | Qty Plus | Button + GND | Internal | Increase dice count |
| GPA3 | Qty Minus | Button + GND | Internal | Decrease dice count |
| GPA4 | TP4056 CHRG | Status pin | 10kΩ external | Charging indicator (if available) |
| GPA5 | TP4056 STDBY | Status pin | 10kΩ external | Full charge indicator (if available) |
| GPA6 | Spare | - | - | Future: LED, sensor, etc. |
| GPA7 | MAX17048 ALRT | Alert pin | Internal | Low battery warning (optional) |

### Port B (GPB0-GPB7)
| Pin | Function | Notes |
|-----|----------|-------|
| GPB0 | Spare | Future: Buzzer, vibration motor |
| GPB1 | Spare | Future features |
| GPB2 | Spare | Future features |
| GPB3 | Spare | Future features |
| GPB4 | Spare | Future features |
| GPB5 | Spare | Future features |
| GPB6 | Spare | Future features |
| GPB7 | Spare | Future features |

**Available for future expansion:** 10 pins (GPA6, GPA7 optional, GPB0-GPB7)

**4th Button (GPA0) - Potential Functions:**
- **Quick Roll Preset:** One-touch roll with saved settings (e.g., always 2d20 advantage)
- **Statistics Display:** Show roll history, averages, streaks
- **Settings Menu:** Access brightness, sensitivity, animation speed
- **Dice Lock:** Lock current dice type to prevent accidental changes
- **Animation Skip:** Skip to result immediately without animation
- **Bluetooth Sync:** Trigger roll on all connected dice simultaneously (future feature)
- **Screenshot/Log:** Save current roll to SD card
- **Color Theme:** Cycle through different display color schemes

**Interrupt Configuration:**
- MCP23017 INT pin → D7 (GPIO20) on XIAO
- Configure to trigger on any button press (GPA0-GPA3)
- Allows ESP32 to sleep and wake on button press

---

## Complete Wiring Diagram

### Power Distribution

```
USB-C (Charging Box)
    ↓
[Pogo Pin Connector]
    ↓
TP4056 Charger Module
  IN+ ← Pogo Pin +
  IN- ← Pogo Pin - (GND)
  OUT+ → LiPo Battery +
  OUT- → LiPo Battery - (GND)
    ↓
LiPo Battery (602030, 300-350mAh)
  + → XIAO BAT+ pad (on back)
  + → MAX17048 CELL pin
  - → XIAO GND
    ↓
XIAO ESP32-C3 Power Output
  3V3 → Power all 3.3V devices
  GND → Common ground
```

### GC9A01 Display (SPI)

```
Display Pin → XIAO Pin
VCC  → 3V3
GND  → GND
SCL  → D8 (GPIO8)   - Hardware SPI Clock
SDA  → D10 (GPIO10) - Hardware SPI MOSI
RES  → D1 (GPIO3)   - Reset
DC   → D2 (GPIO4)   - Data/Command
CS   → D3 (GPIO5)   - Chip Select
BLK  → D0 (GPIO2)   - Backlight PWM control
```

**Display Notes:**
- Pin labels can be misleading: "SDA" = MOSI, "SCL" = SCLK
- This is SPI, not I2C despite labeling
- BLK connected to PWM pin for brightness control
- Hardware SPI @ 40MHz for fast refresh

### SD Card Module (SPI - Shared Bus)

```
SD Card Pin → XIAO Pin
VCC  → 3V3
GND  → GND
MISO → D9 (GPIO9)   - Hardware SPI MISO
MOSI → D10 (GPIO10) - Shared with display
SCK  → D8 (GPIO8)   - Shared with display
CS   → D6 (GPIO21)  - Separate chip select
```

**SD Card Notes:**
- Shares SPI bus with display (MOSI, SCK)
- MISO only used by SD card (display is write-only)
- CS must be HIGH when talking to display, vice versa
- Use separate CS control for each device

### MPU6050 Motion Sensor (I2C)

```
MPU6050 Pin → XIAO Pin
VCC → 3V3
GND → GND
SDA → D4 (GPIO6)  - I2C Data (shared bus)
SCL → D5 (GPIO7)  - I2C Clock (shared bus)
XDA → Not connected (auxiliary I2C, unused)
XCL → Not connected
INT → D7 (GPIO20) - Optional: motion interrupt
AD0 → GND        - Sets I2C address to 0x68
```

**MPU6050 Notes:**
- Detects shake/tilt for roll triggering
- Can generate interrupt on motion for wake-from-sleep
- Shares I2C bus with MCP23017 and MAX17048
- Configure motion threshold in software

### MCP23017 I/O Expander (I2C)

```
MCP23017 Pin → Connection
VCC   → 3V3
GND   → GND
SDA   → D4 (GPIO6)  - I2C Data (shared)
SCL   → D5 (GPIO7)  - I2C Clock (shared)
A0    → GND         - Address bit 0
A1    → GND         - Address bit 1
A2    → GND         - Address bit 2 (Address = 0x20)
RESET → 3V3         - Always enabled
INTA  → D7 (GPIO20) - Interrupt (optional, can share with MPU6050)
INTB  → Not connected

Port A (inputs with internal pull-ups):
GPA0  → Button 4 (TBD) → GND
GPA1  → Mode Button → GND
GPA2  → Qty+ Button → GND
GPA3  → Qty- Button → GND
GPA4  → TP4056 CHRG (via 10kΩ pull-up to 3.3V)
GPA5  → TP4056 STDBY (via 10kΩ pull-up to 3.3V)
GPA6  → Spare
GPA7  → MAX17048 ALRT (optional)

Port B:
GPB0-7 → All spare (future features)
```

**MCP23017 Notes:**
- Configure GPA0-3 as inputs with internal pull-ups
- Buttons pull pins LOW when pressed
- Can configure interrupt-on-change for all button pins
- INTA triggers when any configured pin changes state

### MAX17048 Fuel Gauge (I2C)

```
MAX17048 Pin → Connection
VCC  → 3V3
GND  → GND
SDA  → D4 (GPIO6)  - I2C Data (shared)
SCL  → D5 (GPIO7)  - I2C Clock (shared)
CELL → Battery +   - Direct battery voltage measurement
ALRT → MCP23017 GPA7 (optional) - Low battery alert
QSTRT → Not connected (quick start, rarely used)
```

**MAX17048 Notes:**
- Monitors battery voltage directly from LiPo positive terminal
- Provides accurate state-of-charge (SOC) calculation
- Fixed I2C address 0x36
- Can alert when battery drops below threshold (e.g., 20%)
- Requires 0.1µF and 10µF capacitors (usually on breakout board)

### TP4056 Charging Module

```
TP4056 Module Connections:

Input (from charging box):
IN+  ← USB 5V (via pogo pin)
IN-  ← GND (via pogo pin)

Output (to battery):
OUT+ → LiPo Battery +
OUT- → LiPo Battery - (GND)

Status Outputs (if available on module):
CHRG  → 10kΩ resistor → 3.3V
      └─ Wire to MCP23017 GPA4
      
STDBY → 10kΩ resistor → 3.3V
      └─ Wire to MCP23017 GPA5

Note: Some TP4056 modules don't have CHRG/STDBY pads.
If unavailable, charging status can be inferred from battery % changes.
```

**TP4056 Notes:**
- Provides CC/CV charging for single-cell LiPo
- Built-in over-charge, over-discharge protection (on most modules)
- CHRG pin: LOW when charging, HIGH otherwise
- STDBY pin: LOW when fully charged, HIGH otherwise
- If status pins unavailable, can tap LED connections or omit

### Pogo Pin Connector

```
Charging Box Side:
USB-C Port → 5V + GND
  5V  → Pogo Pin 1 (positive)
  GND → Pogo Pin 2 (negative/ground)

Dice Side:
Pogo Pin 1 (positive) → TP4056 IN+
Pogo Pin 2 (ground)   → TP4056 IN- (GND)

Pin Order/Polarity:
[Not specified - document actual pins used during assembly]
Typically: Pin 1 = +5V, Pin 2 = GND
Mark clearly on both dice and charging box!
```

**Pogo Pin Notes:**
- Use 2-pin magnetic pogo connector
- Consider spring-loaded type for better contact
- Test polarity before final assembly
- Add LED indicator on charging box (5V → LED + resistor → GND)

---

## Hardware Configuration Details

### SPI Bus Configuration

**Hardware SPI Pins (Fixed on XIAO ESP32-C3):**
- SCLK: GPIO8 (D8)
- MOSI: GPIO10 (D10)
- MISO: GPIO9 (D9)

**Device Chip Selects:**
- Display CS: GPIO5 (D3)
- SD Card CS: GPIO21 (D6)

**SPI Initialization:**
- Frequency: 40 MHz for display (fast updates)
- Mode: SPI_MODE0
- Bit order: MSB first

**Important:** 
- Only one device can be active at a time (CS LOW)
- Always set unused CS pins HIGH
- SD card may need lower SPI speed for initialization (400 kHz)

### I2C Bus Configuration

**Hardware I2C Pins (Default on XIAO ESP32-C3):**
- SDA: GPIO6 (D4)
- SCL: GPIO7 (D5)

**Bus Speed:**
- Standard mode: 100 kHz (default)
- Fast mode: 400 kHz (recommended for faster response)

**Pull-up Resistors:**
- Most breakout boards have built-in 4.7kΩ or 10kΩ pull-ups
- If using bare chips, add external 4.7kΩ pull-ups to 3.3V on SDA and SCL

### Display Backlight Power Management

**Brightness Levels:**
- Full (255): Active use, rolling dice - ~60mA draw
- Dim (128): Idle 10+ seconds - ~30mA draw
- Sleep (25): Idle 30+ seconds - ~5-10mA draw
- Off (0): Deep sleep mode - ~0mA draw

**Auto-Dimming Strategy:**
```
0-10 seconds:   Full brightness (255)
10-30 seconds:  Dim (128) - saves ~30mA
30-60 seconds:  Very dim (25) - saves ~50mA
60+ seconds:    Optional: Off (0) - saves ~60mA
```

**Activity Triggers (reset timer to full brightness):**
- Button press
- Shake/roll detection
- Display update
- Charging connected

---

## Power Budget & Battery Life

### Power Consumption Breakdown

| Component | Active | Idle | Sleep | Notes |
|-----------|--------|------|-------|-------|
| GC9A01 Display (backlight) | 60mA | 30mA (dimmed) | 5mA (very dim) | Biggest consumer |
| XIAO ESP32-C3 | 80mA | 30mA | 10mA (light sleep) | During SPI/compute |
| MPU6050 | 3.5mA | 3.5mA | 0.01mA (sleep) | Can sleep between rolls |
| MCP23017 | 1mA | 1mA | 1mA | Very low power |
| MAX17048 | 0.03mA | 0.03mA | 0.03mA | Negligible |
| SD Card | 80mA | 0.1mA | 0.1mA | Only active during animation load |

### Usage Scenarios (300mAh Battery)

**Scenario 1: Continuous Active Use**
- Display full brightness
- Frequent rolling
- Power draw: ~150mA average
- **Runtime: ~2 hours**

**Scenario 2: Gaming Session (Realistic)**
- Active 30% of time (rolling, viewing)
- Idle/dimmed 70% of time
- Power draw: ~105mA average
- **Runtime: ~2.8 hours**

**Scenario 3: Aggressive Power Saving**
- Display dims after 10s
- Very dim after 30s
- MPU6050 sleep mode between rolls
- Power draw: ~80mA average
- **Runtime: ~3.7 hours**

**Scenario 4: Standby Between Game Sessions**
- Deep sleep mode
- Wake on shake (MPU6050 interrupt)
- Power draw: ~5mA
- **Runtime: ~60 hours (2.5 days)**

### Battery Specifications

**LiPo 602030:**
- Nominal voltage: 3.7V
- Capacity: 300-350mAh (typical)
- Charge voltage: 4.2V (max)
- Discharge cutoff: 3.0V (min)
- Protection: Built-in over-charge/discharge protection PCB
- Size: 6mm × 20mm × 30mm

**Charging:**
- TP4056 charges at 1C (300-500mA typical)
- Charge time: ~1-1.5 hours from empty
- Constant current → constant voltage (CC/CV) profile

### Battery Monitoring

**MAX17048 Fuel Gauge:**
- Measures voltage: 3.0V - 4.2V
- Calculates SOC: 0-100% (±1% accuracy)
- Tracks charge/discharge rate: %/hour
- Low battery alert: Configurable threshold (e.g., 20%)

**Battery Voltage to % Mapping:**
```
4.2V  → 100%
4.1V  → 90%
4.0V  → 80%
3.9V  → 70%
3.8V  → 60%
3.7V  → 50%
3.6V  → 40%
3.5V  → 30%
3.4V  → 20%
3.3V  → 10%
3.0V  → 0%
```

---

## Design Decisions & Rationale

### Why MCP23017 Instead of Direct GPIO?

**Problem:** XIAO ESP32-C3 only has 11 GPIO pins, all were needed for display, SD card, and I2C.

**Solution:** MCP23017 I/O expander via I2C
- Adds 16 GPIO pins using only 2 I2C wires (shared with sensors)
- Frees up 4 XIAO pins that were going to be used for buttons
- Allows expansion: 12 pins still available for future features
- Supports interrupts for efficient button handling
- Cost: ~€2 (minimal)

### Why MAX17048 Instead of Voltage Divider?

**Original Plan:** Use voltage divider on D0 (ADC) to measure battery voltage
- Accuracy: ±10-15% (linear interpolation)
- Cost: €0.10 (2 resistors)

**Final Choice:** MAX17048 fuel gauge
- Accuracy: ±1% (sophisticated SOC algorithm)
- Learns battery characteristics over time
- Provides charge/discharge rate
- Frees up D0 for display backlight PWM
- Cost: ~€7 (decided value was worth it)

**Trade-off:** Higher cost, but significantly better user experience with accurate battery % and freed pin for power-saving backlight control.

### Why Display Backlight on D0 Instead of LED?

**Option A:** WS2812B RGB LED for status indication
- Cool visual effects (gold glow for nat 20, red for nat 1)
- Full color control
- Only needs 1 pin

**Option B:** Display backlight PWM control (chosen)
- Saves 30-50mA in typical use
- 40-60% battery life improvement
- Smooth brightness transitions
- More practical for extended gaming sessions

**Rationale:** Battery life is more important than RGB bling for a dice that will be used for 3-4 hour game sessions. LED can be added later via MCP23017 if desired.

### Why MPU6050 Instead of Tilt Switch?

**Original Plan:** SW-520D ball tilt sensor
- Very simple
- Cheap (~€0.50)
- Digital on/off

**Problems:**
- Not accurate enough for "dice rolling" motion
- False triggers from table bumps
- Can't distinguish shake intensity
- No programmable threshold

**Solution:** MPU6050 6-axis IMU
- Detects actual acceleration and rotation
- Can require specific "shake" motion to trigger
- Programmable sensitivity
- Can detect when dice is at rest (ready for next roll)
- Enables future features (gesture controls, counting rolls)
- Cost: ~€2 (minimal increase)

### Why SD Card for Animations?

**Why not store in ESP32 flash?**
- ESP32-C3 has limited flash (4MB total)
- 240×240 pixel frame ≈ 115KB (raw RGB565)
- Even compressed, 10 frames = ~500KB
- Flash space needed for code, fonts, other assets

**SD Card advantages:**
- Cheap: 8GB card = €3
- Can store hundreds of animation frames
- Easy to update: swap card with new animations
- Different animation sets for different dice types
- User-customizable (advanced users can add own animations)

**Trade-off:** Adds complexity (SPI management, file I/O), but enables much richer visual experience.

### Why Pogo Pins Instead of USB Port?

**Reasons:**
1. **No hole in enclosure:** Maintains water resistance, cleaner aesthetic
2. **Magnetic alignment:** Easier to place in charging dock
3. **Durability:** No connector wear from repeated plugging
4. **Safety:** No exposed metal when not charging
5. **Faster:** Just drop in dock, no fumbling with cable

**TP4056 placement:** Inside dice (not charging box)
- Protects battery during use (over-discharge protection)
- Can display charging status on screen
- Charging box is ultra-simple (just USB + pogo pins)
- Can make multiple cheap charging boxes

---

## PCB Layout Recommendations

### Component Placement Strategy

**Layer 1 (Top - Display Side):**
- GC9A01 display (largest component)
- Centered on board
- All other components arranged around edges

**Layer 2 (Bottom - Electronics Side):**
- XIAO ESP32-C3 (center)
- MPU6050 near XIAO (short I2C traces)
- MCP23017 near buttons
- MAX17048 near battery connection
- TP4056 near pogo pins
- SD card slot accessible from edge

**Circular PCB Design:**
- Match display diameter (~33mm) + ~5mm border
- Total PCB diameter: ~38-40mm
- Mount buttons around perimeter (4 corners of virtual square)
- Pogo pins on bottom, centered

### Critical Trace Considerations

**SPI Bus (High Speed - 40MHz):**
- Keep SCLK, MOSI traces as short as possible
- Match trace lengths between display and SD card
- Use ground plane underneath
- Avoid routing near I2C or power lines

**I2C Bus (Moderate Speed - 400kHz):**
- Keep SDA/SCL traces together
- Add pull-up resistors close to XIAO
- Can be longer than SPI without issues
- Add test points for debugging

**Power Distribution:**
- Wide traces for VCC and GND (3.3V @ 200mA peak)
- Star ground topology (all grounds to single point)
- Decoupling capacitors close to each IC
- Separate analog and digital grounds if possible

**Display Backlight:**
- PWM trace from GPIO2 to BLK pin
- Can handle 60mA continuous
- Consider adding small capacitor at BLK pin for smoothing

### Recommended Trace Widths (1oz copper)

| Net | Current | Width | Notes |
|-----|---------|-------|-------|
| VCC (3.3V) | 200mA | 0.5mm | Power distribution |
| GND | 200mA | 0.5mm+ | Make as wide as possible |
| SPI (SCLK, MOSI) | <10mA | 0.2mm | Keep short, controlled impedance |
| I2C (SDA, SCL) | <10mA | 0.2mm | Can be longer than SPI |
| GPIO signals | <10mA | 0.2mm | Standard digital signals |
| Battery + | 500mA | 1.0mm | Charging current |

---

## Enclosure Design Considerations

### Form Factor

**Target Shape:** D20 icosahedron (20-sided die)
- Outer diameter: ~45-50mm
- Faceted geometry with flat top for display
- Display window on top face
- 4 buttons on corners of top face
- Pogo pins on bottom face (hidden)

**Alternative:** Rounded cylinder with D20 aesthetic
- Easier to 3D print than true icosahedron
- More comfortable to hold
- Simpler button placement

### Material

**3D Printed:**
- PLA or PETG (rigid, paintable)
- Clear resin for "crystal dice" effect with internal lighting
- TPU overmold for grip (optional)

**Two-Part Design:**
- Top shell: Display + buttons visible
- Bottom shell: Electronics + battery
- Ultrasonic weld or snap-fit assembly
- Gasket for water resistance (optional)

### Button Placement

**3 Buttons arranged around display:**
```
        [Mode]
          |
[Qty-] - DISPLAY - [Qty+]
```

**4th Button (Function TBD):**
- Could be placed on side of dice
- Could be on bottom face (opposite pogo pins)
- Reserved for future features (quick preset, statistics view, settings, etc.)

**Rolling:** Shake the dice (MPU6050 accelerometer detection)
- No dedicated roll button needed
- Natural dice-like interaction
- Programmable shake sensitivity

**Alternative Layout (if 4th button function is determined):**
```
     [Mode] [Btn4]
          |
[Qty-] - DISPLAY - [Qty+]
```

### Display Window

- Flush-mounted or slightly recessed display
- Transparent acrylic/polycarbonate window
- Glued from inside for smooth exterior
- Consider anti-glare coating

### Pogo Pin Mounting

- Recessed area on bottom for alignment
- Magnets embedded in bottom shell
- Corresponding magnets in charging box
- Test hole alignment before final assembly

### Charging Box Design

**Simple dock design:**
- USB-C port on back
- Pogo pins on top (spring-loaded preferred)
- Magnets for alignment
- LED indicator: Power connected (optional)
- Angled stand (45°) to display dice screen while charging

---

## Assembly Notes & Tips

### Soldering Sequence

1. **XIAO ESP32-C3:** Solder pin headers first (or directly solder wires)
2. **Breakout boards:** Desolder bulky headers if space-constrained
3. **I2C bus:** Solder all SDA/SCL connections, test with I2C scanner
4. **SPI devices:** Add display, test basic graphics before SD card
5. **Buttons to MCP23017:** Use ribbon cable or thin silicone wire
6. **Power system:** Last! Test voltages before connecting battery

### Testing Milestones

**Stage 1: Power & Communication**
- [ ] XIAO powers on from USB
- [ ] I2C scanner detects all 3 devices (0x20, 0x36, 0x68)
- [ ] SPI devices respond (display shows test pattern)

**Stage 2: Individual Modules**
- [ ] Display shows graphics, backlight dims via PWM
- [ ] MPU6050 reads acceleration data
- [ ] MCP23017 reads button states
- [ ] MAX17048 reads battery voltage
- [ ] SD card mounts and reads files

**Stage 3: Integration**
- [ ] Buttons trigger display updates
- [ ] Shake detection triggers animations
- [ ] Battery % displays accurately
- [ ] Animations load from SD card smoothly

**Stage 4: Power Management**
- [ ] Display auto-dims after timeout
- [ ] Battery charges via pogo pins
- [ ] Charging status displays on screen
- [ ] Deep sleep mode works (wake on shake)

### Common Issues & Solutions

**Display not showing:**
- Check SPI wiring (SCL/SDA labels are misleading - these are SPI pins!)
- Verify CS pin is LOW during communication
- Try lowering SPI speed (1MHz for testing)
- Check backlight is ON (D0 = HIGH)

**I2C devices not detected:**
- Use I2C scanner code to find devices
- Check SDA/SCL are not swapped
- Verify 4.7kΩ pull-ups present (on breakout or external)
- Try lowering I2C speed (100kHz)
- Check each device address configuration (A0-A2 pins, AD0 pin)

**Buttons not responding:**
- Verify MCP23017 detected on I2C (address 0x20)
- Check internal pull-ups enabled in code
- Test with multimeter: pins should read 3.3V (pulled up), 0V when pressed
- Verify interrupt pin connection if using interrupt mode

**SD card not mounting:**
- Try lower SPI speed for initialization (400kHz)
- Check SD card is formatted FAT32
- Verify all 6 SPI connections (VCC, GND, MISO, MOSI, SCK, CS)
- Test with known-good SD card

**Battery not charging:**
- Check polarity of pogo pins (use multimeter!)
- Verify TP4056 IN+ has 5V when connected
- Check LED on TP4056 (should light up during charging)
- Ensure battery protection circuit hasn't tripped

**MAX17048 reading incorrect %:**
- Let battery charge/discharge fully once (calibration)
- Verify CELL pin connected directly to battery +
- Check I2C address (should be 0x36)
- Wait 10 minutes for chip to stabilize after power-on

---

## Software Architecture Notes

(Code implementation to be handled by Claude Code when components arrive)

### Key Software Modules Required

1. **Display Driver:**
   - Adafruit GC9A01A library
   - Hardware SPI @ 40MHz
   - Backlight PWM control (analogWrite on D0)
   - Frame buffer management for animations

2. **Motion Detection:**
   - Adafruit MPU6050 library
   - Shake detection algorithm (acceleration threshold + duration)
   - Rest detection (stable for X milliseconds = ready to roll)
   - Interrupt-based wake from sleep

3. **Button Handling:**
   - Adafruit MCP23X17 library
   - Interrupt-on-change configuration
   - Debouncing in software (50ms typical)
   - Long-press detection (1000ms for mode change)

4. **Battery Monitoring:**
   - SparkFun MAX1704x library
   - SOC reading (0-100%)
   - Voltage reading (3.0-4.2V)
   - Change rate (%/hour) for charge detection
   - Low battery alert threshold

5. **SD Card:**
   - Standard SD library
   - FAT32 filesystem
   - Sequential frame reading for animations
   - File caching for performance

6. **Power Management:**
   - ESP32 light sleep between button presses
   - Wake on MCP23017 interrupt
   - Wake on MPU6050 motion interrupt
   - Display timeout and dimming
   - MPU6050 sleep mode between rolls

### Configuration Parameters to Expose

- Shake sensitivity (acceleration threshold)
- Rest time before ready (milliseconds)
- Display timeout values (dim, sleep, off)
- Backlight brightness levels
- Low battery warning threshold
- Animation frame rate
- Button debounce time
- Long press duration

---

## Bill of Materials (BOM)

| Item | Description | Qty | Source | Part # | Unit Price | Total |
|------|-------------|-----|--------|--------|------------|-------|
| XIAO ESP32-C3 | Seeed Studio ESP32-C3 | 1 | Seeed/DigiKey | 102110635 | €5.00 | €5.00 |
| GC9A01 Display | 1.28" 240x240 Round TFT | 1 | AliExpress | - | €8.00 | €8.00 |
| MPU6050 | 6-axis IMU module | 1 | AliExpress | - | €2.00 | €2.00 |
| MCP23017 | I2C I/O expander module | 1 | AliExpress | - | €2.00 | €2.00 |
| MAX17048 | LiPo fuel gauge module | 1 | Various | - | €7.00 | €7.00 |
| SD Card Module | MicroSD SPI module | 1 | AliExpress | - | €2.00 | €2.00 |
| TP4056 | USB-C charging module | 1 | AliExpress | - | €1.00 | €1.00 |
| LiPo Battery | 602030 300-350mAh | 1 | AliExpress | - | €3.00 | €3.00 |
| Pogo Pins | 2-pin magnetic connector | 1 set | AliExpress | - | €3.00 | €3.00 |
| Push Buttons | 6x6mm tactile switch | 4 | AliExpress | - | €0.25 | €1.00 |
| MicroSD Card | 8-16GB Class 10 | 1 | Local | - | €3.00 | €3.00 |
| Silicone Wire | 28 AWG, various colors | 2m | AliExpress | - | €1.00 | €2.00 |
| Pin Headers | 2.54mm male, female | 1 set | Local | - | €1.00 | €1.00 |
| | | | | **TOTAL:** | | **€40.00** |

**Optional Components:**
| Item | Description | Price |
|------|-------------|-------|
| WS2812B LED | Addressable RGB LED | €1.00 |
| Piezo Buzzer | 3-5V active buzzer | €0.50 |
| Vibration Motor | 3V coin motor | €1.00 |

---

## Testing & Debugging Tools

### Recommended Tools

1. **Multimeter:**
   - Continuity testing
   - Voltage verification (3.3V, 4.2V, 5V)
   - Current measurement (power consumption)

2. **USB Serial Monitor:**
   - Arduino IDE Serial Monitor
   - View debug output from XIAO
   - I2C scanner results
   - Sensor readings

3. **Logic Analyzer (optional):**
   - Debug SPI timing issues
   - Verify I2C communication
   - Troubleshoot button interrupts

4. **Oscilloscope (optional):**
   - Check PWM signal quality
   - Verify SPI clock frequency
   - Debug power supply noise

### Test Code Snippets (Reference Only)

**I2C Scanner:**
Check which devices are detected on I2C bus. Should show 0x20, 0x36, 0x68.

**SPI Test:**
Display should show basic graphics (rectangles, circles, text) to verify communication.

**Button Test:**
Serial monitor shows which button is pressed via MCP23017.

**Battery Test:**
Display current voltage and SOC percentage from MAX17048.

**Motion Test:**
Serial monitor shows acceleration values when shaking MPU6050.

**SD Card Test:**
List files on SD card, load and display a test image.

---

## Revision History

| Date | Version | Changes | Author |
|------|---------|---------|--------|
| 2026-01-26 | 1.0 | Initial hardware specification based on conversation with Theun | Claude |

---

## Additional Resources

### Libraries Required
- Adafruit GFX Library
- Adafruit GC9A01A
- Adafruit MPU6050
- Adafruit MCP23X17
- SparkFun MAX1704x Fuel Gauge
- SD (built-in)
- Wire (built-in)
- SPI (built-in)

### Documentation Links
- XIAO ESP32-C3: https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/
- GC9A01 Datasheet: Search "GC9A01A datasheet"
- MPU6050 Datasheet: https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
- MCP23017 Datasheet: https://www.microchip.com/en-us/product/MCP23017
- MAX17048 Datasheet: https://www.analog.com/en/products/max17048.html
- TP4056 Datasheet: Search "TP4056 datasheet"

### GitHub Repository
- Project code: https://github.com/theunschut/D20
- Development notes: claude.md (in repo)
- Arduino sketches: D20/ directory

---

## Notes for Claude Code

When components arrive and you begin software development:

1. **Start with individual module testing** - get each sensor/display working standalone before integration
2. **Reference Config.h** - all pin definitions are centralized there
3. **Hardware SPI is critical** - use 40MHz for display performance
4. **I2C bus is shared** - all three devices on same SDA/SCL
5. **Power management is key** - implement backlight dimming early for battery life
6. **SD card animations** - load frames sequentially, don't try to buffer entire animation
7. **MPU6050 tuning** - shake threshold will need experimentation for good feel
8. **MAX17048 calibration** - let battery charge/discharge once for accurate readings

**This document contains all hardware specifications needed for implementation. Code structure, algorithms, and feature implementation are deferred to Claude Code.**

---

*End of Hardware Specification Document*