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

#define DESCRIPTION_SIZE 32
#define DESCRIPTION_DEFAULT "Default mode"

#define COLOR_SELECTION_MODE_ASC 0
#define COLOR_SELECTION_MODE_RAND 1
#define COLOR_SELECTION_MODE_GENERATED 2

#define COLOR_SELECTION_MODE_COUNT 3

class LedStripMode : public JsonEntity {
public:
  uint16_t index = 0;
  char description[DESCRIPTION_SIZE] = DESCRIPTION_DEFAULT;
  uint16_t colorsCount = 0;
  uint16_t colorSelectionMode = 0;
  uint16_t animationMode = 0;
  uint16_t animationSpeed = 50;
  uint16_t animationProgressMode = 0;
  uint16_t animationIntensity = 1;
  bool animationDirection = true;
  RgbColor colors[32];

  bool updateEntityFromJson(JsonObject &json, ErrorCallbackFunctionType errorCallback);
  bool updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback);
};

#include "../animation/Animation.h"

#endif
