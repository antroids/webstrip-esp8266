#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define NO_GLOBAL_SPIFFS
#include "FS.h"

#include "Types.h"
#include <ArduinoJson.h>
#include <limits.h>

#define TEMP_DIR "/temp"

class FileSystem : public FS {
public:
  bool saveJson(JsonObject &json, const char *filePath, ErrorCallbackFunctionType errorCallback) {
    File jsonFile = open(filePath, "w");
    json.printTo(jsonFile);
    jsonFile.close();
    return true;
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

  File createTempFile() {
    char buf[32];
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
