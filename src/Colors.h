#ifndef COLORS_H
#define COLORS_H

#include "BufferedNeoPixelBus.h"

#define DEFAULT_COLORS                                                                                                                                         \
  { "#9400D3", "#0000FF", "#00FFFF", "#00FF00", "#FFFF00", "#FF7F00", "#FF0000" }
#define DEFAULT_COLORS_COUNT 7

const RgbColor BLACK(0);
const RgbColor YELLOW(255, 255, 0);
const RgbColor GREEN(0, 255, 0);
const RgbColor BLUE(0, 0, 255);

inline RgbColor changeColorBrightness(RgbColor color, float brightness) { return RgbColor::LinearBlend(color, BLACK, 1 - brightness); }

#endif
