#ifndef LED_STRIP_MODE_H
#define LED_STRIP_MODE_H

#include "BufferedNeoPixelBus.h"
#include "Colors.h"
#include "JsonEntity.h"
#include <Arduino.h>

#define JSON_FIELD_MODE_INDEX "index"
#define JSON_FIELD_MODE_DESCRIPTION "description"
#define JSON_FIELD_MODE_COLORS "colors"
#define JSON_FIELD_MODE_COLOR_SELECTION_MODE "colorSelectionMode"
#define JSON_FIELD_MODE_ANIMATION_MODE "animationMode"
#define JSON_FIELD_MODE_ANIMATION_SPEED "animationSpeed"
#define JSON_FIELD_MODE_ANIMATION_PROGRESS_MODE "animationProgressMode"
#define JSON_FIELD_MODE_ANIMATION_INTENSITY "animationIntensity"
#define JSON_FIELD_MODE_ANIMATION_DIRECTION "animationDirection"

#define MODE_DESCRIPTION_SIZE 32
#define MODE_DESCRIPTION_DEFAULT "Default mode"
#define MODE_INDEX_MAX 15
#define MODE_COLORS_SIZE 32

class LedStripMode : public JsonEntity {
public:
  uint16_t index = 0;
  char description[MODE_DESCRIPTION_SIZE] = MODE_DESCRIPTION_DEFAULT;
  uint16_t colorsCount = 0;
  uint16_t colorSelectionMode = 0;
  uint16_t animationMode = 0;
  uint16_t animationSpeed = 128;
  uint16_t animationProgressMode = 0;
  uint16_t animationIntensity = 1;
  bool animationDirection = true;
  RgbColor colors[MODE_COLORS_SIZE];

  bool updateEntityFromJson(JsonObject &json, ErrorCallbackFunctionType errorCallback);
  bool updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback);
};

#include "../animation/Animation.h"

#endif
