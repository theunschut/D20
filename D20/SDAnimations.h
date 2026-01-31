/*
 * SDAnimations.h - SD card animation playback (non-blocking)
 *
 * Streams pre-rendered RGB565 frames from a MicroSD card to the display.
 * Playback is driven frame-by-frame from loop() — buttons and motion
 * detection remain responsive during animations.
 * Falls back gracefully if the SD card is missing or has no frames.
 *
 * Frame format (configured in Config.h):
 *   Raw RGB565 binary, one file per frame
 *   240 x 240 x 2 bytes = 115,200 bytes per frame
 *   Directory: /roll/
 *   Files: 001.bin, 002.bin, 003.bin, ...
 *
 * Generating frames:
 *   Convert each animation frame to 240x240 RGB565 little-endian binary.
 *   Example with Python / Pillow:
 *     img = Image.open("frame.png").resize((240,240))
 *     pixels = list(img.getdata())
 *     with open("001.bin","wb") as f:
 *       for r,g,b in pixels:
 *         rgb565 = ((r>>3)<<11)|((g>>2)<<5)|(b>>3)
 *         f.write(rgb565.to_bytes(2,"little"))
 */

#ifndef SD_ANIMATIONS_H
#define SD_ANIMATIONS_H

// Initialize SD card. Returns true if mounted and frames are available.
bool initSDAnimations();

// Start non-blocking roll animation playback
void startRollAnimation();

// Call every loop — advances to next frame when due. No-op if not playing.
void updateSDAnimation();

// True while animation frames are being displayed
bool isAnimationPlaying();

// Stop playback early (e.g. button skip)
void stopAnimation();

// Returns true if the SD card is mounted and animation frames exist.
bool isSDAvailable();

#endif
