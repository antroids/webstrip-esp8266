#ifndef IMPL_ESP_8266_HTTP_CLIENT_H
#define IMPL_ESP_8266_HTTP_CLIENT_H

#include "../web/HTTPClient.h"
#include <ESP8266HTTPClient.h>

#define ESP_8266_HTTP_CLIENT_RESPONSE_MAX_ERROR_SIZE 64
#define ESP_8266_HTTP_CLIENT_VALIDATION_ERROR -1000

class ESP8266HTTPClient;

class ESP8266HTTPClientResponse : public HTTPClientResponse {
public:
  ESP8266HTTPClientResponse(ESP8266HTTPClient *_client) : HTTPClientResponse(), client(_client) {}

  uint16_t code;
  char errorMessage[ESP_8266_HTTP_CLIENT_RESPONSE_MAX_ERROR_SIZE] = {0};

  uint16_t getCode();
  int getContentLength();
  Stream *getContentStream();
  int writeToStream(Stream *stream);
  bool isConnected();
  void close();
  const char *getErrorMessage();

protected:
  ESP8266HTTPClient *client;
};

class ESP8266HTTPClient : public WebStrip::HTTPClient {
public:
  class ESP8266HTTPRequestBuilder : public RequestBuilder {
  public:
    ESP8266HTTPRequestBuilder(ESP8266HTTPClient *_client) : client(_client), response(new ESP8266HTTPClientResponse(_client)) {}
    ~ESP8266HTTPRequestBuilder() { delete response; }

    RequestBuilder *url(const char *url);
    RequestBuilder *get();
    HTTPClientResponse *sendRequest();
    void clear();

  protected:
    ESP8266HTTPClient *client;
    ESP8266HTTPClientResponse *response;
    const char *_url = NULL;
    HTTPClient::Method method = GET;

    bool validate();
  };

  RequestBuilder *getRequestBuilder();

  ::HTTPClient clientImpl;

protected:
  ESP8266HTTPRequestBuilder builder = ESP8266HTTPRequestBuilder(this);
};

#endif /* end of include guard: IMPL_ESP_8266_HTTP_CLIENT_H */
