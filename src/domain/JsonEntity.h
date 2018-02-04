#ifndef JSON_ENTITY_H
#define JSON_ENTITY_H

#include "Types.h"
#include <Arduino.h>
#include <ArduinoJson.h>

class JsonEntity {
public:
  virtual bool updateEntityFromJson(JsonObject &json, ErrorCallbackFunctionType errorCallback) { return true; };
  virtual bool updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback) { return true; };

protected:
  bool validateRange(JsonObject &json, const char *fieldName, int min, int max, ErrorCallbackFunctionType errorCallback) {
    int value = json[fieldName];
    char buf[128];
    if (value < min || value > max) {
      String msg = String("Value '") + String(value) + String("' in field '") + String(fieldName) + String("' must be in range [") + String(min) + String(",") +
                   String(max) + String("]");
      msg.toCharArray(buf, 128);
      return errorCallback(buf);
    }
    return true;
  }
};

#endif
