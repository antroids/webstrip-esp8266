#ifndef API_JSON_API_H
#define API_JSON_API_H

#include "../Types.h"
#include "../domain/Error.h"
#include "../domain/Esp8266SysInfo.h"
#include "../update/WebClientUpdater.h"
#include "WebServer.h"
#include <ArduinoOTA.h>

#define ARG_INDEX "index"
#define ARG_AUTO_MODE_CHANGING "autoModeChanging"

class JsonApi {
public:
  JsonApi(Context *_context) : context(_context) { initWebServer(); };

  void processConnection() { server->handleClient(); }

protected:
  Context *context;
  WebServer *server;

  void initWebServer();
  void initUrlMappings();

  void sendError(const char *message, int httpCode);

  ErrorCallbackFunctionType getRequestErrorHandler();
  bool requestErrorHandler(const char *errorMessage);
  bool loadModeFromFS(index_id_t index, ErrorCallbackFunctionType errorCallback);
  JsonObject &loadJsonFromFS(DynamicJsonBuffer *jsonBuffer, String path, ErrorCallbackFunctionType errorCallback);

  void onRoot();
  void onIndexJsGz();
  void onIndexCssGz();
  void onModePost();
  void onModeGet();
  void onSaveModeGet();
  void onLoadModeGet();
  void onListModesGet();
  void onOptionsPost();
  void onOptionsGet();
  void onOtaUpdate();
  void onWebClientUpdate();
  void onSysInfoGet();

  void handleNotFound();
};

#include "../Context.h"

#endif
