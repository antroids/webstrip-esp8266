#ifndef UPDATE_WEB_CLIENT_UPDATER_H
#define UPDATE_WEB_CLIENT_UPDATER_H

#include "../api/JsonApi.h"
#include "HTTPUpdater.h"

#define WEB_CLIENT_VERSION_URL "http://s3.amazonaws.com/antroids-webstrip/webclient/webclient.json"

#define WEB_CLIENT_HTML_URL "http://s3.amazonaws.com/antroids-webstrip/webclient/index.html"
#define WEB_CLIENT_JS_URL "http://s3.amazonaws.com/antroids-webstrip/webclient/static/js/main.js.gz"
#define WEB_CLIENT_CSS_URL "http://s3.amazonaws.com/antroids-webstrip/webclient/static/css/main.css.gz"

#define WEB_CLIENT_VERSION_FILE "/web/webclient.json"

#define WEB_CLIENT_HTML_FILE (JsonApi::WEB_CLIENT_HTML_FILE_PATH)
#define WEB_CLIENT_JS_FILE (JsonApi::WEB_CLIENT_JS_FILE_PATH)
#define WEB_CLIENT_CSS_FILE (JsonApi::WEB_CLIENT_CSS_FILE_PATH)

class WebClientUpdater : public HTTPUpdater {
public:
  WebClientUpdater(WebStrip::HTTPClient *_client, UpdaterStatusCallbackFunctionType sc) : HTTPUpdater(_client, sc) {}

protected:
  bool update(ErrorCallbackFunctionType errorCallback) {
    char tempHtmlFileName[TEMP_FILE_MAX_LENGTH] = {0};
    char tempJsFileName[TEMP_FILE_MAX_LENGTH] = {0};
    char tempCssFileName[TEMP_FILE_MAX_LENGTH] = {0};
    bool result = true;

    SPIFFS.generateTempFileName(tempHtmlFileName);
    if (!client->downloadResource(WEB_CLIENT_HTML_URL, tempHtmlFileName, errorCallback)) {
      SPIFFS.remove(tempHtmlFileName);
      Log::mainLogger.errf("Can't download from %s to %s", WEB_CLIENT_HTML_URL, tempHtmlFileName);
      return false;
    }
    statusCallback(UPDATER_STATUS_PROGRESS, 0.15);
    Log::mainLogger.infof("File downloaded from %s to %s", WEB_CLIENT_HTML_URL, tempHtmlFileName);
    SPIFFS.generateTempFileName(tempJsFileName);
    if (!client->downloadResource(WEB_CLIENT_JS_URL, tempJsFileName, errorCallback)) {
      SPIFFS.remove(tempHtmlFileName);
      SPIFFS.remove(tempJsFileName);
      Log::mainLogger.errf("Can't download from %s to %s", WEB_CLIENT_JS_URL, tempJsFileName);
      return false;
    }
    statusCallback(UPDATER_STATUS_PROGRESS, 0.50);
    Log::mainLogger.infof("File downloaded from %s to %s", WEB_CLIENT_JS_URL, tempJsFileName);
    SPIFFS.generateTempFileName(tempCssFileName);
    if (!client->downloadResource(WEB_CLIENT_CSS_URL, tempCssFileName, errorCallback)) {
      SPIFFS.remove(tempHtmlFileName);
      SPIFFS.remove(tempJsFileName);
      SPIFFS.remove(tempCssFileName);
      Log::mainLogger.errf("Can't download from %s to %s", WEB_CLIENT_CSS_URL, tempCssFileName);
      return false;
    }
    statusCallback(UPDATER_STATUS_PROGRESS, 0.75);
    Log::mainLogger.infof("File downloaded from %s to %s", WEB_CLIENT_CSS_URL, tempCssFileName);

    if (SPIFFS.exists(WEB_CLIENT_HTML_FILE) && SPIFFS.remove(WEB_CLIENT_HTML_FILE)) {
      Log::mainLogger.info("Old HTML file removed");
    }
    if (!SPIFFS.rename(tempHtmlFileName, WEB_CLIENT_HTML_FILE)) {
      return errorCallback("Can't rename HTML file");
    }
    if (SPIFFS.exists(WEB_CLIENT_JS_FILE) && SPIFFS.remove(WEB_CLIENT_JS_FILE)) {
      Log::mainLogger.info("Old JS file removed");
    }
    if (!SPIFFS.rename(tempJsFileName, WEB_CLIENT_JS_FILE)) {
      return errorCallback("Can't rename JS file");
    }
    if (SPIFFS.exists(WEB_CLIENT_CSS_FILE) && SPIFFS.remove(WEB_CLIENT_CSS_FILE)) {
      Log::mainLogger.info("Old CSS file removed");
    }
    if (!SPIFFS.rename(tempCssFileName, WEB_CLIENT_CSS_FILE)) {
      return errorCallback("Can't rename CSS file");
    }
    statusCallback(UPDATER_STATUS_PROGRESS, 0.80);
    UpdaterVersionInfo versionInfo = getVersionInfo(errorCallback);
    if (versionInfo == UpdaterVersionInfo::invalid) {
      return false;
    }
    statusCallback(UPDATER_STATUS_PROGRESS, 0.95);
    versionInfo.currentVersion = versionInfo.availableVersion;
    if (!SPIFFS.saveJson(&versionInfo, WEB_CLIENT_VERSION_FILE, errorCallback)) {
      return errorCallback("Can't save Webclient version json");
    }
    Log::mainLogger.info("Webclient updated");

    return true;
  }

  const char *getVersionInfoUrl() { return WEB_CLIENT_VERSION_URL; }
  const char *getVersionInfoFilePath() { return WEB_CLIENT_VERSION_FILE; }
};

#endif
