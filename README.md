# Digital D20 Dice Roller

A digital multi-dice roller for D&D using Arduino ESP32 and a round GC9A01 display.

## Features

- **Multiple Dice Types**: D4, D6, D8, D10, D12, D20, D100
- **Multi-Dice Rolls**: Roll 1-4 dice at once (e.g., 2D20, 3D6)
- **Advantage/Disadvantage**: D&D 5e mechanics - roll twice, keep higher/lower
- **Fast Animation**: Hardware SPI @ 40MHz for smooth roll animations
- **Visual Feedback**:
  - Color-coded dice types
  - Yellow highlight for max rolls (natural 20, etc.)
  - Red highlight for natural 1s
  - Individual roll breakdown with colors
- **Roll History**: Shows previous roll result

## Hardware

### Current Development Setup (Arduino Nano ESP32)
- **Board**: Arduino Nano ESP32
- **Display**: 1.28" Round TFT LCD (240x240, GC9A01 driver)
- **Buttons**: 4 push buttons

### Target Hardware (Final Product)
- **Board**: XIAO ESP32-C3 (compact, battery-capable)
- **Display**: Same GC9A01 display
- **Sensor**: Ball tilt sensor for roll trigger (currently using button for testing)

## Wiring

### Arduino Nano ESP32 (Development)

**Display:**
- VCC → 3.3V
- GND → GND
- SCL → D13 (Hardware SPI SCK)
- SDA → D11 (Hardware SPI MOSI)
- RES → D7 (Reset)
- DC → D8 (Data/Command)
- CS → D9 (Chip Select)
- BLK → 3.3V (Backlight)

**Buttons:**
- Roll Button → D2 and GND
- Mode Button → D3 and GND
- Quantity + → D4 and GND
- Quantity - → D5 and GND

### XIAO ESP32-C3 (Target)
See `claude.md` for detailed XIAO pin configuration and notes.

## Controls

- **Roll Button**: Roll the dice
- **Mode Button** (short press): Change dice type
- **Mode Button** (long press, 1s): Toggle Advantage/Disadvantage/Normal
- **Qty +**: Increase number of dice (1-4)
- **Qty -**: Decrease number of dice (1-4)

## Software Requirements

### Arduino Libraries
- Adafruit GFX Library
- Adafruit GC9A01A

### Installation
1. Install Arduino IDE
2. Add ESP32 board support via Board Manager
3. Install required libraries via Library Manager
4. Select board: "Arduino Nano ESP32" (or "XIAO ESP32-C3" for final)
5. Upload using: `Sketch > Upload Using Programmer`

## Code Structure

- `D20.ino` - Main program with button handling and game logic
- `Display.h/cpp` - Display rendering functions
- `DiceTypes.h/cpp` - Dice type definitions and utilities
- `claude.md` - Development notes and troubleshooting guide

## Performance Notes

- **Hardware SPI is critical**: 10-20x faster than software SPI
- Display initialization @ 40MHz: `tft.begin(40000000)`
- Use `fillRect()` for partial clears instead of full `fillScreen()`
- XIAO pins **must be soldered** for reliable SPI communication

## Known Issues

- XIAO ESP32-C3 requires soldered pin headers (un-soldered pins cause intermittent SPI failures)
- Last image briefly visible on reboot (~0.5s) - acceptable with hardware SPI speed
- Upload to Nano ESP32 requires "Upload Using Programmer" to avoid DFU errors

## Future Enhancements

- Replace roll button with tilt sensor
- Physical enclosure with 4 corner buttons around display
- Sleep mode for battery saving
- Battery level indicator
- Sound effects
- Modifier support (+5, etc.)

## License

MIT

## Documentation

See `claude.md` for detailed development notes, troubleshooting, and lessons learned.
