/*
 * SDAnimations.cpp - SD card animation playback (non-blocking)
 *
 * Uses the D20Display subclass (see Display.h) to access setAddrWindow
 * publicly for efficient bulk writePixels() streaming.
 *
 * The display and SD card share the SPI bus (SCK + MOSI).  They must not
 * be active simultaneously.  The SD read and display write in displayFrame()
 * are strictly sequential: read chunk into RAM, then write chunk to display.
 * For this reason we buffer the entire frame first, then push it.
 *
 * Playback is non-blocking: startRollAnimation() begins playback,
 * updateSDAnimation() advances one frame per loop() call, and
 * isAnimationPlaying() reports status.  stopAnimation() aborts early
 * (e.g. button skip).
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

// Non-blocking playback state
static bool          animPlaying   = false;
static int           currentFrame  = 0;
static unsigned long nextFrameTime = 0;

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
  SPI.begin();  // Ensure MISO is configured — display init doesn't use it
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

void startRollAnimation() {
  if (!sdMounted || frameCount == 0) return;
  animPlaying   = true;
  currentFrame  = 0;
  nextFrameTime = millis();  // First frame on next update
}

void updateSDAnimation() {
  if (!animPlaying) return;

  unsigned long now = millis();
  if (now < nextFrameTime) return;  // Not time for next frame yet

  currentFrame++;
  if (currentFrame > frameCount) {
    animPlaying = false;  // Finished — GameState will pick this up
    return;
  }

  char path[32];
  snprintf(path, sizeof(path), "%s/%03d.bin", ANIM_DIR, currentFrame);

  if (!displayFrame(path)) {
    Serial.print("Failed to display frame: ");
    Serial.println(path);
    animPlaying = false;
    return;
  }

  nextFrameTime = now + (1000 / ANIM_DEFAULT_FPS);
}

bool isAnimationPlaying() {
  return animPlaying;
}

void stopAnimation() {
  animPlaying = false;
}

bool isSDAvailable() {
  return sdMounted && (frameCount > 0);
}
