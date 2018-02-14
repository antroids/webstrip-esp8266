#ifndef API_WEB_SERVER_H
#define API_WEB_SERVER_H

#include "../Log.h"
#include "../domain/JsonEntity.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

#define MIME_JSON "application/json"
#define MIME_HTML "text/html"
#define MIME_JS "text/javascript"
#define MIME_CSS "text/css"

#define HTTP_CODE_OK 200
#define HTTP_CODE_WRONG_REQUEST 400
#define HTTP_CODE_NOT_FOUND 404
#define HTTP_CODE_SERVER_ERROR 500

#define ARG_JSON "data"

class WebServer : public ESP8266WebServer {
public:
  WebServer(IPAddress addr, int port = 80) : ESP8266WebServer(addr, port){};
  WebServer(int port = 80) : ESP8266WebServer(port){};

  void send(int code, JsonObject *jsonObject);

  bool send(int code, JsonEntity *jsonEntity, ErrorCallbackFunctionType errorCallback);
  bool send(int code, JsonEntity *jsonEntity);
  using ESP8266WebServer::send;

  void setupResponseHeaders() { sendHeader("Access-Control-Allow-Origin", "*"); }
};

#endif
