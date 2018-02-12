#ifndef UPDATE_WEB_CLIENT_UPDATER_H
#define UPDATE_WEB_CLIENT_UPDATER_H

#include "HTTPUpdater.h"

#define WEB_CLIENT_HTML_URL "http://s3.amazonaws.com/antroids-webstrip/webclient/index.html"
#define WEB_CLIENT_JS_URL "http://s3.amazonaws.com/antroids-webstrip/webclient/static/js/main.js.gz"
#define WEB_CLIENT_CSS_URL "http://s3.amazonaws.com/antroids-webstrip/webclient/static/css/main.css.gz"

#define WEB_CLIENT_HTML_FILE "/web/index.html"
#define WEB_CLIENT_JS_FILE "/web/static/js/main.js.gz"
#define WEB_CLIENT_CSS_FILE "/web/static/css/main.css.gz"

class WebClientUpdater : public HTTPUpdater {
public:
  WebClientUpdater(UpdaterStatusCallbackFunctionType sc) : HTTPUpdater(sc) {}

protected:
  bool update(ErrorCallbackFunctionType errorCallback) {
    char tempHtmlFileName[32] = {0};
    char tempJsFileName[32] = {0};
    char tempCssFileName[32] = {0};
    bool result = true;

    SPIFFS.generateTempFileName(tempHtmlFileName);
    if (!saveFile(WEB_CLIENT_HTML_URL, tempHtmlFileName, errorCallback)) {
      SPIFFS.remove(tempHtmlFileName);
      Log::mainLogger.errf("Can't download from %s to %s", WEB_CLIENT_HTML_URL, tempHtmlFileName);
      return false;
    }
    Log::mainLogger.infof("File downloaded from %s to %s", WEB_CLIENT_HTML_URL, tempHtmlFileName);
    SPIFFS.generateTempFileName(tempJsFileName);
    if (!saveFile(WEB_CLIENT_JS_URL, tempJsFileName, errorCallback)) {
      SPIFFS.remove(tempHtmlFileName);
      SPIFFS.remove(tempJsFileName);
      Log::mainLogger.errf("Can't download from %s to %s", WEB_CLIENT_JS_URL, tempJsFileName);
      return false;
    }
    Log::mainLogger.infof("File downloaded from %s to %s", WEB_CLIENT_JS_URL, tempJsFileName);
    SPIFFS.generateTempFileName(tempCssFileName);
    if (!saveFile(WEB_CLIENT_CSS_URL, tempCssFileName, errorCallback)) {
      SPIFFS.remove(tempHtmlFileName);
      SPIFFS.remove(tempJsFileName);
      SPIFFS.remove(tempCssFileName);
      Log::mainLogger.errf("Can't download from %s to %s", WEB_CLIENT_CSS_URL, tempCssFileName);
      return false;
    }
    Log::mainLogger.infof("File downloaded from %s to %s", WEB_CLIENT_CSS_URL, tempCssFileName);

    if (SPIFFS.exists(WEB_CLIENT_HTML_FILE) && SPIFFS.remove(WEB_CLIENT_HTML_FILE)) {
      Log::mainLogger.info("Old HTML file removed");
    }
    SPIFFS.rename(tempHtmlFileName, WEB_CLIENT_HTML_FILE);
    if (SPIFFS.exists(WEB_CLIENT_JS_FILE) && SPIFFS.remove(WEB_CLIENT_JS_FILE)) {
      Log::mainLogger.info("Old JS file removed");
    }
    SPIFFS.rename(tempJsFileName, WEB_CLIENT_JS_FILE);
    if (SPIFFS.exists(WEB_CLIENT_CSS_FILE) && SPIFFS.remove(WEB_CLIENT_CSS_FILE)) {
      Log::mainLogger.info("Old CSS file removed");
    }
    SPIFFS.rename(tempCssFileName, WEB_CLIENT_CSS_FILE);
    Log::mainLogger.info("Webclient updated");

    return true;
  }
};

#endif
