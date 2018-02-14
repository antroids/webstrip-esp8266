#include "WebServer.h"

void WebServer::send(int code, JsonObject *jsonObject) {
  String header;
  const size_t length = jsonObject->measureLength();
  setupResponseHeaders();
  _prepareHeader(header, code, MIME_JSON, length);
  _currentClient.write(header.c_str(), header.length());
  if (length) {
    jsonObject->printTo(_currentClient);
  }
}

bool WebServer::send(int code, JsonEntity *jsonEntity, ErrorCallbackFunctionType errorCallback) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();

  if (jsonEntity->updateJsonFromEntity(json, errorCallback)) {
    setupResponseHeaders();
    send(code, &json);
    return true;
  }
  return false;
}

bool WebServer::send(int code, JsonEntity *jsonEntity) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();

  if (jsonEntity->updateJsonFromEntity(json, Log::logErrorHandler)) {
    setupResponseHeaders();
    send(code, &json);
    return true;
  }
  return false;
}
