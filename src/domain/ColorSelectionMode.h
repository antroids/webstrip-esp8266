#ifndef DOMAIN_COLOR_SELECTION_MODE_H
#define DOMAIN_COLOR_SELECTION_MODE_H

#include "../BufferedNeoPixelBus.h"
#include "../Types.h"
#include "LedStripMode.h"
#include "StaticIndex.h"

#define GENERATE_RANDOM_COLOR (HsbColor(((float)random(360)) / 360, 1, 0.5))

class ColorSelectionMode : public StaticIndex<ColorSelectionMode, 8> {
public:
  ColorSelectionMode() : StaticIndex(this){};
  virtual RgbColor generateColor(uint16_t ledIndex, uint16_t pixelCount, LedStripMode *mode) { return BLACK; };
};

class AscPaletteColorSelectionMode : public ColorSelectionMode {
  RgbColor generateColor(uint16_t ledIndex, uint16_t pixelCount, LedStripMode *mode) { return mode->colors[ledIndex % mode->colorsCount]; }
};

class RandPaletteColorSelectionMode : public ColorSelectionMode {
  RgbColor generateColor(uint16_t ledIndex, uint16_t pixelCount, LedStripMode *mode) { return mode->colors[random(mode->colorsCount)]; }
};

class RandColorSelectionMode : public ColorSelectionMode {
  RgbColor generateColor(uint16_t ledIndex, uint16_t pixelCount, LedStripMode *mode) { return RgbColor(GENERATE_RANDOM_COLOR); }
};

class AscPaletteStretchColorSelectionMode : public ColorSelectionMode {
  RgbColor generateColor(uint16_t ledIndex, uint16_t pixelCount, LedStripMode *mode) {
    uint16_t repeatColorsCount = pixelCount / mode->colorsCount;
    uint16_t colorIndex = ledIndex / repeatColorsCount;
    return mode->colors[colorIndex < mode->colorsCount ? colorIndex : mode->colorsCount - 1];
  }
};

#endif
