#ifndef LED_STRIP_MODE_H
#define LED_STRIP_MODE_H

#include "Arduino.h"
#include "BufferedNeoPixelBus.h"
#include "Colors.h"

#define DESCRIPTION_SIZE 32
#define DESCRIPTION_DEFAULT "Default mode"
#define DEFAULT_MODE_INDEX 0

#define COLOR_SELECTION_MODE_ASC 0
#define COLOR_SELECTION_MODE_RAND 1
#define COLOR_SELECTION_MODE_GENERATED 2

#define COLOR_SELECTION_MODE_COUNT 3

struct LedStripMode {
  uint16_t index = DEFAULT_MODE_INDEX;
  char description[DESCRIPTION_SIZE] = DESCRIPTION_DEFAULT;
  uint16_t colorsCount = 0;
  uint16_t colorSelectionMode = 0;
  uint16_t animationMode = 0;
  uint16_t animationSpeed = 50;
  uint16_t animationProgressMode = 0;
  uint16_t animationIntensity = 1;
  bool animationDirection = true;
  RgbColor colors[32];
};

#endif
