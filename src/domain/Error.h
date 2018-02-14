#ifndef DOMAIN_ERROR_H
#define DOMAIN_ERROR_H

#include "JsonEntity.h"

#define JSON_FIELD_ERROR_MESSAGE "message"
#define JSON_FIELD_ERROR_MESSAGE_LENGTH 128

class Error : public JsonEntity {
public:
  const char *message;

  Error(const char *_message) : message(_message) {}

  bool updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
    json[JSON_FIELD_ERROR_MESSAGE] = message;
    return true;
  }
};

#endif
