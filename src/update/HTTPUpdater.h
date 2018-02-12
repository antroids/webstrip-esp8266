#ifndef UPDATE_HTTP_UPDATER_H
#define UPDATE_HTTP_UPDATER_H

#include "../FileSystem.h"
#include "../Log.h"
#include "../domain/JsonEntity.h"
#include "Updater.h"
#include <ESP8266HTTPClient.h>

class UpdaterVersionInfo : public JsonEntity {
  uint16_t currentVersion;
  uint16_t availableVersion;

  bool updateJsonFromEntity(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
    json["currentVersion"] = currentVersion;
    json["availableVersion"] = availableVersion;
    return true;
  }

  bool updateEntityFromJson(JsonObject &json, ErrorCallbackFunctionType errorCallback) {
    currentVersion = json["currentVersion"];
    availableVersion = json["availableVersion"];
    return true;
  }
};

class HTTPUpdater : public Updater {
public:
  UpdaterVersionInfo getVersionInfo();

  bool saveFile(const char *url, const char *filePath, ErrorCallbackFunctionType errorCallback);

  // virtual const char *getVersionInfoUrl() = 0;

  HTTPUpdater(UpdaterStatusCallbackFunctionType sc) : Updater(sc) {}

protected:
  HTTPClient client;
};

#endif
