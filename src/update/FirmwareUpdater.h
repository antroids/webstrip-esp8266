#ifndef UPDATE_FIRMWARE_UPDATER_H
#define UPDATE_FIRMWARE_UPDATER_H

#include "HTTPUpdater.h"

#define FIRMWARE_VERSION_URL "http://s3.amazonaws.com/antroids-webstrip/firmware/esp8266.json"
#define FIRMWARE_URL "http://s3.amazonaws.com/antroids-webstrip/firmware/esp8266.bin"

#define FIRMWARE_VERSION_FILE "/firmware/esp8266.json"

class FirmwareUpdater : public HTTPUpdater {
public:
  FirmwareUpdater(UpdaterStatusCallbackFunctionType sc) : HTTPUpdater(sc) {}

protected:
  bool update(ErrorCallbackFunctionType errorCallback) {
    if (!downloadFirmware(FIRMWARE_URL, errorCallback)) {
      return false;
    }

    statusCallback(UPDATER_STATUS_PROGRESS, 0.90);
    UpdaterVersionInfo versionInfo = getVersionInfo(errorCallback);
    if (versionInfo == UpdaterVersionInfo::invalid) {
      return false;
    }
    statusCallback(UPDATER_STATUS_PROGRESS, 0.95);
    versionInfo.currentVersion = versionInfo.availableVersion;
    if (!SPIFFS.saveJson(&versionInfo, FIRMWARE_VERSION_URL, errorCallback)) {
      return errorCallback("Can't save Firmware version json");
    }
    if (!Update.end()) {
      errorCallback("Firmware flashing error, eboot write failed");
      return false;
    }
    Log::mainLogger.info("Firmware updated");
    statusCallback(UPDATER_STATUS_END, 1);
    ESP.restart(); // add abstraction layer
    return true;
  }

  bool downloadFirmware(const char *url, ErrorCallbackFunctionType errorCallback) {
    HTTPClient client;

    client.begin(url);
    Log::mainLogger.infof("URL opened '%s'", url);
    int httpCode = client.GET();
    bool result = true;
    float progress = 0;
    if (httpCode == HTTP_CODE_OK) {
      int len = client.getSize();
      int targetLen = len;
      uint8_t buff[512] = {0};
      WiFiClient *stream = client.getStreamPtr();

      Log::mainLogger.infof("Firmware size %d", len);
      if (len <= 0) {
        errorCallback("Cannot get firmware size from server!");
        client.end();
        return false;
      }
      if (!Update.begin(len)) {
        Log::mainLogger.errf("Cannot begin firmware update, error %u", Update.getError());
        errorCallback("Cannot begin firmware update");
        client.end();
        return false;
      }
      while (client.connected() && len > 0) {
        size_t size = stream->available();
        Log::mainLogger.infof("Firmware downloading, available %d", size);
        if (size) {
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          Update.write(buff, c);
          if (len > 0) {
            len -= c;
            float localProgress = ((float)len / targetLen) * 0.9;
            if (localProgress - progress > 0.01) {
              progress = localProgress;
              statusCallback(UPDATER_STATUS_PROGRESS, progress);
            }
          }
        }
        delay(10);
      }
    } else {
      char messageBuffer[256];
      sprintf(messageBuffer, "'%s' code is %d", url, httpCode);
      result = errorCallback(messageBuffer);
    }
    client.end();
    Log::mainLogger.infof("Firmware downloaded from '%s'", url);
    return result;
  }

  const char *getVersionInfoUrl() { return FIRMWARE_VERSION_URL; }
  const char *getVersionInfoFilePath() { return FIRMWARE_VERSION_FILE; }
};

#endif
