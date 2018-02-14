#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define NO_GLOBAL_SPIFFS
#include "FS.h"

#include "Types.h"
#include "domain/JsonEntity.h"
#include <ArduinoJson.h>
#include <limits.h>

#define TEMP_DIR "/temp"
#define FILE_MAX_LENGTH 32
#define TEMP_FILE_MAX_LENGTH FILE_MAX_LENGTH

class FileSystem : public FS {
public:
  bool saveJson(JsonObject &json, const char *filePath, ErrorCallbackFunctionType errorCallback) {
    File jsonFile = open(filePath, "w");
    json.printTo(jsonFile);
    jsonFile.close();
    return true;
  }

  bool saveJson(JsonEntity *jsonEntity, const char *filePath, ErrorCallbackFunctionType errorCallback) {
    DynamicJsonBuffer jsonBuffer;
    char buf[FILE_MAX_LENGTH + 30];
    JsonObject &json = jsonBuffer.createObject();
    jsonEntity->updateJsonFromEntity(json, errorCallback);
    if (!saveJson(json, filePath, errorCallback)) {
      sprintf(buf, "Can't save JSON to file '%s'", filePath);
      return errorCallback(buf);
    }
    return true;
  }

  bool saveJson(JsonEntity *jsonEntity, String filePath, ErrorCallbackFunctionType errorCallback) {
    char buf[FILE_MAX_LENGTH];
    filePath.toCharArray(buf, FILE_MAX_LENGTH);
    return saveJson(jsonEntity, buf, errorCallback);
  }

  JsonObject &loadJson(DynamicJsonBuffer *jsonBuffer, const char *filePath, ErrorCallbackFunctionType errorCallback) {
    if (!exists(filePath)) {
      errorCallback("JSON file not found!");
      return JsonObject::invalid();
    }
    File jsonFile = open(filePath, "r");
    JsonObject &json = jsonBuffer->parseObject(jsonFile);
    jsonFile.close();
    return json;
  }

  bool loadJson(JsonEntity *jsonEntity, const char *filePath, ErrorCallbackFunctionType errorCallback) {
    DynamicJsonBuffer jsonBuffer;
    char buf[FILE_MAX_LENGTH + 50];
    JsonObject &json = loadJson(&jsonBuffer, filePath, errorCallback);
    if (json == JsonObject::invalid() || !jsonEntity->updateEntityFromJson(json, errorCallback)) {
      return false;
    }
    return true;
  }

  bool loadJson(JsonEntity *jsonEntity, String filePath, ErrorCallbackFunctionType errorCallback) {
    char buf[FILE_MAX_LENGTH];
    filePath.toCharArray(buf, FILE_MAX_LENGTH);
    return loadJson(jsonEntity, buf, errorCallback);
  }

  File createTempFile() {
    char buf[TEMP_FILE_MAX_LENGTH];
    generateTempFileName(buf);
    return open(buf, "w");
  }

  void generateTempFileName(char *buf) {
    do {
      sprintf(buf, "%s/%lu.tmp", TEMP_DIR, random(ULONG_MAX));
    } while (exists(buf));
  }

  void clearTempDir() {
    Dir tempDir = openDir(TEMP_DIR);
    while (tempDir.next()) {
      remove(tempDir.fileName());
    }
  }
};

extern FileSystem SPIFFS;

#endif
