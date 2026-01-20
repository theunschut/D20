# D20 Digital Dice - Future Features

This document tracks potential features to add to the digital dice roller project.

---

## 🔥 High Priority (Easy + High Impact)

### 1. Roll Animation Variations
- Different animation speeds based on dice type (D100 spins longer than D4)
- "Dramatic roll" mode for important rolls (hold button longer = more suspense)
- Shake intensity affects animation (if using tilt sensor)

### 2. Roll Statistics Tracking
- Track last 10-20 rolls in memory
- Show average, highest, lowest on long-press
- "Hot/Cold" dice indicator (are you rolling well today?)
- Reset stats option

### 3. Quick Roll Memory
- Press button again immediately to re-roll same dice
- Useful for repeated checks (skill checks, attacks)
- Shows "REROLL" indicator

### 4. Dice Presets
- Save favorite configurations (2D20+ADV, 3D6, etc.)
- Quick-switch between presets
- Could use button combinations

### 5. Modifier Support ⭐ (Very D&D specific)
- Add/subtract modifiers (+5, -2, etc.)
- Show both raw roll and modified total
- Use Qty buttons to adjust modifier
- Display: "18 (+5) = 23"

---

## 🎨 Medium Priority (Visual Polish)

### 6. Themed Color Schemes
- Different color palettes (Fire, Ice, Shadow, etc.)
- Cycle through themes with button combo
- Save preference to EEPROM

### 7. Critical/Fail Animations
- Special effect when rolling nat 20 (flash, pulse border)
- Special effect for nat 1 (shake, red flash)
- More dramatic than current color change

### 8. Battery Indicator
- Show battery % when switching on
- Low battery warning
- Dim display to save power when idle

---

## ⚡ Advanced Features (More Complex)

### 9. Tilt Sensor Integration ⭐ (Planned)
- Replace roll button with SW-520D ball tilt sensor
- Shake to roll (more natural!)
- Different shake patterns = different actions
- Requires debouncing and shake detection algorithm

### 10. Sleep Mode
- Auto-sleep after 2-5 minutes idle
- Wake on shake/button press
- Saves battery significantly

### 11. Sound Effects (Requires buzzer)
- Rolling sound (subtle beep/click)
- Different sounds for crit/fail
- Mute toggle
- Adds ~$0.50 in parts (piezo buzzer)

### 12. Bluetooth Dice Logger (Advanced)
- Log rolls to phone app
- Share results with DM/party
- ESP32 has built-in Bluetooth
- Requires companion app development

---

## 🎲 D&D Specific Features

### 13. Initiative Tracker Mode
- Roll for initiative and store it
- Show your initiative number persistently
- Useful at start of combat

### 14. Death Save Mode
- Track successes/failures on D20 rolls
- Visual counter (3 success / 3 fail)
- Auto-reset after rest

### 15. Inspiration Token
- Visual reminder if you have inspiration
- Toggle on/off with button combo

### 16. Common D&D Roll Shortcuts
- "Attack Roll" preset (1D20 + modifier + ADV option)
- "Damage Roll" mode (switch quickly between different damage dice)
- "Saving Throw" mode

---

## 🔧 Quality of Life

### 17. Roll History Scroll
- View last 5-10 rolls on screen
- Scroll through with buttons
- Useful when you forget your previous roll

### 18. Dice Lock Mode
- Prevent accidental rolls
- Lock/unlock with button combo
- Useful in bag/pocket

### 19. Brightness Control
- Adjust backlight brightness
- Save preference
- Helps battery life

### 20. Custom Dice
- Define custom dice (D3, D7, D30, etc.)
- Useful for special game mechanics

---

## 🥇 Top 5 Recommended Features

Based on current progress and hardware:

### 1. **Modifier Support** 🥇
- Essential for D&D
- Uses existing buttons (Qty +/- could add/subtract modifier)
- High value, medium effort
- **Estimated Time**: 2-3 hours

### 2. **Tilt Sensor Integration** 🥈
- Already mentioned in original requirements
- Makes it feel like a real dice
- Medium-high effort, but very cool
- **Estimated Time**: 4-6 hours
- **Hardware Needed**: SW-520D ball tilt sensor (~$0.30)

### 3. **Roll Statistics** 🥉
- Track your luck!
- Good for debugging weighted rolls
- Low effort, fun payoff
- **Estimated Time**: 2-3 hours

### 4. **Sleep Mode** ⚡
- Critical for battery operation
- Moderate effort
- Extends battery life 10x
- **Estimated Time**: 2-3 hours

### 5. **Critical/Fail Animations** ✨
- Makes nat 20s feel AMAZING
- Low effort (just enhance existing code)
- High satisfaction
- **Estimated Time**: 1-2 hours

---

## 📋 Suggested Implementation Order

### Phase 1 (Next Session - Before XIAO Migration)
- [ ] Roll statistics tracking
- [ ] Enhanced crit/fail animations
- [ ] Modifier support

**Rationale**: These features can be developed and tested on Arduino Nano ESP32 before migrating to XIAO.

### Phase 2 (After XIAO Migration & Soldering)
- [ ] Tilt sensor integration
- [ ] Sleep mode
- [ ] Battery indicator

**Rationale**: These features require the final hardware (XIAO with battery support).

### Phase 3 (Polish & Optional)
- [ ] Themed color schemes
- [ ] Brightness control
- [ ] Roll history scroll
- [ ] Dice lock mode

**Rationale**: Nice-to-have features that enhance user experience.

### Phase 4 (Advanced/Future)
- [ ] Sound effects (requires piezo buzzer)
- [ ] Bluetooth logging (requires app development)
- [ ] D&D specific modes (initiative tracker, death saves)

**Rationale**: More complex features that require additional hardware or development.

---

## 💡 Feature Ideas from User

Add your own feature ideas here:

- [ ] _Your idea here..._

---

## ✅ Completed Features

### Current Implementation (v1.0)
- [x] Multiple dice types (D4, D6, D8, D10, D12, D20, D100)
- [x] Multi-dice rolls (1-4 dice at once)
- [x] Advantage/Disadvantage mode (D&D 5e)
- [x] Fast roll animation with hardware SPI @ 40MHz
- [x] Visual feedback for critical rolls (nat 20 = yellow, nat 1 = red)
- [x] Individual roll breakdown for multi-dice
- [x] Last roll number display
- [x] Color-coded dice types
- [x] Dual-circle border design
- [x] 4-button control system
- [x] Long-press for mode switching

---

*Last Updated: 2026-01-20*
*Based on current hardware: Arduino Nano ESP32 (dev) / XIAO ESP32-C3 (target)*
