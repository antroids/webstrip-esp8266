#include "HTTPUpdater.h"

UpdaterVersionInfo UpdaterVersionInfo::invalid(UINT16_MAX, UINT16_MAX);

UpdaterVersionInfo HTTPUpdater::getVersionInfo(ErrorCallbackFunctionType errorCallback) {
  UpdaterVersionInfo current;
  UpdaterVersionInfo available;
  char buf[TEMP_FILE_MAX_LENGTH] = {0};

  if (SPIFFS.exists(getVersionInfoFilePath())) {
    Log::mainLogger.info("Version info file found");
    if (!SPIFFS.loadJson(&current, getVersionInfoFilePath(), errorCallback)) {
      Log::mainLogger.err("Cannot open version info file");
      return UpdaterVersionInfo::invalid;
    }
  } else {
    current.currentVersion = 0;
  }
  Log::mainLogger.infof("Current version is %lu", current.currentVersion);

  SPIFFS.generateTempFileName(buf);
  if (!client->downloadResource(getVersionInfoUrl(), buf, errorCallback)) {
    return UpdaterVersionInfo::invalid;
  }
  Log::mainLogger.info("Version file downloaded");
  if (!SPIFFS.loadJson(&available, buf, errorCallback)) {
    SPIFFS.remove(buf);
    return UpdaterVersionInfo::invalid;
  }
  SPIFFS.remove(buf);
  Log::mainLogger.infof("Available version is %lu", available.availableVersion);

  return UpdaterVersionInfo(current.currentVersion, available.availableVersion);
}
