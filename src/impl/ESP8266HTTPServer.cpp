#include "../FileSystem.h"
#include "../domain/Error.h"
#include "ESP8266HTTPServer.h"

// Server wrapper
void ESP8266WebServerWrapper::send(http_code_t code, JsonObject *jsonObject) {
  String header;
  const size_t length = jsonObject->measureLength();
  setupResponseHeaders();
  _prepareHeader(header, code, HTTP::MIME_JSON, length);
  _currentClient.write(header.c_str(), header.length());
  if (length) {
    jsonObject->printTo(_currentClient);
  }
}

bool ESP8266WebServerWrapper::send(http_code_t code, JsonEntity *jsonEntity) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();

  if (jsonEntity->updateJsonFromEntity(json, Log::logErrorHandler)) {
    setupResponseHeaders();
    send(code, &json);
    return true;
  }
  return false;
}

const char *ESP8266WebServerWrapper::getArg(const char *name) {
  for (int i = 0; i < _currentArgCount; ++i) {
    if (_currentArgs[i].key == name)
      return _currentArgs[i].value.c_str();
  }
  return "";
}

const char *ESP8266WebServerWrapper::getHeader(const char *name) {
  for (int i = 0; i < _headerKeysCount; ++i) {
    if (_currentHeaders[i].key.equalsIgnoreCase(name))
      return _currentHeaders[i].value.c_str();
  }
  return "";
}

const char *ESP8266WebServerWrapper::getURI() { return _currentUri.c_str(); }

void ESP8266WebServerWrapper::setupResponseHeaders() { sendHeader("Access-Control-Allow-Origin", "*"); }

// Request
const char *ESP8266HTTPRequest::getURI() { return server->getURI(); };
HTTPRequest::Method ESP8266HTTPRequest::getMethod() { return ESP8266HTTPServer::convertMethod(server->method()); };

const char *ESP8266HTTPRequest::getArg(const char *name) { return server->getArg(name); };
bool ESP8266HTTPRequest::hasArg(const char *name) { return server->hasArg(name); };

const char *ESP8266HTTPRequest::getHeader(const char *name) { return server->getHeader(name); };
bool ESP8266HTTPRequest::hasHeader(const char *name) { return server->hasHeader(name); };

// Response
void ESP8266HTTPResponse::send(http_code_t code, const char *mime, const char *content) {
  if (sent) {
    Log::mainLogger.err("Response already sent!");
    return;
  }
  Log::mainLogger.info("Sending string response");
  server->send(code, mime, content);
  sent = true;
};
void ESP8266HTTPResponse::send(http_code_t code, JsonObject *jsonObject) {
  if (sent) {
    Log::mainLogger.err("Response already sent!");
    return;
  }
  Log::mainLogger.info("Sending json response");
  server->send(code, jsonObject);
  sent = true;
}
void ESP8266HTTPResponse::send(http_code_t code, JsonEntity *jsonEntity) {
  if (sent) {
    Log::mainLogger.err("Response already sent!");
    return;
  }
  Log::mainLogger.info("Sending json entity response");
  server->send(code, jsonEntity);
  sent = true;
};

// Wrapper
HTTPServerHandlerFunctionWrapper::HTTPServerHandlerFunctionWrapper(ESP8266HTTPServer *_server, const char *_uri, HTTPRequest::Method _method,
                                                                   HTTPServer::HTTPServerHandlerFunction _handler)
    : FunctionRequestHandler([=]() { handle(); }, []() {}, String(_uri), ESP8266HTTPServer::convertMethod(_method)), server(_server), handler(_handler) {
  Log::mainLogger.infof("Request handler registered %s %d", _uri, _method);
}

void HTTPServerHandlerFunctionWrapper::handle() {
  server->response->reset();
  handler(server->request, server->response);
}

// Server
void ESP8266HTTPServer::on(const char *uri, HTTPRequest::Method method, HTTPServerHandlerFunction handler) {
  HTTPServerHandlerFunctionWrapper *wrapper = new HTTPServerHandlerFunctionWrapper(this, uri, method, handler);
  server->addHandler(wrapper);
}

void ESP8266HTTPServer::on(const char *uri, const char *mime, const char *filePath) {
  HTTPServerHandlerFunction h = ESP8266HTTPServer::getFileContentHandler(this->server, mime, filePath);
  on(uri, HTTPRequest::Method::GET, h);
}

void ESP8266HTTPServer::on(const char *uri, HTTPServerHandlerFunction handler) { on(uri, HTTPRequest::Method::ANY, handler); }

HTTPMethod ESP8266HTTPServer::convertMethod(HTTPRequest::Method method) {
  switch (method) {
  case HTTPRequest::Method::GET:
    return HTTPMethod::HTTP_GET;
  case HTTPRequest::Method::POST:
    return HTTPMethod::HTTP_POST;
  case HTTPRequest::Method::PUT:
    return HTTPMethod::HTTP_PUT;
  case HTTPRequest::Method::PATCH:
    return HTTPMethod::HTTP_PATCH;
  case HTTPRequest::Method::DELETE:
    return HTTPMethod::HTTP_DELETE;
  case HTTPRequest::Method::OPTIONS:
    return HTTPMethod::HTTP_OPTIONS;
  default:
    return HTTPMethod::HTTP_ANY;
  }
}

HTTPRequest::Method ESP8266HTTPServer::convertMethod(HTTPMethod method) {
  switch (method) {
  case HTTPMethod::HTTP_GET:
    return HTTPRequest::Method::GET;
  case HTTPMethod::HTTP_POST:
    return HTTPRequest::Method::POST;
  case HTTPMethod::HTTP_PUT:
    return HTTPRequest::Method::PUT;
  case HTTPMethod::HTTP_PATCH:
    return HTTPRequest::Method::PATCH;
  case HTTPMethod::HTTP_DELETE:
    return HTTPRequest::Method::DELETE;
  case HTTPMethod::HTTP_OPTIONS:
    return HTTPRequest::Method::OPTIONS;
  default:
    return HTTPRequest::Method::ANY;
  }
}

HTTPServer::HTTPServerHandlerFunction ESP8266HTTPServer::getFileContentHandler(ESP8266WebServerWrapper *server, const char *mime, const char *filePath) {
  return [=](HTTPRequest *request, HTTPResponse *response) {
    Log::mainLogger.infof("FileContentHandler %s %s", mime, filePath);
    if (SPIFFS.exists(filePath)) {
      Log::mainLogger.info("FileContentHandler: file exists");
      File file = SPIFFS.open(filePath, "r");
      Log::mainLogger.info("FileContentHandler: file opened");
      server->streamFile(file, mime);
      Log::mainLogger.info("FileContentHandler: file sent");
      file.close();
    } else {
      Log::mainLogger.info("FileContentHandler: file not found");
      Error error("File not found");
      response->send(HTTP::CODE_NOT_FOUND, &error);
    }
  };
}
