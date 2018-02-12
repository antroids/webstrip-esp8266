#include "HTTPUpdater.h"

UpdaterVersionInfo UpdaterVersionInfo::invalid(UINT16_MAX, UINT16_MAX);

bool HTTPUpdater::saveFile(const char *url, const char *filePath, ErrorCallbackFunctionType errorCallback) {
  client.begin(url);
  Log::mainLogger.infof("URL opened '%s'", url);
  int httpCode = client.GET();
  bool result = true;
  if (httpCode == HTTP_CODE_OK) {
    int len = client.getSize();
    uint8_t buff[128] = {0};
    WiFiClient *stream = client.getStreamPtr();
    File file = SPIFFS.open(filePath, "w");
    Log::mainLogger.infof("File opened '%s'", filePath);
    while (client.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      Log::mainLogger.infof("File downloading, available %d", size);
      if (size) {
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        file.write(buff, c);
        if (len > 0) {
          len -= c;
        }
      }
      delay(1);
    }
    file.close();
  } else {
    char messageBuffer[256];
    sprintf(messageBuffer, "'%s' code is %d", url, httpCode);
    Log::mainLogger.err(messageBuffer);
    result = errorCallback(messageBuffer);
  }
  client.end();
  Log::mainLogger.infof("Downloaded from '%s' to '%s'", url, filePath);
  return result;
}

UpdaterVersionInfo HTTPUpdater::getVersionInfo(ErrorCallbackFunctionType errorCallback) {
  DynamicJsonBuffer currentBuffer;
  DynamicJsonBuffer availableBuffer;
  UpdaterVersionInfo current;
  UpdaterVersionInfo available;
  char buf[TEMP_FILE_MAX_LENGTH] = {0};

  if (SPIFFS.exists(getVersionInfoFilePath())) {
    JsonObject &currentJson = SPIFFS.loadJson(&currentBuffer, getVersionInfoFilePath(), errorCallback);
    if (currentJson == JsonObject::invalid() || !current.updateEntityFromJson(currentJson, errorCallback)) {
      return UpdaterVersionInfo::invalid;
    }
  } else {
    current.currentVersion = 0;
  }
  Log::mainLogger.infof("Current version is %lu", current.currentVersion);

  SPIFFS.generateTempFileName(buf);
  if (!saveFile(getVersionInfoUrl(), buf, errorCallback)) {
    return UpdaterVersionInfo::invalid;
  }
  Log::mainLogger.info("Version file downloaded");
  JsonObject &availableJson = SPIFFS.loadJson(&availableBuffer, buf, errorCallback);
  if (availableJson == JsonObject::invalid() || !available.updateEntityFromJson(availableJson, errorCallback)) {
    Log::mainLogger.errf("Can't load JSON from file '%s'", buf);
    SPIFFS.remove(buf);
    return UpdaterVersionInfo::invalid;
  }
  SPIFFS.remove(buf);
  Log::mainLogger.infof("Available version is %lu", available.availableVersion);

  return UpdaterVersionInfo(current.currentVersion, available.availableVersion);
}
