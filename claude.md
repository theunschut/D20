# Digital D20 Dice Project - Development Notes

## Project Overview
A digital D20 dice roller using an ESP32 microcontroller and a round GC9A01 display, designed for tabletop RPG gaming (D&D, etc.).

## Project Documentation Files
- **`claude.md`** (this file) - Development notes, troubleshooting, hardware configurations, and code organization guidelines
- **`Resources/Instructions.txt`** - Original project requirements and initial wiring specifications
- **`Resources/future-features.md`** - Planned features and enhancement ideas for future development
- **`README.md`** - Project overview, setup instructions, and usage guide

## Development Environment

### Operating System: Windows
This project is being developed on a **Windows machine**.

**IMPORTANT WARNING:**
- **NEVER use `2>nul` in bash commands** - This is Windows CMD syntax that will create a file called `nul` when used in Git Bash/WSL
- The file `nul` is a **reserved name in Windows** and is extremely difficult to remove
- Use `2>/dev/null` instead for stderr redirection in bash on Windows
- If a `nul` file is accidentally created, it must be removed using special Windows commands or file manager tricks

### Development Tools
- **Arduino IDE** - For compiling and uploading to ESP32 boards
- **Git Bash / WSL** - For command-line operations
- **Board**: XIAO ESP32-C3 selected in Arduino IDE

## Final Working Configuration

### Current Hardware (Production)
- **Microcontroller**: Seeed Studio XIAO ESP32-C3 (soldered headers)
- **Display**: 1.28" Round TFT LCD, 240x240 pixels, GC9A01 driver
- **Input**: 4 push buttons (Roll, Mode, Qty+, Qty-)
- **Power**: USB (3.3V) or battery

### Previous Development Hardware
- **Microcontroller**: Arduino Nano ESP32 (ESP32-S3)
- **Display**: 1.28" Round TFT LCD, 240x240 pixels, GC9A01 driver
- **Input**: 4 push buttons
- **Power**: USB (3.3V)

**Note**: Development was done on Arduino Nano ESP32 due to pre-soldered headers. Now migrated to XIAO ESP32-C3 (with soldered headers) for smaller form factor, ideal for a portable dice roller.

### Wiring Diagrams

#### XIAO ESP32-C3 (Current/Production Build)
**GC9A01 Display to XIAO ESP32-C3:**
- VCC → 3V3
- GND → GND
- SCL → D8 (GPIO8 - Hardware SPI SCK)
- SDA → D10 (GPIO10 - Hardware SPI MOSI)
- RES → D1 (GPIO3 - Reset)
- DC → D2 (GPIO4 - Data/Command)
- CS → D3 (GPIO5 - Chip Select)
- BLK → 3V3 (Backlight)

**Buttons:**
- Roll Button → D7 (GPIO20) + GND
- Mode Button → D6 (GPIO21) + GND
- Qty Plus (+) → D4 (GPIO6) + GND
- Qty Minus (-) → D5 (GPIO7) + GND

**Note:** D0 (GPIO2) is a strapping pin - avoided entirely. D9 (GPIO9) is hardware SPI MISO - kept free.

**Important Notes for XIAO:**
- D9 (GPIO9) is the hardware SPI MISO pin - kept free (not used by display but reserved for SPI)
- **Hardware SPI default pins:** D8=GPIO8 (SCK), D10=GPIO10 (MOSI)
- Display uses 5 pins: D1, D2, D3, D8 (SCK), D10 (MOSI)
- Available GPIO pins after display: D0, D4, D5, D6, D7 (5 pins available, using 4 for buttons)
- **Pin headers MUST be soldered** - un-soldered pins cause unreliable SPI communication
- **CRITICAL: Use GPIO numbers in code, not D-numbers** - Arduino library expects GPIO pin numbers (e.g., GPIO5 for D3)
- **CRITICAL: Wire to correct hardware SPI pins** - Must use D8/D10 for hardware SPI, not D4/D5
- **Using HARDWARE SPI @ 40MHz** - Works perfectly with correct wiring and GPIO numbers

#### Arduino Nano ESP32 (Development/Testing)
**GC9A01 Display to Arduino Nano ESP32:**
- VCC → 3.3V
- GND → GND
- SCL → D13 (SPI SCLK)
- SDA → D11 (SPI MOSI)
- RES → D7 (Reset)
- DC → D8 (Data/Command)
- CS → D9 (Chip Select)
- BLK → 3.3V (Backlight)

**Button:**
- One side → D2
- Other side → GND

### Software Libraries
- **Adafruit GFX Library** - Graphics primitives
- **Adafruit GC9A01A** - GC9A01 display driver

### Upload Method
Use **Sketch > Upload Using Programmer** in Arduino IDE for reliable uploads to Arduino Nano ESP32.

## Key Findings & Troubleshooting

### Display Pin Labeling Issue
**Critical Discovery**: The GC9A01 displays from AliExpress have misleading pin labels:
- Pin labeled "SDA" is actually SPI **MOSI** (Master Out Slave In)
- Pin labeled "SCL" is actually SPI **SCLK** (Serial Clock)
- This is **NOT** an I2C interface despite the labeling suggesting otherwise
- Source: AliExpress customer review confirmed this labeling confusion
- **Common Mistake**: Easy to swap SCL/SDA on breadboard - double-check wiring if display initializes but shows nothing

### Pin Order Confusion
The display PCB shows pins in reverse order when viewed from front vs back:
- **Front reading (left to right)**: GND, VCC, SCL, SDA, RES, DC, CS, BLK
- **Back reading (left to right)**: BLK, CS, DC, RES, SDA, SCL, VCC, GND
- When connecting from the back (solder side), the order is reversed

### Library Compatibility Issues

#### TFT_eSPI Library
**Status**: ❌ Does NOT work reliably
- Causes crashes on both Arduino Nano ESP32 and XIAO ESP32-C3
- Error: `assert failed: xQueueSemaphoreTake queue.c:1545`
- Issue confirmed in multiple forum posts (2024-2025)
- Users report blank screens even with correct wiring

#### Arduino_GFX Library
**Status**: ❌ Does NOT work
- Compilation error on Arduino Nano ESP32: `fatal error: esp32-hal-periman.h: No such file or directory`
- Version incompatibility with ESP32 board package
- Not recommended for this project

#### Adafruit_GC9A01A Library
**Status**: ✅ WORKS PERFECTLY
- Confirmed working configuration found in Arduino forums
- Reliable initialization and display output
- Software SPI mode works best
- This is the recommended library for GC9A01 + Arduino Nano ESP32

### Board-Specific Issues

#### Arduino Nano ESP32
**Status**: ✅ Working (with correct pins)
- Initial attempts failed due to incorrect pin assignments
- **Working pin configuration** (confirmed from forum post):
  - CS → D9 (not D10)
  - DC → D8
  - RST → D7 (not D9)
  - MOSI → D11
  - SCLK → D13
- Upload method: Must use "Upload Using Programmer"
- Regular upload gets stuck or gives DFU errors

#### XIAO ESP32-C3
**Status**: ✅ WORKING PERFECTLY with Hardware SPI
- **Current production board** with small form factor (21mm x 17.8mm)
- Pin headers soldered successfully
- **Root causes identified**:
  1. Must use GPIO numbers in code, not D-numbers (e.g., GPIO5 for D3)
  2. Must wire to hardware SPI pins D8/D10 for hardware SPI
  3. Easy to swap SCL/SDA on breadboard (double-check wiring!)
- Hardware SPI @ 40MHz working flawlessly
- All 4 buttons functional

**Why XIAO is ideal for final product:**
1. **Size**: Tiny footprint (21mm x 17.8mm) perfect for handheld dice
2. **Power**: Built-in battery charging circuit for LiPo batteries
3. **Cost**: Lower cost than Arduino Nano ESP32
4. **Integration**: Pairs perfectly with round 1.28" GC9A01 display for compact design
5. **GPIO**: Has sufficient pins for display + 4 buttons

**Final working pin configuration:**
- CS → GPIO5 (D3 board label)
- DC → GPIO4 (D2 board label)
- RST → GPIO3 (D1 board label)
- Hardware SPI pins: MOSI → GPIO10 (D10), SCLK → GPIO8 (D8)
- Buttons: GPIO20 (D7-Roll), GPIO21 (D6-Mode), GPIO6 (D4-Qty+), GPIO7 (D5-Qty-)

**Critical Requirements:**
1. Pin headers MUST be soldered for reliable SPI operation
2. Use GPIO numbers in Arduino code, NOT D-numbers from board labels

### Display Testing - ST7789 vs GC9A01

#### ST7789 Display (2.0" 240x320)
**Status**: ✅ Works immediately
- Used as test display to verify overall setup
- Worked first try with Adafruit_ST7789 library
- Helped confirm wiring methodology was correct
- Proved the issue was GC9A01-specific, not general setup problem

#### GC9A01 Display (1.28" Round 240x240)
**Status**: ✅ Works with correct pin configuration
- Required specific pin assignment (D7/D8/D9 combo)
- Works perfectly once correct pins identified
- Beautiful round display ideal for dice application

## Code Features

### Core Functionality
1. **Random Number Generation**: 1-20 using `random(1, 21)`
2. **Debouncing**: 500ms debounce on button press
3. **Visual Feedback**: Red flash animation during roll
4. **Centered Display**: Numbers properly centered on round screen

### Special Effects
- **Natural 20 (Critical Hit)**: Displays in YELLOW (0xFFE0)
- **Natural 1 (Critical Fail)**: Displays in RED (0xF800)
- **Normal Rolls**: Display in WHITE (0xFFFF)
- **Border**: Green double-circle border for D&D aesthetic

### Color Definitions (RGB565 format)
```cpp
#define COLOR_BG      0x0000  // Black
#define COLOR_TEXT    0xFFFF  // White
#define COLOR_FLASH   0xF800  // Red
#define COLOR_D20     0x07E0  // Green
#define COLOR_CRIT    0xFFE0  // Yellow (natural 20)
#define COLOR_FAIL    0xF800  // Red (natural 1)
```

## Common Problems & Solutions

### Problem: Display shows backlight but no content
**Causes**:
1. Wrong pin assignments (most common)
2. Incompatible library (TFT_eSPI)
3. Poor connections (XIAO un-soldered pins)
4. Wrong board selected in Arduino IDE

**Solution**:
- Use exact pin configuration documented above
- Use Adafruit_GC9A01A library
- Ensure solid connections
- Select correct board in Tools > Board

### Problem: Upload fails on Arduino Nano ESP32
**Symptoms**:
- "uploading error: exit status 74"
- "No DFU capable USB device available"
- Upload hangs at "Connecting..."

**Solution**:
- Use **Sketch > Upload Using Programmer** instead of regular Upload
- Don't use "Upload" button or Ctrl+U
- This bypasses DFU mode issues

### Problem: Code compiles but crashes on startup
**Error**: `assert failed: xQueueSemaphoreTake queue.c:1545`

**Cause**: Using TFT_eSPI library or Arduino_GFX library

**Solution**: Switch to Adafruit_GC9A01A library

### Problem: "Display init SUCCESS" but nothing shows
**Cause**: Library sending commands but display not responding (initialization issue)

**Solution**:
- Verify all 8 pins are connected
- Check VCC is 3.3V (not 5V)
- Try adding small delays in initialization
- Verify using Adafruit library, not others

## Research Sources

### Forum Posts (2024-2025)
1. **Arduino Forum**: "Arduino nano ESP32 works with Adafruit_GC9A01 but not with TFT_eSPI"
   - Confirmed Adafruit_GC9A01A works on Nano ESP32
   - Pin configuration: CS=D9, DC=D8, RST=D7

2. **GitHub Issues**: "Arduino Nano ESP32 s3 blank screen on GC9A01 with TFT_eSPI #3787"
   - Confirmed TFT_eSPI compatibility issues
   - Users recommend Adafruit library instead

3. **Seeed Forum**: "GC9A01 and Xiaio esp32"
   - XIAO ESP32 requires soldered pins for SPI
   - Loose connections cause intermittent failures

4. **AliExpress Reviews**: Customer confirmed:
   - Display works with Arduino SPI_TFT library
   - Pin labels are misleading (SDA/SCL are SPI not I2C)

### Key Takeaway
The GC9A01 display ecosystem has significant library compatibility issues in 2024-2025, particularly with newer ESP32 boards. The Adafruit_GC9A01A library is the most reliable choice for Arduino Nano ESP32.

## Future Enhancement Ideas

1. **Animation Improvements**
   - Spinning number animation during roll
   - Particle effects for critical hits/fails
   - Smooth transitions

2. **Multi-Dice Support**
   - D4, D6, D8, D10, D12 modes
   - Mode selection via button hold
   - Display dice type on screen

3. **Statistics**
   - Roll history
   - Average tracking
   - Streak counter

4. **Power Management**
   - Battery operation (LiPo)
   - Sleep mode when inactive
   - Low battery indicator

5. **Audio**
   - Buzzer for dice roll sound
   - Different tones for critical/fail

6. **Enclosure**
   - 3D printed case
   - Button integrated into design
   - Portable form factor

## Bill of Materials

### Final Product (XIAO-based)
| Item | Specification | Quantity | Notes |
|------|---------------|----------|-------|
| Microcontroller | Seeed Studio XIAO ESP32-C3 | 1 | ESP32-C3 based, ultra-compact |
| Display | GC9A01 1.28" 240x240 | 1 | Round TFT LCD |
| Button | Tactile push button | 1 | Normally open |
| Pin Headers | 2.54mm pitch | 1 set | **Must be soldered** to XIAO |
| LiPo Battery | 3.7V 100-500mAh | 1 | Optional, for portable use |
| USB Cable | USB-C | 1 | For XIAO programming/charging |

### Development Setup (Nano-based)
| Item | Specification | Quantity | Notes |
|------|---------------|----------|-------|
| Microcontroller | Arduino Nano ESP32 | 1 | ESP32-S3 based, pre-soldered headers |
| Display | GC9A01 1.28" 240x240 | 1 | Round TFT LCD |
| Button | Tactile push button | 1 | Normally open |
| Jumper Wires | Male-to-female | 10 | For breadboard connections |
| USB Cable | USB-C | 1 | For power/programming |

### Why Two Boards?
- **Nano ESP32**: Used for development because headers are already soldered, faster iteration
- **XIAO ESP32-C3**: Target for final product due to small size and battery charging capability

## Development Timeline Summary

1. **Initial Setup**: Attempted XIAO ESP32-C3 with TFT_eSPI → Failed (library incompatibility)
2. **Library Testing**: Tried multiple libraries on XIAO (TFT_eSPI, Arduino_GFX, Adafruit) → All showed init success but no visuals
3. **Root Cause Discovery**: Identified un-soldered XIAO pins causing intermittent SPI communication failures
4. **Hardware Switch**: Moved to Arduino Nano ESP32 (has soldered headers) → Upload issues
5. **Upload Method**: Discovered "Upload Using Programmer" for Nano → Upload success
6. **Display Testing**: Tested ST7789 display on Nano → Immediate success (proved setup methodology)
7. **Pin Research**: Found forum post with working GC9A01 + Nano ESP32 pin config → Success
8. **GC9A01 Success**: GC9A01 working on Nano with Adafruit library → Development complete
9. **Next Phase**: Need to solder XIAO headers and port working code to XIAO for final product

**Current Status**: Development complete on Nano ESP32, pending XIAO header soldering for production build.

## Lessons Learned

1. **Don't trust display pin labels** - Verify actual protocol (I2C vs SPI)
2. **Check forum compatibility** before choosing libraries for new boards
3. **Test with known-working hardware** (ST7789) to isolate variables
4. **Pin assignments matter** - Even switching CS and RST pins can make/break functionality
5. **Software SPI more reliable** than hardware SPI for these displays (when using wrong pin numbers!)
6. **Community knowledge invaluable** - Forum posts saved hours of debugging
7. **Upload method matters** - "Upload Using Programmer" bypasses many ESP32 issues
8. **Soldered connections are CRITICAL for SPI** - Un-soldered pins work for slow I2C but fail for high-speed SPI
9. **Development board strategy** - Use a board with pre-soldered headers (like Nano) for faster development, then migrate to target board (XIAO) once code is proven
10. **SPI pin conflicts** - Avoid using hardware SPI MISO pin (D9/GPIO21 on XIAO) for other purposes like buttons
11. **"Display init SUCCESS" doesn't mean working** - Library can successfully initialize but still fail during pixel data transfer with poor connections
12. **CRITICAL: GPIO vs D-numbers on XIAO** - Arduino code must use GPIO pin numbers (e.g., GPIO5), not board labels (e.g., D3). This was the root cause of "init success but no display" issue with soldered headers.
13. **CRITICAL: Hardware SPI requires specific pins** - For hardware SPI to work on XIAO ESP32-C3, must wire to D8 (GPIO8/SCK) and D10 (GPIO10/MOSI), not arbitrary pins like D4/D5. Software SPI can use any pins, but hardware SPI uses board-specific defaults.
14. **Easy to swap SCL/SDA** - When wiring on a breadboard, it's easy to accidentally swap the SCL and SDA pins. If display initializes successfully but shows no visuals, check SCL/SDA are not reversed.
15. **GPIO2 is a strapping pin on ESP32-C3** - D0 (GPIO2) is used during boot and may not work reliably for critical inputs like the main roll button. Use GPIO20 (D7) or other non-strapping pins for primary inputs.
16. **Windows-specific: Never use `2>nul` in bash** - On Windows, using `2>nul` in Git Bash/WSL creates a file called `nul` which is a reserved name and extremely hard to remove. Always use `2>/dev/null` for stderr redirection in bash.
17. **Refactor early while code is fresh** - Did Step 1 refactoring (Config, ButtonHandler, GameState) at 283 lines, resulting in clean 53-line main file. Waiting longer would have made refactoring harder and riskier. The "one refactor ahead" approach (refactor before adding complex features) provides good foundation without over-engineering.

## Project Status

### Development Phase
✅ **COMPLETE** - Fully functional Digital Multi-Dice on Arduino Nano ESP32 with:
- Round GC9A01 display working
- 4 button inputs (Roll, Mode, Qty+/-)
- Multiple dice types (D4, D6, D8, D10, D12, D20, D100)
- Advantage/Disadvantage rolling
- Multi-dice rolling (1-4 dice)
- Special effects for critical rolls
- Clean, centered display with D&D aesthetic

### Production Phase
✅ **COMPLETE** - Migration to XIAO ESP32-C3:
1. ✅ Code ready (using Adafruit_GC9A01A library)
2. ✅ Pin configuration documented
3. ✅ Soldered pin headers to XIAO board
4. ✅ Updated code for XIAO pin assignments (GPIO numbers, not D-numbers)
5. ✅ All 4 buttons mapped to available GPIOs
6. ✅ **Hardware VERIFIED and WORKING** - Hardware SPI @ 40MHz
7. ✅ **Step 1 Refactoring COMPLETE** - Modular code structure (Config, ButtonHandler, GameState)
8. ⏳ Design/create enclosure for portable use
9. ⏳ Add battery for truly portable operation

**Current**: Fully functional XIAO ESP32-C3 with modular codebase ready for feature expansion!
**Next**: Choose next feature (statistics, tilt sensor, sleep mode) or enclosure design.

---

## Code Organization Guidelines

### Current Modular Structure (Step 1 Refactoring - Completed 2026-01-25)

The D20 project uses a clean modular architecture for maintainability and future expansion:

```
D20/
├─ D20.ino (53 lines)          - Main coordination only
├─ Config.h (67 lines)         - Hardware configuration & constants
├─ ButtonHandler.h/cpp (97)    - Button input handling
├─ GameState.h/cpp (147)       - Game logic & state management
├─ Display.h/cpp (285)         - Display rendering
└─ DiceTypes.h/cpp (44)        - Type definitions & utilities
```

#### **Module Responsibilities:**

1. **D20.ino** - Main coordination (83% reduction from 283 lines!)
   - Arduino lifecycle functions (`setup()`, `loop()`)
   - Display object instantiation
   - Subsystem initialization
   - High-level coordination

2. **Config.h** - Hardware configuration
   - Pin definitions (with GPIO number mappings)
   - All constants (debounce, SPI speed, max dice quantity)
   - Complete wiring reference documentation
   - Easy hardware porting

3. **ButtonHandler.h/cpp** - Input handling
   - Button state tracking
   - Debouncing logic
   - Long-press detection
   - Single `updateButtons()` function
   - Easy to swap for tilt sensor later

4. **GameState.h/cpp** - Game logic
   - Game state variables (diceType, quantity, rollMode, etc.)
   - All game logic functions (roll, change dice, toggle mode)
   - Random number generation
   - Ready for statistics/history features

5. **Display.h/cpp** - Display rendering
   - 5+ display-related functions (`initDisplay`, `drawWelcomeScreen`, `animatedRoll`, etc.)
   - Clear separation of concerns (display vs game logic)
   - Could be reused in other projects with similar displays

6. **DiceTypes.h/cpp** - Type definitions
   - Centralized type definitions (`DiceType`, `RollMode` enums)
   - 3 utility functions (`getDiceName`, `getDiceMax`, `getDiceColor`)
   - Color constants
   - Pure helper functions with no side effects

#### **Benefits of This Structure:**
- ✅ **Maintainability**: Related code grouped together
- ✅ **Testability**: Each module can be tested in isolation
- ✅ **Reusability**: Display, ButtonHandler, GameState can be used in other projects
- ✅ **Readability**: Main file is now just 53 lines of coordination code
- ✅ **Extensibility**: Easy to add features (stats → GameState, tilt → ButtonHandler)
- ✅ **Hardware portability**: Change Config.h to port to different boards

#### **Future Expansion Ready:**
- **Add tilt sensor?** → Modify ButtonHandler.cpp
- **Add statistics?** → Extend GameState.cpp
- **Add sleep mode?** → Add to Config.h and main.ino
- **Add bluetooth?** → Create new module, GameState is isolated and ready

#### **File Naming Convention:**
- Use descriptive names: `Display.h`, `GameState.h` (not `utils.h`, `helpers.h`)
- Match header and implementation files: `Display.h` ↔ `Display.cpp`
- Use PascalCase for module names

---

## Final Working Configuration Summary (2026-01-25)

### ✅ Hardware: XIAO ESP32-C3 (CONFIRMED WORKING)

**Display Wiring (GC9A01 1.28" Round):**
```
VCC → 3V3
GND → GND
SCL → D8  (GPIO8 - Hardware SPI Clock)
SDA → D10 (GPIO10 - Hardware SPI MOSI)
RES → D1  (GPIO3 - Reset)
DC  → D2  (GPIO4 - Data/Command)
CS  → D3  (GPIO5 - Chip Select)
BLK → 3V3 (Backlight)
```

**Button Wiring:**
```
Roll Button (Primary)    → D7 + GND (GPIO20)
Mode Button (Long press) → D6 + GND (GPIO21)
Qty Plus Button          → D4 + GND (GPIO6)
Qty Minus Button         → D5 + GND (GPIO7)
```

**Unused Pins:**
- D0 (GPIO2) - Strapping pin, avoided
- D9 (GPIO9) - Hardware SPI MISO, kept free

### ✅ Software Configuration

**Pin Definitions (use GPIO numbers in code):**
```cpp
#define TFT_CS    5   // D3 = GPIO5
#define TFT_DC    4   // D2 = GPIO4
#define TFT_RST   3   // D1 = GPIO3
// Hardware SPI auto-uses GPIO8 (D8) and GPIO10 (D10)

#define ROLL_BUTTON_PIN  20  // D7 = GPIO20
#define MODE_BUTTON_PIN  21  // D6 = GPIO21
#define QTY_PLUS_PIN     6   // D4 = GPIO6
#define QTY_MINUS_PIN    7   // D5 = GPIO7
```

**Display Initialization:**
```cpp
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);  // 3-param = hardware SPI
tft.begin(40000000);  // Hardware SPI @ 40MHz
```

### ✅ Features Working
- Multiple dice types (D4/D6/D8/D10/D12/D20/D100)
- Multi-dice rolling (1-4 dice)
- Advantage/Disadvantage mode
- Critical hit/fail colors
- Hardware SPI @ 40MHz
- All 4 buttons functional

### 🎯 Next Steps
1. Battery integration (LiPo + charging circuit)
2. Enclosure design and 3D printing
3. Optional: Tilt sensor for shake-to-roll
4. Power management (sleep mode, battery indicator)

---
*Last Updated: 2026-01-25*
*Claude Code Development Session*
