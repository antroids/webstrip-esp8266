#ifndef API_JSON_API_H
#define API_JSON_API_H

#include "../Types.h"
#include "../domain/Error.h"
#include "../domain/Esp8266SysInfo.h"
#include <ArduinoOTA.h>

#define ARG_INDEX "index"
#define ARG_JSON "data"
#define ARG_AUTO_MODE_CHANGING "autoModeChanging"

class HTTPServer;
class HTTPRequest;
class HTTPResponse;

class JsonApi : public UpdateHandler {
public:
  JsonApi(Context *_context) : context(_context) { initWebServer(); };

  void handleUpdate();

  static const char *WEB_CLIENT_HTML_FILE_PATH;
  static const char *WEB_CLIENT_JS_FILE_PATH;
  static const char *WEB_CLIENT_CSS_FILE_PATH;

protected:
  Context *context;
  HTTPServer *server;

  void initWebServer();
  void initUrlMappings();

  void sendError(HTTPResponse *response, const char *message, int httpCode);

  ErrorCallbackFunctionType getRequestErrorHandler(HTTPResponse *response);
  bool requestErrorHandler(HTTPResponse *response, const char *errorMessage);
  bool loadModeFromFS(index_id_t index, ErrorCallbackFunctionType errorCallback);
  JsonObject &loadJsonFromFS(DynamicJsonBuffer *jsonBuffer, String path, ErrorCallbackFunctionType errorCallback);

  void onModePost(HTTPRequest *request, HTTPResponse *response);
  void onModeGet(HTTPRequest *request, HTTPResponse *response);
  void onSaveModeGet(HTTPRequest *request, HTTPResponse *response);
  void onLoadModeGet(HTTPRequest *request, HTTPResponse *response);
  void onListModesGet(HTTPRequest *request, HTTPResponse *response);
  void onOptionsPost(HTTPRequest *request, HTTPResponse *response);
  void onOptionsGet(HTTPRequest *request, HTTPResponse *response);
  void onOtaUpdate(HTTPRequest *request, HTTPResponse *response);
  void onWebClientUpdate(HTTPRequest *request, HTTPResponse *response);
  void onSysInfoGet(HTTPRequest *request, HTTPResponse *response);

  void handleNotFound();
};

#endif
