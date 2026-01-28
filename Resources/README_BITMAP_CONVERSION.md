# Dice Bitmap Conversion

This directory contains the SVG dice images and the script to convert them to Arduino bitmaps.

## Files

- **PolyhedralDiceSet/** - SVG dice images (D4, D6, D8, D10, D12, D20)
- **convert_svg_to_bitmap.js** - Node.js script to convert SVGs to C bitmap arrays

## How to Use

### Prerequisites

From the project root:

```bash
npm install
```

This installs `jimp` and `sharp` dependencies.

### Converting SVGs to Bitmaps

From the project root, run:

```bash
npm run convert-bitmaps
```

Or directly:

```bash
node Resources/convert_svg_to_bitmap.js
```

3. This will generate:
   - Debug PNG files (`*_debug.png`) in each dice subdirectory
   - `D:\repos\D20\D20\DiceBitmaps.h` with C bitmap arrays

### Adjusting Bitmap Size

Edit the `BITMAP_SIZE` constant in `convert_svg_to_bitmap.js`:

```javascript
const BITMAP_SIZE = 150;  // Change this value (e.g., 100, 150, 200)
```

Current size: **150x150 pixels**

### How It Works

1. **SVG → PNG**: First converts SVG to PNG using Sharp library
2. **PNG → Bitmap**: Reads PNG with Jimp and extracts pixel data
3. **Alpha Channel**: Transparent pixels (background) are not drawn
4. **Monochrome**: Black pixels (dice lines) become 1-bits in the bitmap array
5. **PROGMEM**: Bitmaps are stored in Arduino flash memory to save RAM

### Troubleshooting

- **All pixels black**: Alpha channel issue - the script handles transparent backgrounds correctly
- **Filled squares**: Bitmap inversion issue - black pixels (dice) should be 1, white (background) should be 0
- **Dice too small**: Increase `BITMAP_SIZE` and regenerate

## Output Format

The script generates `DiceBitmaps.h` with:

```cpp
const unsigned char bitmap_DICE_D20[] PROGMEM = { ... };
const int bitmap_DICE_D20_width = 150;
const int bitmap_DICE_D20_height = 150;
```

These arrays are used by `Display.cpp` with `tft.drawBitmap()`.
