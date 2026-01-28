#!/usr/bin/env node
/**
 * Convert SVG dice images to C bitmap arrays for Arduino/Adafruit GFX
 * Using jimp for more reliable pixel access
 */

const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');
const { promisify } = require('util');
const execAsync = promisify(exec);

let Jimp;
try {
  const jimpModule = require('jimp');
  Jimp = jimpModule.default || jimpModule.Jimp || jimpModule;
} catch (e) {
  console.log('Installing jimp...');
  require('child_process').execSync('npm install jimp', { stdio: 'inherit' });
  const jimpModule = require('jimp');
  Jimp = jimpModule.default || jimpModule.Jimp || jimpModule;
}

// Configuration
const SVG_DIR = path.join(__dirname, 'PolyhedralDiceSet');
const OUTPUT_FILE = path.join(__dirname, '..', 'D20', 'DiceBitmaps.h');
const BITMAP_SIZE = 220;  // Large screen-filling (240x240 display)

const DICE_FILES = {
  'DICE_D4': 'D4/D4.svg',
  'DICE_D6': 'D6/D6.svg',
  'DICE_D8': 'D8/D8.svg',
  'DICE_D10': 'D10/D10.svg',
  'DICE_D12': 'D12/D12.svg',
  'DICE_D20': 'D20/D20.svg',
};

async function svgToBitmapArray(svgPath, size) {
  console.log(`  Processing ${svgPath}...`);

  // Convert SVG to PNG using the previously saved debug file
  const pngPath = svgPath.replace('.svg', '_debug.png');

  if (!fs.existsSync(pngPath)) {
    console.error(`    ERROR: ${pngPath} not found!`);
    return { bitmapBytes: [], width: 0, height: 0 };
  }

  // Read PNG with Jimp
  const image = await Jimp.read(pngPath);
  const width = image.bitmap.width;
  const height = image.bitmap.height;

  console.log(`    Image size: ${width}x${height}`);

  // Convert to bitmap bytes
  const bitmapBytes = [];
  let blackCount = 0;
  let whiteCount = 0;

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x += 8) {
      let byte = 0;
      for (let bit = 0; bit < 8; bit++) {
        if (x + bit < width) {
          // Access pixel directly from bitmap data (RGBA format)
          const idx = (y * width + (x + bit)) * 4;
          const r = image.bitmap.data[idx];
          const g = image.bitmap.data[idx + 1];
          const b = image.bitmap.data[idx + 2];
          const a = image.bitmap.data[idx + 3];

          // Check alpha first - transparent pixels (A=0) are background
          if (a < 128) {
            // Transparent = background = white = don't draw
            whiteCount++;
          } else {
            // Opaque pixel - check if black or white
            const gray = (r + g + b) / 3;
            if (gray < 128) {  // Black opaque pixel = dice lines
              byte |= (1 << (7 - bit));
              blackCount++;
            } else {  // White opaque pixel
              whiteCount++;
            }
          }
        }
      }
      bitmapBytes.push(byte);
    }
  }

  const dataBytes = bitmapBytes.filter(b => b !== 0).length;
  const coverage = ((blackCount / (width * height)) * 100).toFixed(1);
  console.log(`    ${blackCount} dice pixels (${coverage}% coverage), ${dataBytes}/${bitmapBytes.length} bytes used`);

  return { bitmapBytes, width, height };
}

async function generateHeaderFile() {
  console.log('Converting SVG dice to C bitmap arrays...\n');

  const output = [];
  output.push('/*');
  output.push(' * DiceBitmaps.h - Bitmap data for dice shapes');
  output.push(' * Auto-generated from SVG files');
  output.push(' */');
  output.push('');
  output.push('#ifndef DICE_BITMAPS_H');
  output.push('#define DICE_BITMAPS_H');
  output.push('');
  output.push('#include <Arduino.h>');
  output.push('');

  for (const [diceName, svgFile] of Object.entries(DICE_FILES)) {
    const svgPath = path.join(SVG_DIR, svgFile);

    if (!fs.existsSync(svgPath)) {
      console.log(`WARNING: ${svgPath} not found, skipping`);
      continue;
    }

    console.log(`Converting ${diceName}...`);

    try {
      const { bitmapBytes, width, height } = await svgToBitmapArray(svgPath, BITMAP_SIZE);

      if (bitmapBytes.length === 0) continue;

      // Generate C array
      output.push(`// ${diceName} - ${width}x${height} pixels`);
      output.push(`const unsigned char bitmap_${diceName}[] PROGMEM = {`);

      // Format as hex bytes, 12 per line
      for (let i = 0; i < bitmapBytes.length; i += 12) {
        const line = bitmapBytes.slice(i, i + 12);
        const hexValues = line.map(b => `0x${b.toString(16).padStart(2, '0').toUpperCase()}`).join(', ');
        output.push(`  ${hexValues},`);
      }

      output.push('};');
      output.push(`const int bitmap_${diceName}_width = ${width};`);
      output.push(`const int bitmap_${diceName}_height = ${height};`);
      output.push('');
    } catch (error) {
      console.error(`ERROR processing ${diceName}:`, error.message);
    }
  }

  output.push('#endif');

  // Write to file
  fs.writeFileSync(OUTPUT_FILE, output.join('\n'));

  console.log(`\n✅ Generated: ${OUTPUT_FILE}`);
}

generateHeaderFile().catch(console.error);
