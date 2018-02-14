#ifndef DOMAIN_LED_STRIP_MODE_H
#define DOMAIN_LED_STRIP_MODE_H

#include "JsonEntity.h"
#include <Arduino.h>

#define OPTIONS_JSON_FILE_PATH "/options.json"

#define JSON_FIELD_OPTIONS_DOMAIN "domain"
#define JSON_FIELD_OPTIONS_DOMAIN_SIZE 32
#define JSON_FIELD_OPTIONS_PIXEL_COUNT "pixelCount"
#define JSON_FIELD_OPTIONS_PORT "port"

class Options : public JsonEntity {
public:
  led_index_t pixelCount = 32;
  char domain[JSON_FIELD_OPTIONS_DOMAIN_SIZE] = "WebStrip";
  uint32_t port = 80;

  bool updateEntityFromJson(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
    if (json.containsKey(JSON_FIELD_OPTIONS_PIXEL_COUNT)) {
      pixelCount = json[JSON_FIELD_OPTIONS_PIXEL_COUNT];
    }
    if (json.containsKey(JSON_FIELD_OPTIONS_PORT)) {
      port = json[JSON_FIELD_OPTIONS_PORT];
    }
    if (json.containsKey(JSON_FIELD_OPTIONS_DOMAIN)) {
      String d = json[JSON_FIELD_OPTIONS_DOMAIN];
      d.toCharArray(this->domain, JSON_FIELD_OPTIONS_DOMAIN_SIZE);
    }
    return true;
  }

  bool updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
    json[JSON_FIELD_OPTIONS_PIXEL_COUNT] = pixelCount;
    json[JSON_FIELD_OPTIONS_PORT] = port;
    json[JSON_FIELD_OPTIONS_DOMAIN] = domain;
    return true;
  }
};

#endif
