# Digital D20 Dice Project - Development Notes

## Project Overview
A digital D20 dice roller using an ESP32 microcontroller and a round GC9A01 display, designed for tabletop RPG gaming (D&D, etc.).

## Final Working Configuration

### Target Hardware (Production)
- **Microcontroller**: Seeed Studio XIAO ESP32-C3
- **Display**: 1.28" Round TFT LCD, 240x240 pixels, GC9A01 driver
- **Input**: Push button
- **Power**: USB (3.3V) or battery

### Development/Testing Hardware (Current)
- **Microcontroller**: Arduino Nano ESP32 (ESP32-S3)
- **Display**: 1.28" Round TFT LCD, 240x240 pixels, GC9A01 driver
- **Input**: Push button
- **Power**: USB (3.3V)

**Note**: Arduino Nano ESP32 is being used for development because it has pre-soldered headers. The final product will use XIAO ESP32-C3 due to its smaller form factor, making it ideal for a portable dice roller.

### Wiring Diagrams

#### XIAO ESP32-C3 (Final/Production Build)
**GC9A01 Display to XIAO ESP32-C3:**
- VCC → 3V3
- GND → GND
- SCL → D8 (GPIO6 - SPI SCLK)
- SDA → D10 (GPIO7 - SPI MOSI)
- RES → D1 (GPIO3 - Reset)
- DC → D2 (GPIO4 - Data/Command)
- CS → D3 (GPIO5 - Chip Select)
- BLK → 3V3 (Backlight)

**Button:**
- One side → D0 (GPIO2)
- Other side → GND

**Important Notes for XIAO:**
- D9 (GPIO21) is the default SPI MISO pin - avoid using it for button to prevent SPI conflicts
- Button moved to D0 (GPIO2) to keep D9 free for SPI hardware functionality
- **Pin headers MUST be soldered** - un-soldered pins cause unreliable SPI communication

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
**Status**: ⚠️ Requires soldered headers (not yet tested with soldered pins)
- **Target board for final product** due to small form factor (21mm x 17.8mm)
- Currently has un-soldered pin headers causing unreliable connections
- SPI communication is extremely sensitive to poor connections - even slight movement causes failures
- Software SPI initialization causes memory access faults with loose connections
- **Critical Requirement**: Pin headers MUST be soldered for reliable SPI operation
- Serial monitor shows "Display init SUCCESS" but no visuals due to poor connections during high-speed pixel data transfer
- Initial slow commands succeed, but fast pixel data transfer fails intermittently

**Why XIAO is ideal for final product:**
1. **Size**: Tiny footprint (21mm x 17.8mm) perfect for handheld dice
2. **Power**: Built-in battery charging circuit for LiPo batteries
3. **Cost**: Lower cost than Arduino Nano ESP32
4. **Integration**: Pairs perfectly with round 1.28" GC9A01 display for compact design
5. **GPIO**: Has sufficient pins for display + button

**Expected pin configuration** (once soldered, needs verification):
- Similar to current working Nano configuration
- CS → D3 (GPIO5), DC → D2 (GPIO4), RST → D1 (GPIO3)
- Hardware SPI pins: MOSI → D10 (GPIO7), SCLK → D8 (GPIO6)
- Button → D0 (GPIO2) to avoid D9/GPIO21 SPI MISO conflict

**Next Steps for XIAO:**
1. Solder pin headers to XIAO ESP32-C3
2. Test with exact same Adafruit_GC9A01A library code
3. Verify pin assignments work as expected
4. If issues persist, try pin combinations similar to Nano's working config
5. Document final working XIAO configuration

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
5. **Software SPI more reliable** than hardware SPI for these displays
6. **Community knowledge invaluable** - Forum posts saved hours of debugging
7. **Upload method matters** - "Upload Using Programmer" bypasses many ESP32 issues
8. **Soldered connections are CRITICAL for SPI** - Un-soldered pins work for slow I2C but fail for high-speed SPI
9. **Development board strategy** - Use a board with pre-soldered headers (like Nano) for faster development, then migrate to target board (XIAO) once code is proven
10. **SPI pin conflicts** - Avoid using hardware SPI MISO pin (D9/GPIO21 on XIAO) for other purposes like buttons
11. **"Display init SUCCESS" doesn't mean working** - Library can successfully initialize but still fail during pixel data transfer with poor connections

## Project Status

### Development Phase
✅ **COMPLETE** - Fully functional Digital D20 dice on Arduino Nano ESP32 with:
- Round GC9A01 display working
- Button input functional
- Special effects for critical rolls (natural 20 = yellow, natural 1 = red)
- Clean, centered display with D&D aesthetic

### Production Phase
⏳ **PENDING** - Migration to XIAO ESP32-C3 requires:
1. ✅ Code ready (can use same Adafruit_GC9A01A library)
2. ✅ Pin configuration documented
3. ⏳ Solder pin headers to XIAO board
4. ⏳ Test and verify display works with soldered connections
5. ⏳ Confirm final pin assignments (may need minor adjustments)
6. ⏳ Design/create enclosure for portable use
7. ⏳ Add battery for truly portable operation

**Target**: Compact, battery-powered, pocket-sized D20 dice using XIAO ESP32-C3 + round GC9A01 display.

---
*Last Updated: 2026-01-20*
*Claude Code Development Session*
