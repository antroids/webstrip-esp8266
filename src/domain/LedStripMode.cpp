#include "ColorSelectionMode.h"
#include "LedStripMode.h"

bool LedStripMode::updateEntityFromJson(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
  index_id_t am = animationMode;

  if (json.containsKey(JSON_FIELD_MODE_DESCRIPTION)) {
    String description = json[JSON_FIELD_MODE_DESCRIPTION];
    description.toCharArray(this->description, MODE_DESCRIPTION_SIZE);
  }
  if (json.containsKey(JSON_FIELD_MODE_COLOR_SELECTION_MODE)) {
    if (!validateRange(json, JSON_FIELD_MODE_COLOR_SELECTION_MODE, 0, ColorSelectionMode::getCount() - 1, errorCallback))
      return false;
    colorSelectionMode = json[JSON_FIELD_MODE_COLOR_SELECTION_MODE];
  }
  if (json.containsKey(JSON_FIELD_MODE_ANIMATION_MODE)) {
    if (!validateRange(json, JSON_FIELD_MODE_ANIMATION_MODE, 0, Animation::getCount() - 1, errorCallback))
      return false;
    am = json[JSON_FIELD_MODE_ANIMATION_MODE];
  }
  if (json.containsKey(JSON_FIELD_MODE_ANIMATION_PROGRESS_MODE)) {
    if (!validateRange(json, JSON_FIELD_MODE_ANIMATION_PROGRESS_MODE, 0, AnimationProgressMode::getCount() - 1, errorCallback))
      return false;
    animationProgressMode = json[JSON_FIELD_MODE_ANIMATION_PROGRESS_MODE];
  }
  if (json.containsKey(JSON_FIELD_MODE_ANIMATION_SPEED)) {
    if (!validateRange(json, JSON_FIELD_MODE_ANIMATION_SPEED, Animation::inputScale.min, Animation::inputScale.max, errorCallback))
      return false;
    animationSpeed = json[JSON_FIELD_MODE_ANIMATION_SPEED];
  }
  if (json.containsKey(JSON_FIELD_MODE_ANIMATION_INTENSITY)) {
    if (!validateRange(json, JSON_FIELD_MODE_ANIMATION_INTENSITY, Animation::inputScale.min, Animation::inputScale.max, errorCallback))
      return false;
    animationIntensity = json[JSON_FIELD_MODE_ANIMATION_INTENSITY];
  }
  if (json.containsKey(JSON_FIELD_MODE_ANIMATION_DIRECTION)) {
    animationDirection = json[JSON_FIELD_MODE_ANIMATION_DIRECTION];
  }
  if (json.containsKey(JSON_FIELD_MODE_NEXT_MODE)) {
    nextMode = json[JSON_FIELD_MODE_NEXT_MODE];
  }
  if (json.containsKey(JSON_FIELD_MODE_NEXT_MODE_DELAY)) {
    nextModeDelay = json[JSON_FIELD_MODE_NEXT_MODE_DELAY];
  }
  if (json.containsKey(JSON_FIELD_MODE_COLORS)) {
    if (!json.is<JsonArray>(JSON_FIELD_MODE_COLORS) && !errorCallback("Json prop colors must be array"))
      return false;
    HtmlColor htmlColor;
    JsonArray &colorsArray = (JsonArray &)json[JSON_FIELD_MODE_COLORS];
    if (colorsArray.size() > MODE_COLORS_SIZE && !errorCallback("Too many colors!")) {
      return false;
    }
    for (uint8_t i = 0; i < colorsArray.size(); i++) {
      String colorCode = colorsArray[i];
      htmlColor.Parse<HtmlColorNames>(colorCode);
      colors[i] = RgbColor(htmlColor);
    }
    colorsCount = colorsArray.size();
  }

  setAnimationMode(am);
  return true;
};

bool LedStripMode::updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
  char colorBuffer[10];
  JsonArray &colorsArray = json.createNestedArray(JSON_FIELD_MODE_COLORS);
  for (int i = 0; i < colorsCount; i++) {
    HtmlColor htmlColor(colors[i]);
    htmlColor.ToNumericalString(colorBuffer, 10);
    colorsArray.add(String(colorBuffer));
  }
  json[JSON_FIELD_MODE_INDEX] = index;
  json[JSON_FIELD_MODE_DESCRIPTION] = description;
  json[JSON_FIELD_MODE_COLOR_SELECTION_MODE] = colorSelectionMode;
  json[JSON_FIELD_MODE_ANIMATION_MODE] = animationMode;
  json[JSON_FIELD_MODE_ANIMATION_SPEED] = animationSpeed;
  json[JSON_FIELD_MODE_ANIMATION_INTENSITY] = animationIntensity;
  json[JSON_FIELD_MODE_ANIMATION_DIRECTION] = animationDirection;
  json[JSON_FIELD_MODE_NEXT_MODE] = nextMode;
  json[JSON_FIELD_MODE_NEXT_MODE_DELAY] = nextModeDelay;
  json[JSON_FIELD_MODE_ANIMATION_PROGRESS_MODE] = animationProgressMode;
  return true;
};

void LedStripMode::setAnimationMode(const index_id_t newLedStripAnimationMode) {
  Log::mainLogger.infof("Changing animation mode from '%d' to '%d'", animationMode, newLedStripAnimationMode);
  Animation::getFromIndex(animationMode)->stop();
  Log::mainLogger.info("Stopping previous animation mode");
  Animation::getFromIndex(newLedStripAnimationMode)->start();
  Log::mainLogger.info("Starting next animation mode");
  animationMode = newLedStripAnimationMode;
}
