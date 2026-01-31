/*
 * SDAnimations.cpp - SD card animation playback
 *
 * Uses the D20Display subclass (see Display.h) to access setAddrWindow
 * publicly for efficient bulk writePixels() streaming.
 * Each frame is streamed in 512-pixel (1 KB) chunks to keep RAM usage low.
 *
 * The display and SD card share the SPI bus (SCK + MOSI).  They must not
 * be active simultaneously.  The SD read and display write in displayFrame()
 * are strictly sequential: read chunk into RAM, then write chunk to display.
 * The display's startWrite/endWrite block (which holds TFT_CS LOW) wraps
 * only the write side; SD.open/read happen before startWrite.
 * For this reason we buffer the entire frame first, then push it.
 */

#include "SDAnimations.h"
#include "Config.h"
#include "Display.h"
#include <SD.h>
#include <SPI.h>
#include <Arduino.h>

extern D20Display tft;

static bool sdMounted  = false;
static int  frameCount = 0;

// Full-frame buffer — 240×240×2 = 115,200 bytes (~113 KB)
// ESP32-C3 has 400 KB SRAM; this is the largest single allocation.
static uint16_t frameBuffer[ANIM_FRAME_WIDTH * ANIM_FRAME_HEIGHT];

// Count frames by checking for sequentially numbered .bin files
static int countFrames() {
  int count = 0;
  while (true) {
    char path[32];
    snprintf(path, sizeof(path), "%s/%03d.bin", ANIM_DIR, count + 1);
    if (SD.exists(path)) {
      count++;
    } else {
      break;
    }
  }
  return count;
}

bool initSDAnimations() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card not found - animations disabled");
    sdMounted = false;
    return false;
  }

  sdMounted = true;
  Serial.println("SD card mounted");

  frameCount = countFrames();
  if (frameCount > 0) {
    Serial.print("Found ");
    Serial.print(frameCount);
    Serial.println(" animation frames in " ANIM_DIR);
  } else {
    Serial.println("No animation frames found in " ANIM_DIR);
  }

  return sdMounted && (frameCount > 0);
}

// Read an entire frame from SD into the buffer, then push to display.
// SD read completes before the display SPI transaction begins.
static bool displayFrame(const char* path) {
  File f = SD.open(path, FILE_READ);
  if (!f) return false;

  if ((long)f.size() < ANIM_FRAME_SIZE) {
    f.close();
    return false;
  }

  // Read entire frame while SD CS is active (TFT CS is HIGH)
  f.read((uint8_t*)frameBuffer, ANIM_FRAME_SIZE);
  f.close();

  // Now push the buffer to the display (TFT CS goes LOW, SD CS is HIGH)
  const int totalPixels = ANIM_FRAME_WIDTH * ANIM_FRAME_HEIGHT;
  tft.startWrite();
  tft.setWindow(0, 0, ANIM_FRAME_WIDTH - 1, ANIM_FRAME_HEIGHT - 1);
  tft.writePixels(frameBuffer, totalPixels);
  tft.endWrite();

  return true;
}

bool playRollAnimation() {
  if (!sdMounted || frameCount == 0) return false;

  unsigned long frameDelay = 1000 / ANIM_DEFAULT_FPS;

  for (int i = 1; i <= frameCount; i++) {
    char path[32];
    snprintf(path, sizeof(path), "%s/%03d.bin", ANIM_DIR, i);

    unsigned long frameStart = millis();

    if (!displayFrame(path)) {
      Serial.print("Failed to display frame: ");
      Serial.println(path);
      return false;
    }

    // Pace frames to target FPS, accounting for render time
    unsigned long elapsed = millis() - frameStart;
    if (elapsed < frameDelay) {
      delay(frameDelay - elapsed);
    }
  }

  return true;
}

bool isSDAvailable() {
  return sdMounted && (frameCount > 0);
}
