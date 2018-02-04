#ifndef LED_STRIP_MODE_H
#define LED_STRIP_MODE_H

#include "../animation/Animation.h"
#include "BufferedNeoPixelBus.h"
#include "Colors.h"
#include "JsonEntity.h"
#include <Arduino.h>

#define JSON_FIELD_INDEX "index"
#define JSON_FIELD_DESCRIPTION "description"
#define JSON_FIELD_COLORS "colors"
#define JSON_FIELD_COLOR_SELECTION_MODE "colorSelectionMode"
#define JSON_FIELD_ANIMATION_MODE "animationMode"
#define JSON_FIELD_ANIMATION_SPEED "animationSpeed"
#define JSON_FIELD_ANIMATION_PROGRESS_MODE "animationProgressMode"
#define JSON_FIELD_ANIMATION_INTENSITY "animationIntensity"
#define JSON_FIELD_ANIMATION_DIRECTION "animationDirection"

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

  bool updateEntityFromJson(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
    if (json.containsKey(JSON_FIELD_DESCRIPTION)) {
      String description = json[JSON_FIELD_DESCRIPTION];
      description.toCharArray(this->description, DESCRIPTION_SIZE);
    }
    if (json.containsKey(JSON_FIELD_COLOR_SELECTION_MODE)) {
      if (!validateRange(json, JSON_FIELD_COLOR_SELECTION_MODE, 0, COLOR_SELECTION_MODE_COUNT - 1, errorCallback))
        return false;
      colorSelectionMode = json[JSON_FIELD_COLOR_SELECTION_MODE];
    }
    if (json.containsKey(JSON_FIELD_ANIMATION_MODE)) {
      if (!validateRange(json, JSON_FIELD_ANIMATION_MODE, 0, Animation::getCount(), errorCallback))
        return false;
      animationMode = json[JSON_FIELD_ANIMATION_MODE];
    }
    if (json.containsKey(JSON_FIELD_ANIMATION_PROGRESS_MODE)) {
      if (!validateRange(json, JSON_FIELD_ANIMATION_PROGRESS_MODE, 0, AnimationProgressMode::getCount(), errorCallback))
        return false;
      animationProgressMode = json[JSON_FIELD_ANIMATION_PROGRESS_MODE];
    }
    if (json.containsKey(JSON_FIELD_ANIMATION_SPEED)) {
      if (!validateRange(json, JSON_FIELD_ANIMATION_SPEED, 0, 255, errorCallback))
        return false;
      animationSpeed = json[JSON_FIELD_ANIMATION_SPEED];
    }
    if (json.containsKey(JSON_FIELD_ANIMATION_INTENSITY)) {
      if (!validateRange(json, JSON_FIELD_ANIMATION_INTENSITY, 0, 255, errorCallback))
        return false;
      animationIntensity = json[JSON_FIELD_ANIMATION_INTENSITY];
    }
    if (json.containsKey(JSON_FIELD_ANIMATION_DIRECTION)) {
      animationDirection = json[JSON_FIELD_ANIMATION_DIRECTION];
    }
    if (json.containsKey(JSON_FIELD_COLORS)) {
      if (!json.is<JsonArray>(JSON_FIELD_COLORS) && !errorCallback("Json prop colors must be array"))
        return false;
      HtmlColor htmlColor;
      JsonArray &colorsArray = (JsonArray &)json[JSON_FIELD_COLORS];
      for (int i = 0; i < colorsArray.size(); i++) {
        String colorCode = colorsArray[i];
        htmlColor.Parse<HtmlColorNames>(colorCode);
        colors[i] = RgbColor(htmlColor);
      }
      colorsCount = colorsArray.size();
    }

    return true;
  };

  bool updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
    char colorBuffer[10];
    JsonArray &colorsArray = json.createNestedArray(JSON_FIELD_COLORS);
    for (int i = 0; i < colorsCount; i++) {
      HtmlColor htmlColor(colors[i]);
      htmlColor.ToNumericalString(colorBuffer, 10);
      colorsArray.add(String(colorBuffer));
    }
    json[JSON_FIELD_INDEX] = index;
    json[JSON_FIELD_DESCRIPTION] = description;
    json[JSON_FIELD_COLOR_SELECTION_MODE] = colorSelectionMode;
    json[JSON_FIELD_ANIMATION_MODE] = animationMode;
    json[JSON_FIELD_ANIMATION_SPEED] = animationSpeed;
    json[JSON_FIELD_ANIMATION_INTENSITY] = animationIntensity;
    json[JSON_FIELD_ANIMATION_DIRECTION] = animationDirection;
    json[JSON_FIELD_ANIMATION_PROGRESS_MODE] = animationProgressMode;
    return true;
  };
};

#endif
