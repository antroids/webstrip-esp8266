#ifndef UPDATE_HTTP_UPDATER_H
#define UPDATE_HTTP_UPDATER_H

#include "../FileSystem.h"
#include "../Log.h"
#include "../domain/JsonEntity.h"
#include "../web/HTTPClient.h"
#include "Updater.h"
#include "limits.h"

struct UpdaterVersionInfo : public JsonEntity {
public:
  static UpdaterVersionInfo invalid;

  uint16_t currentVersion;
  uint16_t availableVersion;

  UpdaterVersionInfo() {}
  UpdaterVersionInfo(uint16_t current, uint16_t available) : JsonEntity(), currentVersion(current), availableVersion(available) {}

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

  bool operator==(const UpdaterVersionInfo &rhs) { return currentVersion == rhs.currentVersion && availableVersion == rhs.availableVersion; }
  bool operator!=(const UpdaterVersionInfo &rhs) { return !(operator==(rhs)); }
};

class HTTPUpdater : public Updater {
public:
  UpdaterVersionInfo getVersionInfo(ErrorCallbackFunctionType errorCallback);

  virtual const char *getVersionInfoUrl() = 0;
  virtual const char *getVersionInfoFilePath() = 0;

  HTTPUpdater(WebStrip::HTTPClient *_client, UpdaterStatusCallbackFunctionType sc) : Updater(sc), client(_client) {}

protected:
  WebStrip::HTTPClient *client;
};

#endif
