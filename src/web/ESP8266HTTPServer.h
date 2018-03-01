#ifndef WEB_ESP_8266_HTTP_SERVER_H
#define WEB_ESP_8266_HTTP_SERVER_H

#include "../FileSystem.h"
#include "../Log.h"
#include "HTTPServer.h"
#include <ESP8266WebServer.h>
#include <detail/RequestHandlersImpl.h>

class ESP8266WebServerWrapper : public ESP8266WebServer {
public:
  ESP8266WebServerWrapper(IPAddress addr, int port = 80) : ESP8266WebServer(addr, port) {}
  ESP8266WebServerWrapper(int port = 80) : ESP8266WebServer(port) {}

  void send(http_code_t code, JsonObject *jsonObject);
  bool send(http_code_t code, JsonEntity *jsonEntity);
  using ESP8266WebServer::send;

  const char *getArg(const char *name);
  const char *getHeader(const char *name);
  const char *getURI();

  void setupResponseHeaders();
};

class ESP8266HTTPRequest : public HTTPRequest {
public:
  ESP8266HTTPRequest(ESP8266WebServerWrapper *_server) : server(_server) {}

  const char *getURI();
  HTTPRequest::Method getMethod();

  const char *getArg(const char *name);
  bool hasArg(const char *name);

  const char *getHeader(const char *name);
  bool hasHeader(const char *name);

protected:
  ESP8266WebServerWrapper *server;
};

class ESP8266HTTPResponse : public HTTPResponse {
public:
  ESP8266HTTPResponse(ESP8266WebServerWrapper *_server) : server(_server) {}

  void send(http_code_t code, const char *mime, const char *content);
  void send(http_code_t code, JsonObject *jsonObject);
  void send(http_code_t code, JsonEntity *jsonEntity);

  bool isSent() { return sent; }
  void reset() { sent = false; }

protected:
  ESP8266WebServerWrapper *server;
  bool sent = false;
};

class ESP8266HTTPServer : public HTTPServer {
public:
  ESP8266HTTPRequest *request;
  ESP8266HTTPResponse *response;

  void on(const char *uri, HTTPRequest::Method method, HTTPServerHandlerFunction handler);
  void on(const char *uri, const char *mime, const char *filePath);
  void on(const char *uri, HTTPServerHandlerFunction handler);

  void handleUpdate() { server->handleClient(); }

  ESP8266HTTPServer(IPAddress addr, uint16_t port = 80) {
    server = new ESP8266WebServerWrapper(addr, port);
    request = new ESP8266HTTPRequest(server);
    response = new ESP8266HTTPResponse(server);
    server->begin();
  }
  ESP8266HTTPServer(uint16_t port = 80) {
    server = new ESP8266WebServerWrapper(port);
    request = new ESP8266HTTPRequest(server);
    response = new ESP8266HTTPResponse(server);
    server->begin();
  }
  ~ESP8266HTTPServer() {
    server->close();

    delete request;
    delete response;
    delete server;
  }

  static HTTPMethod convertMethod(HTTPRequest::Method method);
  static HTTPRequest::Method convertMethod(HTTPMethod method);

  static HTTPServerHandlerFunction getFileContentHandler(ESP8266WebServerWrapper *server, const char *mime, const char *filePath);

protected:
  ESP8266WebServerWrapper *server;
};

class HTTPServerHandlerFunctionWrapper : public FunctionRequestHandler {
public:
  HTTPServerHandlerFunctionWrapper(ESP8266HTTPServer *_server, const char *_uri, HTTPRequest::Method _method, HTTPServer::HTTPServerHandlerFunction _handler);

  void handle();

protected:
  const HTTPServer::HTTPServerHandlerFunction handler;
  const ESP8266HTTPServer *server;
};

#endif
